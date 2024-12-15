#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "thread_pool.h"


/**
 * 线程最多允许5个空闲线程同时存在，多出的自动释放
 * 添加任务后，若任务队列已满，则阻塞等待
 *             若无空闲线程，则创建新的线程，但不能超过最大线程数
 * 
 * 
 * 
 * 
 */



/**
 * @brief 任务队列初始化
 * @param max_task 最大任务数量
 * 默认参数正确
 */
void task_queue_init(struct Task_Queue *queue,uint8_t max_task)
{
    queue->head=queue->tail=NULL;
    queue->num=0;
    queue->max_num=max_task;
    pthread_mutex_init(&queue->mutex,NULL);
    pthread_cond_init(&queue->cond,NULL);
}

/**
 * @brief 创建任务
 * 1.为任务分配内存
 * 2.上锁,防止在添加任务的时候,有别的线程去拿任务
 * 3.将任务插入任务队列(尾插)
 * 4.任务总数++
 * 5.解锁
 * 
 */
uint8_t task_create(struct Thread_Pool *pool,void(*function)(void*),void *arg)
{
    struct Task* task=(struct Task*)malloc(sizeof(struct Task));
    if(task==NULL){
        printf("alloc mem fail\r\n");
        return 1;
    }
    task->function=function;
    task->arg=arg;
    task->next=NULL;

    pthread_mutex_lock(&pool->queue.mutex);

    if(pool->queue.num==pool->queue.max_num){
        pthread_mutex_unlock(&pool->queue.mutex);
        printf("task queue full\r\n");
        free(task);
        return 1;
    }

    if(pool->queue.tail==NULL){
        pool->queue.head=pool->queue.tail=task;
    }else{
        pool->queue.tail->next=task;
        pool->queue.tail=task;
    }
    pool->queue.num++;
    pthread_mutex_unlock(&pool->queue.mutex);
    pthread_cond_signal(&pool->queue.cond);
    
    return 0;
}


/**
 * @brief 线程池工作线程函数
 */
void *thread_pool_run(void *arg)
{
    struct Thread_Pool *pool=(struct Thread_Pool*)arg;
    while(1){

        // // 注册取消处理函数,&pool->lock 把锁传递过去，当前线程突然被取消时，防止死锁
		// pthread_cleanup_push(handler, (void *)&pool->queue.mutex);

        pthread_mutex_lock(&pool->queue.mutex);
        while(pool->status==ENABLE && pool->queue.num==0){//如果线程池开启但是没有任务,则等待条件变量触发
            pthread_cond_wait(&pool->queue.cond,&pool->queue.mutex);
        }
        if(pool->status==DISABLE&&pool->queue.num==0){//线程池关闭且没有任务
            pthread_mutex_unlock(&pool->queue.mutex);
            break;
            //pthread_exit(NULL); // CANNOT use 'break'; 
        }
        struct Task* task=NULL;
        if(pool->queue.num>0){
            task=pool->queue.head;//取出任务
            pool->queue.head=task->next;
            if(pool->queue.head==NULL){
                pool->queue.tail=NULL;
            }
            pool->queue.num--;
        }
        pthread_mutex_unlock(&pool->queue.mutex);
        if(task!=NULL){
            task->function(task->arg);//执行任务
            free(task);
        }
    }
    pthread_exit(NULL);
}


/**
 * @brief 向线程池中添加n个线程
 * 
 */
uint8_t thread_create(struct Thread_Pool *pool,uint8_t num)
{
    uint8_t add_num=0;
    for(uint8_t i=0;i<num;i++){
        if(pthread_create(&pool->tid[pool->total_num],NULL,thread_pool_run,(void*)pool)!=0){
            printf("create thread fail\r\n");
            break;
        }else{
            add_num++;
            pool->total_num++;
        }
    }
    return add_num;
}




/**
 * @brief 从线程池中删除一个线程
 * 
 */
uint8_t thread_destroy(struct Thread_Pool *pool)
{
    if(pool->total_num<=1){
        return 1;
    }
    if(pthread_cancel(pool->tid[pool->total_num])!=0){
        printf("destroy thread fail\r\n");
        return 1;
    };
    pool->total_num--;
    return 0;

}

/**
 * @brief 从线程池中删除多个线程
 * 
 */
uint8_t threads_destroy(struct Thread_Pool *pool,uint8_t num)
{
    if(num==0)
        return 1;
    uint16_t del_num=0;
    for(uint8_t i=0;i<num;i++){
        if(thread_destroy(pool)==0){
            del_num++;
        }
    }
    return del_num;
}



/**
 * @brief 线程池初始化
 * 1.初始化任务队列
 * 2.初始化线程池中的每一个线程,每一个线程均从thread_pool_run函数开始运行
 */
void thread_pool_init(struct Thread_Pool *pool)
{

    pool->total_num=5;
    pool->status=ENABLE;
    task_queue_init(&pool->queue,MAX_TASK_NUM);
    for(uint8_t i=0;i<MAX_THREAD_NUM;i++){
        pthread_create(&pool->tid[i],NULL,thread_pool_run,(void*)pool);//创建的线程均从thread_pool_run函数开始运行
    } 
};


void thread_pool_stop(struct Thread_Pool *pool) {
    pthread_mutex_lock(&pool->queue.mutex);
    pool->status= DISABLE;
    pthread_cond_broadcast(&pool->queue.cond);
    pthread_mutex_unlock(&pool->queue.mutex);
    for(uint8_t i=0;i<MAX_THREAD_NUM;i++){
        pthread_join(pool->tid[i], NULL);
    }
    pthread_mutex_destroy(&pool->queue.mutex);
    pthread_cond_destroy(&pool->queue.cond);
}


void exampleTask(void* arg) {
    int* num = (int*)arg;
    printf("Processing task with number: %d,thread_id=%d\n", *num,pthread_self());
    sleep(1);  // 模拟任务处理时间
    free(num);
    printf("\r\n");
}

int main() {
    struct Thread_Pool pool;
    thread_pool_init(&pool);

    for (int i = 0; i < 305; i++) {
        int* num = (int*)malloc(sizeof(int));
        *num = i;
        task_create(&pool, exampleTask, (void*)num);
    }
 
    sleep(5);  // 等待所有任务完成
    thread_pool_stop(&pool);
 
    return 0;
}