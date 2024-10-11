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
static volatile bool save_png = false;

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

	const char *fd_model_path = "/root/scrfd_480_270_int8.cvimodel";  
	CVI_TDL_SUPPORTED_MODEL_E fd_model_id = CVI_TDL_SUPPORTED_MODEL_SCRFDFACE;
	
	int ret=0;
 
	SAMPLE_TDL_MW_CONTEXT stMWContext = {0};
	ret = MiddleWare_Init(&stMWContext);
	if (ret != CVI_TDL_SUCCESS) {
		printf("system init failed with %#x!\n", ret);
		goto CLEANUP_SYSTEM;
	}

	cvitdl_handle_t cvi_handle = NULL;

	if(CVI_TDL_CreateHandle2(&cvi_handle,1,0)) goto CLEANUP_SYSTEM;
	if(CVI_TDL_SetVpssTimeout(cvi_handle, 1000)) goto CLEANUP_SYSTEM;
	if(CVI_TDL_SetVBPool(cvi_handle, 0, 2)) goto CLEANUP_SYSTEM;
	if(CVI_TDL_OpenModel(cvi_handle,fd_model_id,fd_model_path)) goto CLEANUP_SYSTEM;

	VIDEO_FRAME_INFO_S Frame;
	VIDEO_FRAME_INFO_S *Dst_Frame;
	cvtdl_face_t Facedata = {0};
	memset(&Facedata, 0, sizeof(cvtdl_face_t));
	cvtdl_image_t image={0};
	memset(&image, 0, sizeof(cvtdl_image_t));
	CVI_TDL_CreateImage(&image,720,1280,PIXEL_FORMAT_RGB_888);

	char filename[512];
	sprintf(filename, "/root/hello.bmp");

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

		CVI_TDL_Change_Img(cvi_handle,fd_model_id,&Frame, &Dst_Frame,PIXEL_FORMAT_RGB_888);
		CVI_TDL_CopyVpssImage(Dst_Frame,&image);

		if(Facedata.size>=1)
		{
			stbi_write_bmp(filename,1280,720,STBI_rgb,image.pix[0]);
			printf("write bmp ok\r\n");
		}

		ERROR:   
		CVI_TDL_Free(&Facedata); 
		CVI_TDL_Release_VideoFrame(cvi_handle,fd_model_id,Dst_Frame,true);
		CVI_VPSS_ReleaseChnFrame(0, 0, &Frame); 
		if(ret) {
			bExit = true;
		}
	}


CLEANUP_SYSTEM:
  CVI_VPSS_ReleaseChnFrame(0, 0, &Frame); 
  CVI_TDL_Free(&Facedata); 
  CVI_TDL_DestroyHandle(cvi_handle);
  SAMPLE_TDL_Destroy_MW(&stMWContext);
  CVI_SYS_Exit();
  CVI_VB_Exit();
}



