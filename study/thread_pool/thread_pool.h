#ifndef __THREAD_POOL_H
#define __THREAD_POOL_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#define DISABLE     0
#define ENABLE      1

#define MAX_TASK_NUM     20      //最大任务队列的大小(即最多任务数量)
#define MAX_THREAD_NUM   8      //线程池中的最大线程数量
#define MAX_IDLE_THREAD_NUM   2     //最大空闲线程数量

/**
 * @brief 任务节点 
 * @param function
 * @param arg
 * @param next
 * 
 */
struct Task{
    void (*function)(void *arg);
    void *arg;
    struct Task *next;
};

/**
 * @brief 任务队列
 * @param head 队列头(第一个任务)
 * @param num 任务的数量
 * @param mutex 互斥锁
 * @param cond 条件变量
 * 
 */
struct Task_Queue{ 
    struct Task *head;
    uint8_t num;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

/**
 * @brief 线程池
 * @param queue 任务队列
 * @param tid 队列pid
 * @param shutdown 线程池状态
 * @param busy_num 忙线程数
 * @param total_num      线程总数
 * @param thread_status 线程运行状态
 * 
 * 
 */
struct Thread_Pool{
    struct Task_Queue queue;
    pthread_t tid[MAX_THREAD_NUM];
    uint8_t shutdown;
    uint8_t busy_num;
    uint8_t total_num;
    uint64_t task_status;
    uint64_t thread_status;
};



#endif
