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


int MiddleWare_Config(SAMPLE_TDL_MW_CONFIG_S *MWConfig, SAMPLE_TDL_MW_CONTEXT *MWContext)
{
  
  int ret = SAMPLE_TDL_Get_VI_Config(&MWConfig->stViConfig);   //默认设置VI配置
  if (ret != CVI_SUCCESS || MWConfig->stViConfig.s32WorkingViNum <= 0) {
    printf("Failed to get senor infomation from ini file (/mnt/data/sensor_cfg.ini).\n");
    return -1;
  }

  //获取VI尺寸
  PIC_SIZE_E enPicSize;
  ret = SAMPLE_COMM_VI_GetSizeBySensor(MWConfig->stViConfig.astViInfo[0].stSnsInfo.enSnsType,
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

  MWConfig->stVBPoolConfig.u32VBPoolCount = 3;

  // VBPool 0 for VPSS Grp0 Chn0
  MWConfig->stVBPoolConfig.astVBPoolSetup[0].enFormat = VI_PIXEL_FORMAT;
  MWConfig->stVBPoolConfig.astVBPoolSetup[0].u32BlkCount = 2;
  MWConfig->stVBPoolConfig.astVBPoolSetup[0].u32Height = stSensorSize.u32Height;
  MWConfig->stVBPoolConfig.astVBPoolSetup[0].u32Width = stSensorSize.u32Width;
  MWConfig->stVBPoolConfig.astVBPoolSetup[0].bBind = true;
  MWConfig->stVBPoolConfig.astVBPoolSetup[0].u32VpssChnBinding = VPSS_CHN0;
  MWConfig->stVBPoolConfig.astVBPoolSetup[0].u32VpssGrpBinding = (VPSS_GRP)0;

  // VBPool 1 for VPSS Grp0 Chn1
  MWConfig->stVBPoolConfig.astVBPoolSetup[1].enFormat = VI_PIXEL_FORMAT;
  MWConfig->stVBPoolConfig.astVBPoolSetup[1].u32BlkCount = 2;
  MWConfig->stVBPoolConfig.astVBPoolSetup[1].u32Height = stVencSize.u32Height;
  MWConfig->stVBPoolConfig.astVBPoolSetup[1].u32Width = stVencSize.u32Width;
  MWConfig->stVBPoolConfig.astVBPoolSetup[1].bBind = true;
  MWConfig->stVBPoolConfig.astVBPoolSetup[1].u32VpssChnBinding = VPSS_CHN1;
  MWConfig->stVBPoolConfig.astVBPoolSetup[1].u32VpssGrpBinding = (VPSS_GRP)0;

  // VBPool 2 for TDL preprocessing
  MWConfig->stVBPoolConfig.astVBPoolSetup[2].enFormat = PIXEL_FORMAT_BGR_888_PLANAR;
  MWConfig->stVBPoolConfig.astVBPoolSetup[2].u32BlkCount = 1;
  MWConfig->stVBPoolConfig.astVBPoolSetup[2].u32Height = 720;
  MWConfig->stVBPoolConfig.astVBPoolSetup[2].u32Width = 1280;
  MWConfig->stVBPoolConfig.astVBPoolSetup[2].bBind = false;

  // Setup VPSS Grp0
  MWConfig->stVPSSPoolConfig.u32VpssGrpCount = 1;
  MWConfig->stVPSSPoolConfig.stVpssMode.aenInput[0] = VPSS_INPUT_MEM;
  MWConfig->stVPSSPoolConfig.stVpssMode.enMode = VPSS_MODE_DUAL;
  MWConfig->stVPSSPoolConfig.stVpssMode.ViPipe[0] = 0;
  MWConfig->stVPSSPoolConfig.stVpssMode.aenInput[1] = VPSS_INPUT_ISP;
  MWConfig->stVPSSPoolConfig.stVpssMode.ViPipe[1] = 0;


  SAMPLE_TDL_VPSS_CONFIG_S *pstVpssConfig = &MWConfig->stVPSSPoolConfig.astVpssConfig[0];
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

  // 设置VENC默认配置
  SAMPLE_TDL_Get_Input_Config(&MWConfig->stVencConfig.stChnInputCfg);
  MWConfig->stVencConfig.u32FrameWidth = stVencSize.u32Width;
  MWConfig->stVencConfig.u32FrameHeight = stVencSize.u32Height;

  // 设置RTSP默认配置
  SAMPLE_TDL_Get_RTSP_Config(&MWConfig->stRTSPConfig.stRTSPConfig);

  ret = SAMPLE_TDL_Init_WM(MWConfig, MWContext);    //初始化Middleware
  if (ret != CVI_SUCCESS) {
    printf("init middleware failed! ret=%x\n", ret);
    return -1;
  }

  return 0;

}
