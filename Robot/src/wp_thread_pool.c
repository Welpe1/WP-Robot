#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "wp_utils.h"
#include "wp_thread_pool.h"


/**
 * @brief 创建任务队列
 * 
 */
void task_queue_init(struct Task_Queue *queue)
{
    queue->head=NULL;
    queue->num=0;
    pthread_mutex_init(&queue->mutex,NULL);
    pthread_cond_init(&queue->cond,NULL);
}

/**
 * @brief 线程池工作函数
 * 
 * 如果线程池中的总线程数>任务数(否则可能任务队列还有很多任务,但是系统过早释放了线程)
 * 且线程池中空闲线程超过max_idle_num
 * 则将线程释放
 * 
 * 
 */
void *thread_pool_routine(void *arg)
{
    struct Thread_Pool *pool=(struct Thread_Pool*)arg;
    while(1){

        pthread_mutex_lock(&pool->queue.mutex);
        while(pool->shutdown==DISABLE && pool->queue.num==0){//如果线程池开启但是没有任务,则等待条件变量触发
            pthread_cond_wait(&pool->queue.cond,&pool->queue.mutex);
        }
        if(pool->shutdown==ENABLE&&pool->queue.num==0){//线程池关闭且没有任务
            pthread_mutex_unlock(&pool->queue.mutex);
            pthread_exit(NULL);
        }

        /* task run */
        struct Task* task=pool->queue.head;
        pool->queue.head=task->next;;//取出任务
        pool->queue.num--;
        pool->busy_num++;
        pthread_mutex_unlock(&pool->queue.mutex);

        task->function(task->arg);//执行任务
        free(task);

        pthread_mutex_lock(&pool->queue.mutex);
        pool->busy_num--;
        uint8_t idle_thread=pool->total_num-pool->busy_num;
        uint8_t total_thread=pool->total_num;
        uint8_t task_num=pool->queue.num;
        pthread_mutex_unlock(&pool->queue.mutex);
        /* task stop */

        if(total_thread>=task_num+1 && idle_thread>MAX_IDLE_THREAD_NUM) break;  
        
    }
    pthread_mutex_lock(&pool->queue.mutex);
    pool->total_num--; // 更新线程池中的总线程数
    pthread_mutex_unlock(&pool->queue.mutex);
    pthread_exit(NULL);
    return (void *)NULL;
}

/**
 * @brief 创建任务
 * 1.上锁,防止在操作pool时其它线程也操作pool
 * 2.检查任务队列是否已满,已满则返回错误
 * 3.为任务分配内存
 * 4.将任务插入任务队列(尾插)
 * 5.任务总数++
 * 6.如果此时没有空闲线程则创建线程(不超过最大线程数)
 * 7.解锁
 * 
 */
uint8_t task_create(struct Thread_Pool *pool,void(*function)(void*),void *arg)
{
    pthread_mutex_lock(&pool->queue.mutex);
    if(pool->queue.num==MAX_TASK_NUM){
        pthread_mutex_unlock(&pool->queue.mutex);
        printf("task queue full,create task fail\r\n");
        return 1;
    }
    struct Task* task=(struct Task*)malloc(sizeof(struct Task));
    if(task==NULL){
        pthread_mutex_unlock(&pool->queue.mutex);
        printf("create task fail\r\n");
        return 1;
    }
    task->function=function;
    task->arg=arg;
    task->next=NULL;

    if(pool->queue.head==NULL){
        pool->queue.head=task;
    }else{
        struct Task* tmp=pool->queue.head; 
        while(tmp->next!=NULL){
            tmp=tmp->next;
        }
        tmp->next=task;
    }

    pool->queue.num++;

    uint8_t idle_thread=pool->total_num-pool->busy_num;
    if((idle_thread==0) && (pool->total_num<MAX_THREAD_NUM)){
        if(pthread_create(&pool->tid[pool->total_num],NULL,thread_pool_routine,(void*)pool)!=0){
            printf("fail to create thread\r\n");
            pthread_mutex_unlock(&pool->queue.mutex);
            pthread_cond_signal(&pool->queue.cond);
            return 1;
        }
        printf("no idle thread,create new\r\n");
        pool->total_num++;
    }

    pthread_mutex_unlock(&pool->queue.mutex);
    pthread_cond_signal(&pool->queue.cond);
    usleep(1000);
    return 0;
}


/**
 * @brief 线程池初始化
 * 1.初始化任务队列
 * 2.初始化线程池中的每一个线程,每一个线程均从thread_pool_run函数开始运行
 */
void thread_pool_init(struct Thread_Pool *pool,uint8_t thread_num)
{
    pool->total_num=thread_num;
    pool->busy_num=0;
    pool->shutdown=DISABLE;
    task_queue_init(&pool->queue);
    for(uint8_t i=0;i<pool->total_num;i++){
        pthread_create(&pool->tid[i],NULL,thread_pool_routine,(void*)pool);//创建的线程均从thread_pool_run函数开始运行
    } 
};


void thread_pool_stop(struct Thread_Pool *pool) {
    pthread_mutex_lock(&pool->queue.mutex);
    pool->shutdown= ENABLE;
    pthread_cond_broadcast(&pool->queue.cond);
    pthread_mutex_unlock(&pool->queue.mutex);
    for(uint8_t i=0;i<pool->total_num;i++){
        pthread_join(pool->tid[i], NULL);
    }
    pthread_mutex_destroy(&pool->queue.mutex);
    pthread_cond_destroy(&pool->queue.cond);
}

