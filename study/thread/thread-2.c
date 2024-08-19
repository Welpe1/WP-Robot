/**
 * 线程属性(即pthread_create的第二个传参)初始化和销毁 pthread_attr_init  pthread_attr_destroy
 * 线程属性中的分离属性决定线程结束后能否自动释放资源
 * 默认情况下是以非分离状态启动线程的，线程结束后不会释放系统资源，必须调用pthread_join
 * 若以分离状态启动，则线程结束后自动释放系统资源，此时再调用pthread_join会报错
 * 分离属性在网络通信中使用较多
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct{
    int d1;
    int d2;
}task_arg;

//定义线程运行函数
void* task1_function(void *arg1)
{
    task_arg *arg=(task_arg*)arg1;
    return (void*)(arg->d1+arg->d2);

}

void* task2_function(void *arg2)
{
    task_arg *arg=(task_arg*)arg2;
    return (void*)(arg->d1+arg->d2);

}



int main(int argc,int **argv)
{
    int err;
    pthread_t task1,task2;  
    task_arg task1_arg={10,50},task2_arg={100,500};

    pthread_attr_t attr;        //定义线程属性
    pthread_attr_init(&attr);       //对线程属性初始化

    //task1以正常方式启动
    if((err=pthread_create(&task1,&attr,task1_function,(void*)&task1_arg))!=0){      //创建task1线程(线程ID，线程属性指针，线程函数的地址，线程函数参数的地址)
        perror("pthread create err!");
    }
    int *result=0;
    pthread_join(task1,(void**)&result);
    printf("result is %d\n",result); 
    printf("-----------------------------------------------------\n");     

    //task2以分离状态启动线程
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);     //设置分离属性为分离状态启动
    if((err=pthread_create(&task2,&attr,task2_function,(void*)&task2_arg))!=0){      
        perror("pthread create err!");
    }

    //销毁线程属性
    pthread_attr_destroy(&attr);
    printf("finish\r\n");

    return 0;

}

