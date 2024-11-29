#include "cvi_tdl.h"
#include <inttypes.h>
#include <signal.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include "middleware.h"
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include "cvi_sys.h"
#include "robot_utils.h"



uint8_t face_filter(cvtdl_face_t *stFaceMeta)
{
    uint64_t face_area=0,max_face_area=0;
    uint8_t max=0;
    for(uint8_t i=0;i<stFaceMeta->size;i++)
    {
        face_area=(stFaceMeta->info[i].bbox.x2-stFaceMeta->info[i].bbox.x1)*
        (stFaceMeta->info[i].bbox.y2-stFaceMeta->info[i].bbox.y1);
        if(max_face_area<face_area)
        {
            max_face_area=face_area;
            max=i;

        }
    }
    return max;
    
}


MUTEXAUTOLOCK_INIT(LINUX_DATA_Mutex);
MUTEXAUTOLOCK_INIT(FACE_DATA_Mutex);

static volatile uint8_t gVideoCapture=0x01;
static volatile uint8_t gMailbox=0x01;

static volatile uint64_t gLinux_data=0;     //linux数据(包括人脸数据与控制位)
static volatile uint32_t gFace_data=0;      //人脸位置数据



const char *fd_model_path = "/root/scrfd_480_270_int8.cvimodel";  
CVI_TDL_SUPPORTED_MODEL_E fd_model_id = CVI_TDL_SUPPORTED_MODEL_SCRFDFACE;

void *Video_Capture(void)
{
    int ret=0,count=0;
    cvitdl_handle_t stCVIHandle = NULL;
    cvitdl_service_handle_t stServiceHandle = NULL;
    VIDEO_FRAME_INFO_S stFrame,*stNewFrame; 
    cvtdl_face_t stFaceMeta = {0};
    cvtdl_image_t stImage={0};

    SAMPLE_TDL_MW_CONTEXT stMWContext = {0};
	if(MiddleWare_Init(&stMWContext)) return;
    GOTO_IF_FAILED(CVI_TDL_CreateHandle2(&stCVIHandle,1,0),ret,FREE_MIDDLEWARE); 
    GOTO_IF_FAILED(CVI_TDL_SetVBPool(stCVIHandle, 0, 2),ret,FREE_CVI_HANDLE); 
    GOTO_IF_FAILED(CVI_TDL_SetVpssTimeout(stCVIHandle, 1000),ret,FREE_CVI_HANDLE); 
    GOTO_IF_FAILED(CVI_TDL_Service_CreateHandle(&stServiceHandle,stCVIHandle),ret,FREE_CVI_HANDLE);
    GOTO_IF_FAILED(CVI_TDL_OpenModel(stCVIHandle,fd_model_id,fd_model_path),ret,FREE_CVI_SERVICE_HANDLE); 

    CVI_TDL_CreateImage(&stImage,IMAGE_HEIGHT,IMAGE_WIDTH,PIXEL_FORMAT_RGB_888);

    while(gVideoCapture)
    {
        GOTO_IF_FAILED(CVI_VPSS_GetChnFrame(0,VPSS_CHN0, &stFrame, 2000),ret,FREE_FACE); 
        GOTO_IF_FAILED(CVI_TDL_ScrFDFace(stCVIHandle, &stFrame, &stFaceMeta),ret,FREE_FRAME); 


        if(0x02==gVideoCapture)
        {

            if(++ count > 5) count=0;   //先自增后判断
            char *filename = calloc(64, sizeof(char));
            sprintf(filename, "/root/photo/photo%d.bmp",count);
            CVI_TDL_Change_Img(stCVIHandle,fd_model_id,&stFrame,&stNewFrame,PIXEL_FORMAT_RGB_888);
		    CVI_TDL_CopyVpssImage(stNewFrame,&stImage);
            stbi_write_bmp(filename,1280,720,STBI_rgb,stImage.pix[0]);
            printf("write bmp%d ok\r\n",count);
            free(filename);
            CVI_TDL_Release_VideoFrame(stCVIHandle,fd_model_id,stNewFrame,true);
            gVideoCapture=0x01;
        }

        if(stFaceMeta.size>0)
        {
            uint16_t face_x;
            uint16_t face_y;
            printf("                                       \r\n");
            printf("face num=%d\r\n",stFaceMeta.size);
            uint8_t max=face_filter(&stFaceMeta);
            face_x=(uint16_t)(stFaceMeta.info[max].bbox.x1+stFaceMeta.info[max].bbox.x2)>>1;
            face_y=(uint16_t)(stFaceMeta.info[max].bbox.y1+stFaceMeta.info[max].bbox.y2)>>1;


            {

                MutexAutoLock(FACE_DATA_Mutex, lock);
                gFace_data=face_y <<16 | face_x;    

            }
             
                
            


        }
       

    FREE_FRAME:
        CVI_VPSS_ReleaseChnFrame(0,VPSS_CHN0, &stFrame); 
    FREE_FACE:
        CVI_TDL_Free(&stFaceMeta); 
        if(ret) gVideoCapture = 0x00;
        
    }
    FREE_CVI_SERVICE_HANDLE:
        CVI_TDL_Service_DestroyHandle(stServiceHandle);
    FREE_CVI_HANDLE:
        CVI_TDL_DestroyHandle(stCVIHandle);
    FREE_MIDDLEWARE:
        SAMPLE_TDL_Destroy_MW(&stMWContext);


}


void *Mailbox_Comm(void *pHandle)
{
    int fd=0,ret=0;
    uint64_t u64PhyAddr = 0;    //物理地址
	void *pVirAddr=NULL;        //虚拟地址
    uint8_t *oled_font=NULL;

    uint32_t face_data=0;
    uint8_t control_data;

    fd = open(RTOS_CMDQU_DEV_NAME,  O_RDWR| O_DSYNC);
    if(fd <= 0)
    {
        printf("open /dev/cvi-rtos-cmdqu failed!\n");
        return;
    }

    ret = CVI_SYS_Init();
    TRY_IF_FAILED(CVI_SYS_IonAlloc(&u64PhyAddr, &pVirAddr, "oled_buf",OLED_FONT_SIZE),ret,10);
	if (ret) {
		goto ION_ERROR;
	}

    struct cmdqu_t cmd ={0};
    cmd.ip_id = 0;
    cmd.resv.mstime = 100;
    cmd.block = 1;

    while(gMailbox)
    {

        /**
         * mailbox发送
         */

        cmd.cmd_id =CMD_TEST_C;

        {
            MutexAutoLock(FACE_DATA_Mutex, lock);
            face_data=gFace_data;
            
        }
        printf("face_x=%d\r\n",face_data&0xFFFF);
        printf("face_y=%d\r\n",face_data>>16);


   
        cmd.param_ptr=face_data;
        TRY_IF_FAILED(ioctl(fd , RTOS_CMDQU_SEND_WAIT, &cmd),ret,10);
        if(ret < 0) goto IOCTL_ERROR;

       

        /**
         * mailbox接收
         */

        usleep(20000);

     
    }

IOCTL_ERROR:
    TRY_IF_FAILED(CVI_SYS_IonFree(u64PhyAddr, pVirAddr),ret,10);
ION_ERROR:
    close(fd);
    return 0;

}



static void SampleHandleSig(CVI_S32 signo) {
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  if (SIGINT == signo || SIGTERM == signo) {

    gVideoCapture=0x00;
    gMailbox=0x00;
      
  }
}


int main(int argc, char *argv[])
{
    int ret=0;
    signal(SIGINT, SampleHandleSig);
	signal(SIGTERM, SampleHandleSig);


    pthread_t stVideoThread,stMailboxThread;
    pthread_create(&stVideoThread, NULL,Video_Capture,NULL);
    pthread_create(&stMailboxThread, NULL,Mailbox_Comm,NULL);
  
    pthread_join(stVideoThread, NULL);
    pthread_join(stMailboxThread, NULL);

    return 0;

	
}

