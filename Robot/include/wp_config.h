#ifndef __WP_CONFIG_H
#define __WP_CONFIG_H


/********************************** wp model ***************************************/
#define IMAGE_HEIGHT    1080
#define IMAGE_WIDTH     1920


/********************************** wp mood system ***************************************/
#define MAX_MOOD_NUM            20      //最大情绪数量
#define MAX_NAME_LEN            10      //情绪名最大长度
#define MAX_MOOD_VALUE          240
#define BASE_MOOD_VALUE         100
#define MIN_MOOD_VALUE          10
#define VALUE_INCREASE          3
#define MAX_MOOD_GROUP          8
#define MAX_FRAME_NUM           10

/*status*/
#define RUN_BIT                 0
#define TRIGGER_BIT             1
#define READY                   1
#define PEND                    0
#define ENABLE            1
#define DISABLE           0

/*frame*/
#define FRAME_ROOT  "/root/frame/"



#define DISABLE     0
#define ENABLE      1

#define MAX_TASK_NUM     20      //最大任务队列的大小(即最多任务数量)
#define MAX_THREAD_NUM   8      //线程池中的最大线程数量
#define MAX_IDLE_THREAD_NUM   2     //最大空闲线程数量


#endif
