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

#define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif

MUTEXAUTOLOCK_INIT(LINUX_DATA_Mutex);
MUTEXAUTOLOCK_INIT(FACE_DATA_Mutex);

/**
 * linux_data
 * [0]:人体存在标志位
 * [1]:短触摸(happy)
 * [2]:长触摸(拍照)
 * [3]:晃动
 * ...
 * [9]
 * [10:20]facex
 * [21:31]facey
 */
static volatile bool bExit = false;
static volatile uint32_t gLinux_data=0;     //linux数据
static volatile uint32_t gFace_data=0;      //人脸位置数据(只取低22位)


const char *fd_model_path = "/root/model/scrfd_480_270_int8.cvimodel";  
CVI_TDL_SUPPORTED_MODEL_E fd_model_id = CVI_TDL_SUPPORTED_MODEL_SCRFDFACE;


double cal_time_elapsed(struct timeval start, struct timeval end) {
  double sec = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.;
  return sec;
}

/**
 * 约30ms一帧
 */


struct timeval cur_t;
struct timeval last_t;

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

   

    while(bExit==false)
    {
        // last_t = cur_t;
        // gettimeofday(&cur_t, NULL);
        // double time_elapsed=0;
        //time_elapsed += cal_time_elapsed(last_t, cur_t);
        //printf("last_t=%d,cur_t=%d,time=%lf\r\n",last_t.tv_sec,cur_t.tv_sec,time_elapsed);
        
        if(gVideoCapture==0x01)
        {
            printf("camera open\r\n");
            gettimeofday(&cur_t, NULL);
            printf("last_t=%d\r\n",last_t.tv_sec);
            GOTO_IF_FAILED(CVI_VPSS_GetChnFrame(0,VPSS_CHN0, &stFrame, 2000),ret,FREE_FACE); 
            GOTO_IF_FAILED(CVI_TDL_ScrFDFace(stCVIHandle, &stFrame, &stFaceMeta),ret,FREE_FRAME); 

            if(0x02==gVideoCapture)
            {

                if(++ count > 5) count=0;   //先自增后判断
                char *filename = calloc(64, sizeof(char));
                debug_printf(filename, "/root/photo/photo%d.bmp",count);
                CVI_TDL_Change_Img(stCVIHandle,fd_model_id,&stFrame,&stNewFrame,PIXEL_FORMAT_RGB_888);
                CVI_TDL_CopyVpssImage(stNewFrame,&stImage);
                stbi_write_bmp(filename,1280,720,STBI_rgb,stImage.pix[0]);
                debug_printf("write bmp%d ok\r\n",count);
                free(filename);
                CVI_TDL_Release_VideoFrame(stCVIHandle,fd_model_id,stNewFrame,true);
                gVideoCapture=0x01;
            }

            if(stFaceMeta.size>0){
              
                last_t = cur_t;

                uint16_t face_x;    //只取低11位,不需要缩放
                uint16_t face_y;    //只取低11位
                debug_printf("face num=%d\r\n",stFaceMeta.size);
                uint8_t max=face_filter(&stFaceMeta);
                face_x=(uint16_t)(stFaceMeta.info[max].bbox.x1+stFaceMeta.info[max].bbox.x2)>>1;
                face_y=(uint16_t)(stFaceMeta.info[max].bbox.y1+stFaceMeta.info[max].bbox.y2)>>1;
                //debug_printf("face_x=%d,face_y=%d\r\n",face_x,face_y);
                debug_printf("face_x=%x,face_y=%x\r\n",face_x,face_y);

                {
                    MutexAutoLock(FACE_DATA_Mutex, lock);
                    gFace_data=(face_y <<11) | face_x;   //只取低22位 x y

                }


            }else{
                gettimeofday(&cur_t, NULL);
                //printf("cur_t=%d\r\n",cur_t.tv_sec);
                if(cur_t.tv_sec-last_t.tv_sec>10) gVideoCapture=0x00;//4s
            }
        

        FREE_FRAME:
            CVI_VPSS_ReleaseChnFrame(0,VPSS_CHN0, &stFrame); 
        FREE_FACE:
            CVI_TDL_Free(&stFaceMeta); 
            if(ret) bExit = true;
        }

        
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

    uint32_t face_data=0;       //只取低22位
    uint16_t control_data;

    fd = open(RTOS_CMDQU_DEV_NAME,  O_RDWR| O_DSYNC);
    if(fd <= 0)
    {
        debug_printf("open /dev/cvi-rtos-cmdqu failed!\n");
        return;
    }

    ret = CVI_SYS_Init();
    TRY_IF_FAILED(CVI_SYS_IonAlloc(&u64PhyAddr, &pVirAddr, "oled_buf",OLED_FONT_SIZE),ret,10);
	if (ret) {
		goto ION_ERROR;
	}
    

    /**
     * CMD_TEST_B命令发送共享地址
     */
    struct cmdqu_t cmd ={0};
    cmd.ip_id = 0;
    cmd.cmd_id =CMD_TEST_B;
    cmd.resv.mstime = 100;
    cmd.block = 1;
    // while(cmd.param_ptr != u64PhyAddr+1)
    // {
    cmd.param_ptr = u64PhyAddr;
    TRY_IF_FAILED(ioctl(fd , RTOS_CMDQU_SEND_WAIT, &cmd),ret,10);
    if(ret < 0) goto IOCTL_ERROR;

    // }

    struct Mood_Info stMood_Info={0};
    mood_system_init(&stMood_Info);
    printf("mailbox runing\r\n");

    while(bExit==false)
    {

        /**
         * mailbox发送
         */
        cmd.cmd_id =CMD_TEST_C;
        //memcpy(pVirAddr,emo_data_group[stMood_Info.stEMO_Info.select_pointer][128*8],OLED_FONT_SIZE);  

        {
            MutexAutoLock(FACE_DATA_Mutex, lock);
            face_data=gFace_data;
            
        }

        cmd.param_ptr=(face_data << 10) | (control_data&0x3FF);
        TRY_IF_FAILED(ioctl(fd , RTOS_CMDQU_SEND_WAIT, &cmd),ret,10);
        if(ret < 0) goto IOCTL_ERROR;
        


        /**
         * mailbox接收
         */
        stMood_Info.event_trigger=(uint32_t)cmd.param_ptr;
        debug_printf("cmd.param_ptr=%x\r\n",cmd.param_ptr);
        mood_update(&stMood_Info);    
        if(cmd.param_ptr==0x01)
        {
            last_t = cur_t;
            //printf("1111111111111last_t=%d\r\n",last_t.tv_sec);
            gVideoCapture=0x01;

          
        }
        // if(read_bit(stMood_Info.event_trigger,0))
        // {
        //     gVideoCapture=0x01;
        // }
        // if(read_bit(stMood_Info.event_trigger,2))
        // {
        //     gVideoCapture=0x02;
        // }

 
        printf("mailbox runing\r\n");
        usleep(200000);
     
    }

IOCTL_ERROR:
    TRY_IF_FAILED(CVI_SYS_IonFree(u64PhyAddr, pVirAddr),ret,10);
ION_ERROR:
    close(fd);
    return 0;

}



static void SampleHandleSig(int signo) {
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  if (SIGINT == signo || SIGTERM == signo) {
    bExit=true;
  }

}

int main(int argc, char *argv[])
{


    int ret=0;
    signal(SIGINT, SampleHandleSig);
	signal(SIGTERM, SampleHandleSig);

    gVideoCapture=0x01;

    pthread_t stVideoThread,stMailboxThread;
    pthread_create(&stVideoThread, NULL,Video_Capture,NULL);
   pthread_create(&stMailboxThread, NULL,Mailbox_Comm,NULL);
  
    pthread_join(stVideoThread, NULL);
    pthread_join(stMailboxThread, NULL);
  
    return 0;
	

}

