#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "thread_pool.h"


void set_bit(uint64_t *value,uint8_t position,uint8_t bit)
{
    if(bit){
        *value |= (1 << position);
    }else{
        *value &=~(1 << position);
    }
}


uint8_t count_bit(uint64_t value) 
{  
    uint8_t count = 0;  
    while (value) {  
        count += value & 1;  
        value >>= 1;  
    }  
    return count;  
} 

/**
 * @brief 创建任务队列
 * @param max_num 最大任务数量
 * 
 */
void task_queue_init(struct Task_Queue *queue)
{
    queue->head=NULL;
    queue->num=0;
    pthread_mutex_init(&queue->mutex,NULL);
    pthread_cond_init(&queue->cond,NULL);
}


void printBinary(uint16_t n) 
{
    unsigned int mask = 1 << (sizeof(uint16_t) * 8 - 1);
    while (mask) {
        putchar((n & mask) ? '1' : '0');
        mask >>= 1;
    }
    putchar('\n');
}

void *thread_pool_routine(void *arg)
{
    struct Thread_Pool *pool=(struct Thread_Pool*)arg;
    while(1){

        //printf("total,busy,thread_id=%d,%d,%d\r\n",pool->total_num,pool->busy_num,pthread_self());
        // 注册取消处理函数,&pool->lock 把锁传递过去，当前线程突然被取消时，防止死锁
		//pthread_cleanup_push(handler,(void *)&pool->queue.mutex);
        
        pthread_mutex_lock(&pool->queue.mutex);
        set_bit(&pool->thread_status,(uint8_t)pthread_self()-1,ENABLE);
        while(pool->shutdown==DISABLE && pool->queue.num==0){//如果线程池开启但是没有任务,则等待条件变量触发
            pthread_cond_wait(&pool->queue.cond,&pool->queue.mutex);
        }
        if(pool->shutdown==ENABLE&&pool->queue.num==0){//线程池关闭且没有任务
            pthread_mutex_unlock(&pool->queue.mutex);
            pthread_exit(NULL);
        }

        /* task run */
        set_bit(&pool->task_status,(uint8_t)pthread_self()-1,ENABLE);
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
        set_bit(&pool->task_status,(uint8_t)pthread_self()-1,DISABLE);
        pthread_mutex_unlock(&pool->queue.mutex);

        /* task stop */
        
        /**
         * 线程的创建在thread_pool_init和task_create中
         * 如果线程池中的总线程数>任务数(否则可能任务队列还有很多任务,但是系统过早释放了线程)
         * 且线程池中空闲线程超过max_idle_num
         * 则将线程释放
         * 
         */
        if(total_thread>=task_num+1 && idle_thread>MAX_IDLE_THREAD_NUM) break;  
        
    }
    pthread_mutex_lock(&pool->queue.mutex);
    pool->total_num--; // 更新线程池中的总线程数
    set_bit(&pool->thread_status,(uint8_t)pthread_self()-1,DISABLE);
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
    pool->task_status=0;
    pool->thread_status=0;
    task_queue_init(&pool->queue);
    for(uint8_t i=0;i<pool->total_num;i++){
        pthread_create(&pool->tid[i],NULL,thread_pool_routine,(void*)pool);//创建的线程均从thread_pool_run函数开始运行
    } 
};


void thread_pool_stop(struct Thread_Pool *pool) {
    pthread_mutex_lock(&pool->queue.mutex);
    pool->shutdown= ENABLE;
    pool->task_status=0;
    pool->thread_status=0;
    pthread_cond_broadcast(&pool->queue.cond);
    pthread_mutex_unlock(&pool->queue.mutex);
    sleep(15);
    for(uint8_t i=0;i<pool->total_num;i++){
        pthread_join(pool->tid[i], NULL);
    }
    pthread_mutex_destroy(&pool->queue.mutex);
    pthread_cond_destroy(&pool->queue.cond);
}



void exampleTask(void* arg) {
    int* num = (int*)arg;
    printf("Processing task with number: %d,thread_id=%d\n", *num,pthread_self());
    sleep(2);  // 模拟任务处理时间
    free(num);
    printf("\r\n");
}


void exampleTask1(void* arg) {
    int* num = (int*)arg;
    printf("Processing task with number: %d,thread_id=%d\n", *num,pthread_self());
    sleep(*num);  // 模拟任务处理时间
    free(num);
    printf("\r\n");
}

void get_activate_thread(void *arg)
{
    struct Thread_Pool *pool=(struct Thread_Pool*)arg;

    while(1){
        pthread_mutex_lock(&pool->queue.mutex);
        uint16_t task_status=pool->task_status;
        uint16_t thread_status=pool->thread_status;
        pthread_mutex_unlock(&pool->queue.mutex);
        printf("task status=%d\r\n",count_bit(task_status));
        printBinary(task_status);   
        printf("thread status=%d\r\n",count_bit(thread_status) );
        printBinary(thread_status);  
        printf("\r\n");
        sleep(1);
    }
    
}




int main() {
    struct Thread_Pool pool;
    thread_pool_init(&pool,5);
    task_create(&pool,get_activate_thread,(void*)&pool);
    for (int i=0;i<10;i++) {
        int* num =(int*)malloc(sizeof(int));
        *num = i;
        task_create(&pool,exampleTask,(void*)num);

    }

    sleep(5);  // 等待所有任务完成

    for (int i=0;i<10;i++) {
        int* num =(int*)malloc(sizeof(int));
        *num = i;
        task_create(&pool,exampleTask1,(void*)num);

    }


    thread_pool_stop(&pool);
    return 0;
}
