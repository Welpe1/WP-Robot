/**
 * 线程同步 互斥锁+条件变量
 * 条件变量创建和销毁 pthread_cond_init pthread_cond_destroy
 * 等待操作 pthread_cond_wait pthread_cond_timewait
 * 通知操作 pthread_cond_singal（通知单个线程） pthread_cond_broadcast（通知所有线程）
 * 条件变量数据类型 pthread_cond_t
 * 
 * 
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * 一个线程(set)计算结果，一个线程获(get)取结果并打印
 * get线程必须等到set线程计算完毕才能获取结果
 */

typedef struct 
{
    int res;
    int is_wait;        //用户给出的用于判断的条件
    pthread_cond_t cond;
    pthread_mutex_t mutex;
}Result;

//计算并将结果放入Result中的线程函数
void* set_fn(void *arg)
{
    Result *r=(Result*)arg;
    int i=0,sum=0;
    for(i=0;i<100;i++){
        sum+=i;
    }
    r->res=sum;


    //对两个线程共享的判断条件加锁保护
    pthread_mutex_lock(&r->mutex);
    //判断get线程是否准备好
    while(!r->is_wait){
        pthread_mutex_unlock(&r->mutex);
        usleep(100);
        pthread_mutex_lock(&r->mutex);
    }
    pthread_mutex_unlock(&r->mutex);

    pthread_cond_signal(&r->cond);      //通知等待的get线程




    return (void*)0;
}


//获取结果的线程函数
void* get_fn(void *arg)
{
    Result *r=(Result*)arg;

    //对两个线程共享的判断条件加锁保护
    pthread_mutex_lock(&r->mutex);
    r->is_wait=1;   //代表get线程已经准备好
    pthread_cond_wait(&r->cond,&r->mutex);      //get线程等待
    pthread_mutex_unlock(&r->mutex);        //get线程被唤醒后，此unlock是针对pthread_cond_wait内部的lock而言的
    int res=r->res;
    printf("get sum is %d\n",res);

    return (void*)0;
}

int main()
{
    int err;
    pthread_t set,get;
    Result R;       //两个线程共用的资源
    R.is_wait=0;

    pthread_cond_init(&R.cond,NULL);
    pthread_mutex_init(&R.mutex,NULL);

    if((err=pthread_create(&set,NULL,set_fn,(void*)&R))!=0){
        perror("pthread create err\n");
    }
    if((err=pthread_create(&get,NULL,get_fn,(void*)&R))!=0){
        perror("pthread create err\n");
    }



    pthread_join(set,NULL);
    pthread_join(get,NULL);
    pthread_cond_destroy(&R.cond);
    pthread_mutex_destroy(&R.mutex);

    return 0;

}






















