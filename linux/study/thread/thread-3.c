/**
 * 互斥锁适合写操作较多而读操作较少的情况
 * 互斥锁创建和销毁 pthread_mutex_init  pthread_mutex_destroy
 * 上锁 pthread_mutex_lock（拿不到锁阻塞）  pthread_mutex_trylock（拿不到锁报错）
 * 解锁 pthread_mutex_unlock
 * 互斥锁数据类型 pthread_mutex_t
 * 
 * 互斥锁属性的创建和销毁 pthread_mutexattr_init  pthread_mutexattr_destroy
 * 
 * 尽量不设置全局变量
 * 
 */

#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

/**
 * 现要实现全局变量g_count的值在主线程，子线程1，子线程2中
 * 分别为0,50,150并打印出来,由于子线程1中存在1000us延时，故
 * 如果不加互斥锁则打印信息错误
 */

int g_count=0;      //全局变量，三个线程均可访问
pthread_mutex_t mutex;    //定义互斥锁

void* task1_function(void *arg)
{
    /**
     * 从上锁开始到解锁这段区域称为临界区
     */
    pthread_mutex_lock(&mutex);     //上锁
    g_count=50;
    usleep(1000);
    printf("task1->g_count=%d\n",g_count);
    pthread_mutex_unlock(&mutex);       //解锁
    
    return (void*)0;

}

void* task2_function(void *arg)
{
    pthread_mutex_lock(&mutex);
    g_count=150;
    printf("task2->g_count=%d\n",g_count);
    pthread_mutex_unlock(&mutex);
    return (void*)0;

}

int main(int argc,char **argv)
{
    int err;
    pthread_t task1,task2;      //创建线程ID
    printf("main->g_count=%d\n",g_count);

    pthread_mutex_init(&mutex,NULL);//互斥锁初始化
    if((err=pthread_create(&task1,NULL,task1_function,(void*)20))!=0){
        perror("pthread create err!");
    };
    if((err=pthread_create(&task2,NULL,task2_function,(void*)20))!=0){
        perror("pthread create err!");
    };

    pthread_join(task1,NULL);
    pthread_join(task2,NULL);
    
    pthread_mutex_destroy(&mutex);      //销毁锁

    printf("finish\r\n");
    return 0;

}







