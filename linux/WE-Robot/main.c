#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "middleware.h"

#include "cvi_tdl.h"
#include "cvi_tdl_app.h"
#include "middleware_utils.h"
#include "sample_utils.h"
#include "vi_vo_utils.h"

#include <core/utils/vpss_helper.h>
#include <cvi_comm.h>
#include <rtsp.h>
#include <sample_comm.h>

#include <inttypes.h>
#include "sample_comm.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define OUTPUT_BUFFER_SIZE 10

typedef struct {
  uint64_t u_id;
  float quality;
  cvtdl_image_t image;
  tracker_state_e state;
  uint32_t counter;
} IOData;

typedef struct {
  CVI_S32 voType;
  SAMPLE_TDL_MW_CONTEXT *pstMWContext;
  cvitdl_service_handle_t service_handle;
} SAMPLE_TDL_VENC_THREAD_ARG_S;

MUTEXAUTOLOCK_INIT(IOMutex);
MUTEXAUTOLOCK_INIT(VOMutex);

/* global variables */
static volatile bool bExit = false;
static volatile bool bRunImageWriter = true;
static volatile bool bRunVideoOutput = true;

int rear_idx = 0;
int front_idx = 0;
static IOData data_buffer[OUTPUT_BUFFER_SIZE];
static cvtdl_face_t g_face_meta_0;

double cal_time_elapsed(struct timeval start, struct timeval end) {
  double sec = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.;
  return sec;
}

float g_draw_clrs[] = {0,   0,   0,   255, 0,   0, 0,   255, 0,   0,  0,
                       255, 255, 255, 0,   255, 0, 255, 0,   255, 255};

static void SampleHandleSig(CVI_S32 signo) {
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);

  if (SIGINT == signo || SIGTERM == signo) {
    bExit = true;
  }
}

/* Consumer */
static void *pImageWrite(void *args) {
  printf("[APP] Image Write Up\n");
  while (bRunImageWriter) {
    /* only consumer write front_idx */
    bool empty;
    {
      MutexAutoLock(IOMutex, lock);
      empty = front_idx == rear_idx;
    }
    if (empty) {
      printf("I/O Buffer is empty.\n");
      usleep(100 * 1000);
      continue;
    }
    int target_idx = (front_idx + 1) % OUTPUT_BUFFER_SIZE;
    char *filename = calloc(64, sizeof(char));
    sprintf(filename, "/mnt/data/admin1_data/alios_test/phobos/face_%" PRIu64 "_%u.png",
            data_buffer[target_idx].u_id, data_buffer[target_idx].counter);
    if (data_buffer[target_idx].image.pix_format != PIXEL_FORMAT_RGB_888) {
      printf("[WARNING] Image I/O unsupported format: %d\n",
             data_buffer[target_idx].image.pix_format);
    } else {
      if (data_buffer[target_idx].image.width == 0) {
        printf("[WARNING] Target image is empty.\n");
      } else {
        printf(" > (I/O) Write Face (Q: %.2f): %s ...\n", data_buffer[target_idx].quality,
               filename);
        stbi_write_png(filename, data_buffer[target_idx].image.width,
                       data_buffer[target_idx].image.height, STBI_rgb,
                       data_buffer[target_idx].image.pix[0],
                       data_buffer[target_idx].image.stride[0]);
      }
    }

    free(filename);
    CVI_TDL_Free(&data_buffer[target_idx].image);
    {
      MutexAutoLock(IOMutex, lock);
      front_idx = target_idx;
    }
  }

  printf("[APP] free buffer data...\n");
  while (front_idx != rear_idx) {
    CVI_TDL_Free(&data_buffer[(front_idx + 1) % OUTPUT_BUFFER_SIZE].image);
    {
      MutexAutoLock(IOMutex, lock);
      front_idx = (front_idx + 1) % OUTPUT_BUFFER_SIZE;
    }
  }

  return NULL;
}

static void *run_venc(void *args) {
  printf("[APP] Video Output Up\n");
  SAMPLE_TDL_VENC_THREAD_ARG_S *vo_args = (SAMPLE_TDL_VENC_THREAD_ARG_S *)args;
  if (!vo_args->voType) {
    return NULL;
  }
  cvitdl_service_handle_t service_handle = vo_args->service_handle;
  CVI_S32 s32Ret = CVI_SUCCESS;

  cvtdl_face_t face_meta_0;

  VIDEO_FRAME_INFO_S stVOFrame;
  while (bRunVideoOutput) {
    s32Ret = CVI_VPSS_GetChnFrame(0, 0, &stVOFrame, 1000);
    if (s32Ret != CVI_SUCCESS) {
      printf("CVI_VPSS_GetChnFrame chn0 failed with %#x\n", s32Ret);
      break;
    }

    {
      MutexAutoLock(VOMutex, lock);
      CVI_TDL_CopyFaceMeta(&g_face_meta_0, &face_meta_0);
    }

    size_t image_size = stVOFrame.stVFrame.u32Length[0] + stVOFrame.stVFrame.u32Length[1] +
                        stVOFrame.stVFrame.u32Length[2];
    stVOFrame.stVFrame.pu8VirAddr[0] =
        (uint8_t *)CVI_SYS_Mmap(stVOFrame.stVFrame.u64PhyAddr[0], image_size);
    stVOFrame.stVFrame.pu8VirAddr[1] =
        stVOFrame.stVFrame.pu8VirAddr[0] + stVOFrame.stVFrame.u32Length[0];
    stVOFrame.stVFrame.pu8VirAddr[2] =
        stVOFrame.stVFrame.pu8VirAddr[1] + stVOFrame.stVFrame.u32Length[1];

    for (int i = 0; i < face_meta_0.size; i++) {
      int lb = 0;
      if (strlen(face_meta_0.info[i].name) != 0) {
        lb = atoi(face_meta_0.info[i].name);
      }
      printf("trackid:%d,lb:%d,name:%s\n", (int)face_meta_0.info[i].unique_id, lb,
             face_meta_0.info[i].name);
      cvtdl_service_brush_t brushi;
      int num_clr = 7;
      int ind = lb % num_clr;

      brushi.color.r = g_draw_clrs[3 * ind];
      brushi.color.g = g_draw_clrs[3 * ind + 1];
      brushi.color.b = g_draw_clrs[3 * ind + 2];
      brushi.size = 4;
      cvtdl_face_t face_metai = face_meta_0;
      face_metai.size = 1;
      face_metai.info = &face_meta_0.info[i];
      CVI_TDL_Service_FaceDrawRect(service_handle, &face_metai, &stVOFrame, false, brushi);
    }
    //     CVI_TDL_Service_FaceDrawRect(service_handle, &face_meta_0, &stVOFrame, false, brush_0);
    // #ifdef VISUAL_FACE_LANDMARK
    //     CVI_TDL_Service_FaceDraw5Landmark(&face_meta_0, &stVOFrame);
    // #endif

    CVI_SYS_Munmap((void *)stVOFrame.stVFrame.pu8VirAddr[0], image_size);
    stVOFrame.stVFrame.pu8VirAddr[0] = NULL;
    stVOFrame.stVFrame.pu8VirAddr[1] = NULL;
    stVOFrame.stVFrame.pu8VirAddr[2] = NULL;
    s32Ret = SAMPLE_TDL_Send_Frame_RTSP(&stVOFrame, vo_args->pstMWContext);
    // s32Ret = SendOutputFrame(&stVOFrame, &vo_args->vs_ctx.outputContext);
    if (s32Ret != CVI_SUCCESS) {
      printf("Send Output Frame NG\n");
    }

    s32Ret = CVI_VPSS_ReleaseChnFrame(0, 0, &stVOFrame);
    if (s32Ret != CVI_SUCCESS) {
      printf("CVI_VPSS_ReleaseChnFrame chn0 NG\n");
      break;
    }

    CVI_TDL_Free(&face_meta_0);
  }
  return NULL;
}


int main(int argc, char *argv[]) {
  if (argc !=3) {
    printf("Usage: sample_vi_face_recog scrfd_model_file fr_model_file \n");
    return CVI_TDL_FAILURE;
  }
  CVI_S32 ret = CVI_TDL_SUCCESS;
  // Set signal catch
  signal(SIGINT, SampleHandleSig);
  signal(SIGTERM, SampleHandleSig);
  const char *fd_model_path = argv
      [1];  //"/mnt/data/admin1_data/AI_CV/cv182x/ai_models/phobos_fr/scrfd_500m_bnkps_432_768.cvimodel";//argv[1];
  const char *fr_model_path = argv
      [2];  //"/mnt/data/admin1_data/AI_CV/cv182x/ai_models/phobos_fr/cviface-v5-s.cvimodel";//argv[2];
  
  // const char *fq_model_path = argv[5];
  // const char *config_path = argv[6];
  // const char *mode_id = argv[7];
  int buffer_size = 10;       // atoi(argv[8]);
  float det_threshold = 0.5;  // atof(argv[9]);
  float recog_thresh = 0.4;
  bool write_image = false;  // atoi(argv[10]) == 1;
  int voType = 2;            // atoi(argv[11]);
  // int vi_format = 1;//atoi(argv[12]);

  CVI_TDL_SUPPORTED_MODEL_E fd_model_id = CVI_TDL_SUPPORTED_MODEL_SCRFDFACE;
  CVI_TDL_SUPPORTED_MODEL_E fr_model_id = CVI_TDL_SUPPORTED_MODEL_FACERECOGNITION;

  if (buffer_size <= 0) {
    printf("buffer size must be larger than 0.\n");
    return CVI_FAILURE;
  }

  SAMPLE_TDL_MW_CONTEXT stMWContext = {0};
  ret = MiddleWare_Init(&stMWContext);
  if (ret != CVI_TDL_SUCCESS) {
    printf("failed with %#x!\n", ret);
    goto CLEANUP_SYSTEM;
  }
  cvitdl_handle_t tdl_handle = NULL;
  cvitdl_service_handle_t service_handle = NULL;
  cvitdl_app_handle_t app_handle = NULL;

  ret = CVI_TDL_CreateHandle2(&tdl_handle, 1, 0);
  ret |= CVI_TDL_Service_CreateHandle(&service_handle, tdl_handle);
  ret |= CVI_TDL_APP_CreateHandle(&app_handle, tdl_handle);
  ret |= CVI_TDL_APP_FaceCapture_Init(app_handle, (uint32_t)buffer_size);
  ret |= CVI_TDL_APP_FaceCapture_QuickSetUp(app_handle, fd_model_id, fr_model_id, fd_model_path,
                                            fr_model_path, NULL, NULL);
  app_handle->face_cpt_info->fr_flag = 2;
  if (ret != CVI_TDL_SUCCESS) {
    printf("failed with %#x!\n", ret);
    goto CLEANUP_SYSTEM;
  }
  CVI_TDL_SetVpssTimeout(tdl_handle, 1000);
  CVI_TDL_SetModelThreshold(tdl_handle, fd_model_id, det_threshold);
  CVI_TDL_APP_FaceCapture_SetMode(app_handle, CYCLE);

  face_capture_config_t app_cfg;
  CVI_TDL_APP_FaceCapture_GetDefaultConfig(&app_cfg);
  CVI_TDL_APP_FaceCapture_SetConfig(app_handle, &app_cfg);

 

  VIDEO_FRAME_INFO_S stfdFrame;
  memset(&g_face_meta_0, 0, sizeof(cvtdl_face_t));
  pthread_t io_thread;
  pthread_t vo_thread;
  pthread_create(&io_thread, NULL, pImageWrite, NULL);
  SAMPLE_TDL_VENC_THREAD_ARG_S vo_args = {
      .voType = voType, .service_handle = service_handle, .pstMWContext = &stMWContext};

  // vo_args.vs_ctx = vs_ctx;
  pthread_create(&vo_thread, NULL, run_venc, (void *)&vo_args);
  int fail_counter = 0;
  size_t counter = 0;

  struct timeval last_t;
  gettimeofday(&last_t, NULL);

  double time_elapsed = 0;
  while (bExit == false) {
    printf("\nGet Frame %zu\n", counter);

    ret = CVI_VPSS_GetChnFrame(0, VPSS_CHN1, &stfdFrame, 2000);
    if (ret != CVI_SUCCESS) {
      fail_counter++;
      if (fail_counter < 10) {
        usleep(10);
        continue;
      }
      printf("CVI_VPSS_GetChnFrame chn0 failed with %#x\n", ret);
      break;
    }

    fail_counter = 0;
    counter += 1;
    struct timeval cur_t;
    gettimeofday(&cur_t, NULL);
    time_elapsed += cal_time_elapsed(last_t, cur_t);
    last_t = cur_t;

    ret = CVI_TDL_APP_FaceCapture_Run(app_handle, &stfdFrame);
    if (ret != CVI_TDL_SUCCESS) {
      printf("CVI_TDL_APP_FaceCapture_Run failed with %#x\n", ret);
      break;
    }

    for (uint32_t i = 0; i < app_handle->face_cpt_info->size; i++) {
      if (!app_handle->face_cpt_info->_output[i]) continue;
      do_face_match(service_handle, &app_handle->face_cpt_info->data[i].info, recog_thresh);
    }

    {
      MutexAutoLock(VOMutex, lock);
      CVI_TDL_CopyFaceMeta(&app_handle->face_cpt_info->last_faces, &g_face_meta_0);
    }

    /* Producer */
    if (write_image) {
      for (uint32_t i = 0; i < app_handle->face_cpt_info->size; i++) {
        if (!app_handle->face_cpt_info->_output[i]) continue;

        tracker_state_e state = app_handle->face_cpt_info->data[i].state;
        uint32_t counter = app_handle->face_cpt_info->data[i]._out_counter;
        uint64_t u_id = app_handle->face_cpt_info->data[i].info.unique_id;
        float face_quality = app_handle->face_cpt_info->data[i].info.face_quality;
        if (state == MISS) {
          printf("Produce Face-%" PRIu64 "_out\n", u_id);
        } else {
          printf("Produce Face-%" PRIu64 "_%u\n", u_id, counter);
        }
        /* Check output buffer space */
        bool full;
        int target_idx;
        {
          MutexAutoLock(IOMutex, lock);
          target_idx = (rear_idx + 1) % OUTPUT_BUFFER_SIZE;
          full = target_idx == front_idx;
        }
        if (full) {
          printf("[WARNING] Buffer is full! Drop out!");
          continue;
        }
        /* Copy image data to buffer */
        data_buffer[target_idx].u_id = u_id;
        data_buffer[target_idx].quality = face_quality;
        data_buffer[target_idx].state = state;
        data_buffer[target_idx].counter = counter;
        /* NOTE: Make sure the image type is IVE_IMAGE_TYPE_U8C3_PACKAGE */
        CVI_TDL_CopyImage(&app_handle->face_cpt_info->data[i].image,
                          &data_buffer[target_idx].image);
        {
          MutexAutoLock(IOMutex, lock);
          rear_idx = target_idx;
        }
      }
    }

    printf("average_fps:%.3f\n", counter / time_elapsed);

    ret = CVI_VPSS_ReleaseChnFrame(0, VPSS_CHN1, &stfdFrame);
    if (ret != CVI_SUCCESS) {
      printf("CVI_VPSS_ReleaseChnFrame chn0 NG\n");
      break;
    }
  }
  bRunImageWriter = false;
  bRunVideoOutput = false;
  pthread_join(io_thread, NULL);
  pthread_join(vo_thread, NULL);

CLEANUP_SYSTEM:
  CVI_TDL_APP_DestroyHandle(app_handle);
  CVI_TDL_Service_DestroyHandle(service_handle);
  CVI_TDL_DestroyHandle(tdl_handle);
  SAMPLE_TDL_Destroy_MW(&stMWContext);
  CVI_SYS_Exit();
  CVI_VB_Exit();
}
