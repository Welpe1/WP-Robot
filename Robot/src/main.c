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
#include "mood_system.h"
#include "oledfont.h"


MUTEXAUTOLOCK_INIT(LINUX_DATA_Mutex);
MUTEXAUTOLOCK_INIT(FACE_DATA_Mutex);

static volatile bool bExit = false;

static volatile uint8_t gVideoCapture=0x00;
static volatile uint8_t gMailbox=0x01;
static volatile uint8_t gControl=0x01;

static volatile uint8_t gLinux_data=0;     //linux数据(包括人脸数据与控制位)
static volatile uint16_t gFace_data=0;      //人脸位置数据



const char *fd_model_path = "/root/scrfd_480_270_int8.cvimodel";  
CVI_TDL_SUPPORTED_MODEL_E fd_model_id = CVI_TDL_SUPPORTED_MODEL_YOLOV5;

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
            uint8_t face_x;
            uint8_t face_y;
            // face_data=(uint8_t)(stFaceMeta.info[0].bbox.x1+stFaceMeta.info[0].bbox.x2)>>1;
            // (stFaceMeta.info[0].bbox.y1+stFaceMeta.info[0].bbox.y2)>>1;

            MutexAutoLock(FACE_DATA_Mutex, lock);
            {
                gFace_data=face_x <<8 + face_y;     
                
            }


        }
       

    FREE_FRAME:
        CVI_VPSS_ReleaseChnFrame(0,VPSS_CHN0, &stFrame); 
    FREE_FACE:
        CVI_TDL_Free(&stFaceMeta); 
    FREE_CVI_SERVICE_HANDLE:
        CVI_TDL_Service_DestroyHandle(stServiceHandle);
    FREE_CVI_HANDLE:
        CVI_TDL_DestroyHandle(stCVIHandle);
    FREE_MIDDLEWARE:
        SAMPLE_TDL_Destroy_MW(&stMWContext);
        if(ret) gVideoCapture = false;
        
    }
}


void *Mailbox_Comm(void *pHandle)
{
    int fd=0,ret=0;
    uint64_t u64PhyAddr = 0;    //物理地址
	void *pVirAddr=NULL;        //虚拟地址
    uint8_t *oled_font=NULL;

    uint16_t face_data=0;
    uint8_t control_data;

    fd = open(RTOS_CMDQU_DEV_NAME,  O_RDWR| O_DSYNC);
    if(fd <= 0)
    {
        printf("open /dev/cvi-rtos-cmdqu failed!\n");
        return;
    }

    ret = CVI_SYS_Init();
    TRY_IF_FAILED(CVI_SYS_IonAlloc(&u64PhyAddr, &pVirAddr, "shared memory",OLED_FONT_SIZE),ret,10);
	if (ret) {
		goto ION_ERROR;
	}

    struct cmdqu_t cmd ={0};
    cmd.ip_id = 0;
    cmd.cmd_id =CMD_TEST_B;
    cmd.resv.mstime = 100;
    cmd.block = 1;

    while(cmd.param_ptr != u64PhyAddr+1)
    {
        cmd.param_ptr = u64PhyAddr;
        ret = ioctl(fd , RTOS_CMDQU_SEND, &cmd);
        if(ret < 0) goto IOCTL_ERROR;
    }


    struct Mood_Operation stMood_Operation={0};
    struct Mood_Info stMood_Info={0};
    mood_system_init(&stMood_Operation,&stMood_Info);


    while(gMailbox)
    {

        /**
         * mailbox发送
         */

        cmd.cmd_id =CMD_TEST_C;
        //oled_font=bmp1;
        memcpy(pVirAddr,emo_static_group[stMood_Info.stEMO_Info.select][128*8],OLED_FONT_SIZE);  


        MutexAutoLock(FACE_DATA_Mutex, lock);
        {
            face_data=gFace_data;
            
        }

        cmd.param_ptr=(face_data << 16) | control_data;
        TRY_IF_FAILED(ioctl(fd , RTOS_CMDQU_SEND_WAIT, &cmd),ret,10);
        if(ret < 0) goto IOCTL_ERROR;

        /**
         * mailbox接收
         */

        stMood_Info.event_trigger=(uint16_t)cmd.param_ptr;
        if(count_bit(stMood_Info.event_trigger) % 2 !=0) continue;//偶校验不通过

        stMood_Operation.mood_update(&stMood_Info);     
        if(stMood_Info.event_trigger & (0x01 << 3))
        {
            gVideoCapture=0x02;
        }
    
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
      bExit = true;
  }
}


int main(int argc, char *argv[])
{
    int ret=0;
    signal(SIGINT, SampleHandleSig);
	signal(SIGTERM, SampleHandleSig);



    pthread_t stVideoThread,stMailboxThread,stMoodThread;
    pthread_create(&stVideoThread, NULL,Video_Capture,NULL);
    pthread_create(&stMailboxThread, NULL,Mailbox_Comm,NULL);
  
    pthread_join(stVideoThread, NULL);
    pthread_join(stMailboxThread, NULL);
    pthread_join(stMoodThread, NULL);

	

}

