#ifndef __WP_THREAD_POOL_H
#define __WP_THREAD_POOL_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include "wp_config.h"


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
 * 
 * 
 */
struct Thread_Pool{
    struct Task_Queue queue;
    pthread_t tid[MAX_THREAD_NUM];
    uint8_t shutdown;
    uint8_t busy_num;
    uint8_t total_num;
};


void task_queue_init(struct Task_Queue *queue);
void *thread_pool_routine(void *arg);
uint8_t task_create(struct Thread_Pool *pool,void(*function)(void*),void *arg);
void thread_pool_init(struct Thread_Pool *pool,uint8_t thread_num);
void thread_pool_stop(struct Thread_Pool *pool);

#endif
