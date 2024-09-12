/**
 * 暂时实现不了
 * 怀疑是模型问题
 * 
 */


#include "cvi_tdl.h"
#include "cvi_tdl_app.h"
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include "middleware.h"

static volatile bool bExit = false;
static void SampleHandleSig(CVI_S32 signo) {
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  if (SIGINT == signo || SIGTERM == signo) {
    bExit = true;
  }
}

int main(int argc, char *argv[]) {

  signal(SIGINT, SampleHandleSig);
  signal(SIGTERM, SampleHandleSig);

  const char *fd_model_path = "/root/scrfd_768_432_int8_1x.cvimodel";  
  const char *fr_model_path = "/root/cviface-v6-s.cvimodel"; 
  CVI_TDL_SUPPORTED_MODEL_E fd_model_id = CVI_TDL_SUPPORTED_MODEL_SCRFDFACE;
  CVI_TDL_SUPPORTED_MODEL_E fr_model_id = CVI_TDL_SUPPORTED_MODEL_FACERECOGNITION;
  int ret=0;
 
  SAMPLE_TDL_MW_CONTEXT stMWContext = {0};
  ret = MiddleWare_Init(&stMWContext);
  if (ret != CVI_TDL_SUCCESS) {
    printf("system init failed with %#x!\n", ret);
    goto CLEANUP_SYSTEM;
  }

  cvitdl_handle_t cvi_handle = NULL;
  cvitdl_service_handle_t cvi_service_handle = NULL;

  if(CVI_TDL_CreateHandle2(&cvi_handle,1,0)) goto CLEANUP_SYSTEM;
  if(CVI_TDL_SetVpssTimeout(cvi_handle, 1000)) goto CLEANUP_SYSTEM;
  if(CVI_TDL_SetVBPool(cvi_handle, 0, 2)) goto CLEANUP_SYSTEM;
  if(CVI_TDL_Service_CreateHandle(&cvi_service_handle, cvi_handle)) goto CLEANUP_SYSTEM2;
  if(CVI_TDL_OpenModel(cvi_handle,fd_model_id,fd_model_path)) goto CLEANUP_SYSTEM2;
  if(CVI_TDL_OpenModel(cvi_handle,fr_model_id,fr_model_path)) goto CLEANUP_SYSTEM2;

  VIDEO_FRAME_INFO_S Frame;
  cvtdl_face_t Facedata = {0};
  memset(&Facedata, 0, sizeof(cvtdl_face_t));
  while(bExit == false)
  {
    
    ret = CVI_VPSS_GetChnFrame(0, 0, &Frame, 2000);  
    if (ret) {  
      printf("CVI_VPSS_GetChnFrame chn0 failed\n");
      goto ERROR;
    }

    ret = CVI_TDL_ScrFDFace(cvi_handle, &Frame, &Facedata);
    if (ret) {
      printf("inference failed!, ret=%x\n",ret);
      goto ERROR;
    }
    printf("face num=%d\r\n",Facedata.size);

    ret |= CVI_TDL_FaceRecognition(cvi_handle, &Frame, &Facedata);
    if (ret != CVI_SUCCESS) {
      printf("failed to run face face reg\n");
      return ret;
    }

    // ret = CVI_TDL_Service_FaceDrawRect(cvi_service_handle, &Facedata, &Frame, false,
    //                                       CVI_TDL_Service_GetDefaultBrush());

    // ret = SAMPLE_TDL_Send_Frame_RTSP(&Frame,&stMWContext);
    // if (ret) {
    //   printf("Send Output Frame NG\n");
    //   goto ERROR;
    // }

    ERROR:   
      CVI_VPSS_ReleaseChnFrame(0, 0, &Frame); 
      CVI_TDL_Free(&Facedata); 
      if(ret) {
        bExit = true;
      }
  }

CLEANUP_SYSTEM2:
  CVI_TDL_Service_DestroyHandle(cvi_service_handle);
CLEANUP_SYSTEM:
  CVI_VPSS_ReleaseChnFrame(0, 0, &Frame); 
  CVI_TDL_Free(&Facedata); 
  CVI_TDL_DestroyHandle(cvi_handle);
  SAMPLE_TDL_Destroy_MW(&stMWContext);
  CVI_SYS_Exit();
  CVI_VB_Exit();
}






