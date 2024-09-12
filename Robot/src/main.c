#include "cvi_tdl.h"
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

	const char *fd_model_path = "/root/yolov5n_int8_fuse.cvimodel";  
	CVI_TDL_SUPPORTED_MODEL_E fd_model_id = CVI_TDL_SUPPORTED_MODEL_YOLOV5;
	
    int ret=0;
	SAMPLE_TDL_MW_CONTEXT stMWContext = {0};
	ret = MiddleWare_Init(&stMWContext);
	if (ret != CVI_TDL_SUCCESS) {
		printf("system init failed with %#x!\n", ret);
		goto CLEANUP_SYSTEM;
	}

     printf("step-1\r\n");


	cvitdl_handle_t cvi_handle = NULL;
	cvitdl_service_handle_t cvi_service_handle = NULL;

	if(CVI_TDL_CreateHandle2(&cvi_handle,1,0)) goto CLEANUP_SYSTEM;
	if(CVI_TDL_SetVpssTimeout(cvi_handle, 1000)) goto CLEANUP_SYSTEM;
	if(CVI_TDL_SetVBPool(cvi_handle, 0, 2)) goto CLEANUP_SYSTEM;
	if(CVI_TDL_Service_CreateHandle(&cvi_service_handle, cvi_handle)) goto CLEANUP_SYSTEM2;
  
    printf("step0\r\n");

	ret=CVI_TDL_OpenModel(cvi_handle,fd_model_id,fd_model_path);
    printf("ret=%d\r\n",ret);


        printf("step000000000000000\r\n");

    CVI_TDL_SetModelThreshold(cvi_handle,fd_model_id, 0.5);
    CVI_TDL_SetModelNmsThreshold(cvi_handle,fd_model_id, 0.5);
    printf("step1\r\n");

	VIDEO_FRAME_INFO_S Frame;
    VIDEO_FRAME_INFO_S *Dst_Frame;
    cvtdl_object_t obj_data = {0};
	memset(&obj_data, 0, sizeof(cvtdl_object_t));

    printf("step2\r\n");

	while(bExit == false)
	{
		ret = CVI_VPSS_GetChnFrame(0, 0, &Frame, 2000);  
		if (ret) {  
			printf("CVI_VPSS_GetChnFrame chn0 failed\n");
			goto ERROR;
		}
        printf("step3\r\n");

        
		CVI_TDL_Change_Img(cvi_handle,fd_model_id,&Frame, &Dst_Frame,PIXEL_FORMAT_RGB_888);

        ret = CVI_TDL_Yolov5(cvi_handle,Dst_Frame, &obj_data);
		if (ret) {
			printf("inference failed!, ret=%x\n",ret);
			goto ERROR;
		}
        printf("step4\r\n");

        for (uint32_t i = 0; i < obj_data.size; i++) {
            printf("detect res: %f %f %f %f %f %d\n", obj_data.info[i].bbox.x1, obj_data.info[i].bbox.y1,
           obj_data.info[i].bbox.x2, obj_data.info[i].bbox.y2, obj_data.info[i].bbox.score,
           obj_data.info[i].classes);
            }
        printf("step5\r\n");

		ERROR:   
		CVI_TDL_Free(&obj_data); 
		CVI_VPSS_ReleaseChnFrame(0, 0, &Frame); 
		if(ret) {
			bExit = true;
		}
	}

CLEANUP_SYSTEM2:
  CVI_TDL_Service_DestroyHandle(cvi_service_handle);
CLEANUP_SYSTEM:
  CVI_VPSS_ReleaseChnFrame(0, 0, &Frame); 
  CVI_TDL_Free(&obj_data); 
  CVI_TDL_DestroyHandle(cvi_handle);
  SAMPLE_TDL_Destroy_MW(&stMWContext);
  CVI_SYS_Exit();
  CVI_VB_Exit();
}



