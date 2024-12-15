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

#define MAX_TASK_NUM    10  //最大任务队列的大小(即最多任务数量)
#define MAX_THREAD_NUM   4   //线程池中的最大线程数量(即最多线程数)

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
 * @param head 队列头
 * @param tail 队列尾
 * @param num 任务的数量
 * @param max_num 最大任务数量
 * @param mutex 互斥锁
 * @param cond 条件变量
 * 
 */
struct Task_Queue{
    struct Task *head;  
    struct Task *tail;
    uint8_t num;
    uint8_t max_num;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

/**
 * @brief 线程池
 * @param queue 任务队列
 * @param tid 队列pid
 * @param status 线程池状态
 * @param total_num 线程数量
 * 
 */
struct Thread_Pool{
    struct Task_Queue queue;
    pthread_t tid[MAX_THREAD_NUM];
    uint8_t status;
    uint8_t busy_num;
    uint8_t idle_num;
    uint8_t total_num;
};





#endif
