//第一版 测试版

#include "middleware_utils.h"
#include "sample_utils.h"
#include "vi_vo_utils.h"

#include <core/utils/vpss_helper.h>
#include <cvi_comm.h>
#include <rtsp.h>
#include <sample_comm.h>
#include "cvi_tdl.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * CVITEK媒体处理平台的主要内部处理流程
 * 
 * 摄像头-->VI-->VPSS-->VENC-->h264格式
 * 
 * VI(Video Input影像输入) 负责从摄像头获取图像
 * VPSS(Video Process Sub-System图像处理子系统) 对图像进行处理（如rop、Scale 、像素格式转换、固定角度旋转等）
 * VENC(Video Encoder影像编码) 对视频编码（支持格式有JPEG、MJPEG、H.264、H.265）
 * 
 */


static volatile bool bExit = false;

typedef struct {
  SAMPLE_TDL_MW_CONTEXT *pstMWContext;    //Middleware上下文结构体
  cvitdl_service_handle_t stServiceHandle;    //TDL SDK Service句柄
  pthread_mutex_t Mutex;    //互斥锁
}VideoCapture_Args;     //视频捕获线程结构体

//视频捕获线程
void *VideoCapture(void *args) {
  int ret=0;
  printf("Enter encoder thread\n");
  VideoCapture_Args *pstArgs = (VideoCapture_Args *)args;
  VIDEO_FRAME_INFO_S stFrame;   //视频帧信息结构体
  
  while (bExit == false) {

    ret = CVI_VPSS_GetChnFrame(0, 0, &stFrame, 2000);  
    if (ret) {  
      printf("CVI_VPSS_GetChnFrame chn0 failed\n");
      goto ERROR;
    }

    ret = SAMPLE_TDL_Send_Frame_RTSP(&stFrame, pstArgs->pstMWContext);
    if (ret) {
      printf("Send Output Frame NG");
      goto ERROR;
    }

    ERROR:    //这个ERROR会一直执行，不一定需要goto语句满足条件
      CVI_VPSS_ReleaseChnFrame(0, 0, &stFrame);
      
      if(ret) {
        pthread_mutex_lock(&pstArgs->Mutex);    //上锁
        bExit = true;
        pthread_mutex_unlock(&pstArgs->Mutex);    //解锁
      }
  }
  printf("Exit encoder thread\n");
  pthread_exit(NULL);
}


static void SampleHandleSig(int signo) {
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  printf("handle signal, signo: %d\n", signo);
  if (SIGINT == signo || SIGTERM == signo) {
    bExit = true;
  }
}


//Middleware基础配置
int MiddleWare_Config(SAMPLE_TDL_MW_CONFIG_S *stMWConfig, SAMPLE_TDL_MW_CONTEXT *stMWContext)
{
  
  int ret = SAMPLE_TDL_Get_VI_Config(&stMWConfig->stViConfig);   //默认设置VI配置
  if (ret != CVI_SUCCESS || stMWConfig->stViConfig.s32WorkingViNum <= 0) {
    printf("Failed to get senor infomation from ini file (/mnt/data/sensor_cfg.ini).\n");
    return -1;
  }

  //获取VI尺寸
  PIC_SIZE_E enPicSize;
  ret = SAMPLE_COMM_VI_GetSizeBySensor(stMWConfig->stViConfig.astViInfo[0].stSnsInfo.enSnsType,
                                          &enPicSize);
  if (ret != CVI_SUCCESS) {
    printf("Cannot get senor size\n");
    return -1;
  }

  //获取图像尺寸
  SIZE_S stSensorSize;
  ret = SAMPLE_COMM_SYS_GetPicSize(enPicSize, &stSensorSize);
  if (ret != CVI_SUCCESS) {
    printf("Cannot get senor size\n");
    return -1;
  }

  //设置编码器图像尺寸
  SIZE_S stVencSize = {
      .u32Width = 1280,
      .u32Height = 720,
  };

  stMWConfig->stVBPoolConfig.u32VBPoolCount = 3;

  // VBPool 0 for VPSS Grp0 Chn0
  stMWConfig->stVBPoolConfig.astVBPoolSetup[0].enFormat = VI_PIXEL_FORMAT;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[0].u32BlkCount = 2;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[0].u32Height = stSensorSize.u32Height;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[0].u32Width = stSensorSize.u32Width;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[0].bBind = true;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[0].u32VpssChnBinding = VPSS_CHN0;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[0].u32VpssGrpBinding = (VPSS_GRP)0;

  // VBPool 1 for VPSS Grp0 Chn1
  stMWConfig->stVBPoolConfig.astVBPoolSetup[1].enFormat = VI_PIXEL_FORMAT;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[1].u32BlkCount = 2;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[1].u32Height = stVencSize.u32Height;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[1].u32Width = stVencSize.u32Width;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[1].bBind = true;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[1].u32VpssChnBinding = VPSS_CHN1;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[1].u32VpssGrpBinding = (VPSS_GRP)0;

  // VBPool 2 for TDL preprocessing
  stMWConfig->stVBPoolConfig.astVBPoolSetup[2].enFormat = PIXEL_FORMAT_BGR_888_PLANAR;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[2].u32BlkCount = 1;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[2].u32Height = 720;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[2].u32Width = 1280;
  stMWConfig->stVBPoolConfig.astVBPoolSetup[2].bBind = false;

  // Setup VPSS Grp0
  stMWConfig->stVPSSPoolConfig.u32VpssGrpCount = 1;
  stMWConfig->stVPSSPoolConfig.stVpssMode.aenInput[0] = VPSS_INPUT_MEM;
  stMWConfig->stVPSSPoolConfig.stVpssMode.enMode = VPSS_MODE_DUAL;
  stMWConfig->stVPSSPoolConfig.stVpssMode.ViPipe[0] = 0;
  stMWConfig->stVPSSPoolConfig.stVpssMode.aenInput[1] = VPSS_INPUT_ISP;
  stMWConfig->stVPSSPoolConfig.stVpssMode.ViPipe[1] = 0;


  SAMPLE_TDL_VPSS_CONFIG_S *pstVpssConfig = &stMWConfig->stVPSSPoolConfig.astVpssConfig[0];
  pstVpssConfig->bBindVI = true;

  // Assign device 1 to VPSS Grp0, because device1 has 3 outputs in dual mode.
  VPSS_GRP_DEFAULT_HELPER2(&pstVpssConfig->stVpssGrpAttr, stSensorSize.u32Width,
                           stSensorSize.u32Height, VI_PIXEL_FORMAT, 1);
  pstVpssConfig->u32ChnCount = 2;
  pstVpssConfig->u32ChnBindVI = 0;
  VPSS_CHN_DEFAULT_HELPER(&pstVpssConfig->astVpssChnAttr[0], stVencSize.u32Width,
                          stVencSize.u32Height, VI_PIXEL_FORMAT, true);
  VPSS_CHN_DEFAULT_HELPER(&pstVpssConfig->astVpssChnAttr[1], stVencSize.u32Width,
                          stVencSize.u32Height, VI_PIXEL_FORMAT, true);

  // Get default VENC configurations
  SAMPLE_TDL_Get_Input_Config(&stMWConfig->stVencConfig.stChnInputCfg);
  stMWConfig->stVencConfig.u32FrameWidth = stVencSize.u32Width;
  stMWConfig->stVencConfig.u32FrameHeight = stVencSize.u32Height;

  // Get default RTSP configurations
  SAMPLE_TDL_Get_RTSP_Config(&stMWConfig->stRTSPConfig.stRTSPConfig);

  ret = SAMPLE_TDL_Init_WM(stMWConfig, stMWContext);    //初始化Middleware
  if (ret != CVI_SUCCESS) {
    printf("init middleware failed! ret=%x\n", ret);
    return -1;
  }

  return 0;

}

int main(int argc, char *argv[]) {

  /**
   * TDL SDK主要分为三大模块：Core Service Application
   * Core主要提供算法相关接口，封装复杂的底层操作及算法细节。令使用者可以直接使用VI或VPSS取得的Video Frame Buffer进行模型推理
   * Service提供算法相关辅助API（如绘图, 特征比对, 区域入侵判定等功能）
   * Application封装应用逻辑，目前包含人脸抓拍的应用逻辑
   *
   * cvitdl_handle_t TDL SDK Core句柄
   * cvitdl_service_handle_t TDL SDK service句柄
   * 创建 cvitdl_service_handle_t模块时会需要使用到cvitdl_handle_t作为输入
   * 
   * 
   */
  SAMPLE_TDL_MW_CONFIG_S stMWConfig = {0};    //Middleware结构体
  SAMPLE_TDL_MW_CONTEXT stMWContext = {0};    //Middleware上下文结构体
  cvitdl_handle_t stTDLHandle = NULL;
  cvitdl_service_handle_t stServiceHandle = NULL;
  VideoCapture_Args video_thread_args = {
      .pstMWContext = &stMWContext,
      .stServiceHandle = stServiceHandle,
  };


  MiddleWare_Config(&stMWConfig,&stMWContext);

  signal(SIGINT, SampleHandleSig);    //用户在终端中按下Ctrl+C时，操作系统会向当前运行的进程发送SIGINT信号终止进程
  signal(SIGTERM, SampleHandleSig);
  
  /**
   * CVI_TDL_CreateHandle2
   * 创建使用TDL SDK句柄，并使用指定的VPSS Group ID及Dev ID创建VPSS
   */
  if(CVI_TDL_CreateHandle2(&stTDLHandle,1,0)) goto create_tdl_fail;
  /**
   * CVI_TDL_SetVBPool
   * 指定VBPool给TDL SDK内部VPSS。指定后，TDL SDK内部VPSS会直接从此Pool拿取内存
   * 若不用此 API 指定 Pool，默认由系统自动分配。
   */
  if(CVI_TDL_SetVBPool(stTDLHandle, 0, 2)) goto create_service_fail;
  /**
   * CVI_TDL_SetVpssTimeout
   * 设置TDL SDK等待VPSS硬件超时的时间，预设为100ms
   * 此设置适用于所有 TDL SDK 内的 VPSS Thread。
   */
  CVI_TDL_SetVpssTimeout(stTDLHandle, 1000);
  /**
   * CVI_TDL_Service_CreateHandle
   * 创建 Service 句柄
   */
  if(CVI_TDL_Service_CreateHandle(&stServiceHandle, stTDLHandle)) goto create_service_fail;



  pthread_t videocapture_thread;
  pthread_mutex_init(&video_thread_args.Mutex,NULL);
  pthread_create(&videocapture_thread, NULL, VideoCapture, &video_thread_args);
  pthread_join(videocapture_thread, NULL);
  pthread_mutex_destroy(&video_thread_args.Mutex);

create_service_fail:
  CVI_TDL_DestroyHandle(stTDLHandle);
create_tdl_fail:
  SAMPLE_TDL_Destroy_MW(&stMWContext);


  return 0;
}

