/**
 * 线程标识符数据类型 pthread_t
 * 线程创建 pthread_create
 * 线程终止 return  pthread_exit（主动终止）  pthread_cancel（终止其他线程）
 * 资源释放 pthread_join
 * 线程清理 pthread_cleanup_push  pthread_cleanup_pop（成对出现）
 * 
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int start;
    int end;
    char name[20];
}task_arg;

//定义线程运行函数
void* task1_function(void *arg1)
{
    task_arg *arg=(task_arg*)arg1;
    int i=0;
    for(i=arg->start;i<=arg->end;i++){
        printf("%s run %d\n",arg->name,i);
        usleep(200);
    }

    return (void*)0;            //线程主动终止 
    // pthread_exit((void*)0);         //线程主动终止 
    // exit(0);            //进程（注意是进程）主动终止 
}

void* task2_function(void *arg2)
{
    task_arg *arg=(task_arg*)arg2;
    int i=0;
    for(i=arg->start;i<=arg->end;i++){
        printf("%s run %d\n",arg->name,i);
        usleep(200);
    }

    return (void*)0;
}

void* task3_function(void *arg3)
{
    task_arg *arg=(task_arg*)arg3;
    return (void*)(arg->start+arg->end);

}

int main(int argc,char **argv)
{
    int err;
    pthread_t task1,task2,task3;  //定义线程标识符(每个进程内部的不同线程都有自己的唯一标识ID)
    task_arg task1_arg={10,90,"task1"},task2_arg={40,90,"task2"},task3_arg={140,190,"task3"};

    if((err=pthread_create(&task1,NULL,task1_function,(void*)&task1_arg))!=0){      //创建task1线程(线程ID，线程属性指针，线程函数的地址，线程函数参数的地址)
        perror("pthread create err!");
    }
    if((err=pthread_create(&task2,NULL,task2_function,(void*)&task2_arg))!=0){    
        perror("pthread create err!");
    }
    if((err=pthread_create(&task3,NULL,task3_function,(void*)&task3_arg))!=0){    
        perror("pthread create err!");
    }

    printf("main thread id:%d\n",pthread_self());
    
    //主控线程阻塞一段时间，否则遇到return 0进程直接终止，则所有线程被杀死
    //sleep(10); 


    /**
     * 主控线程调用pthread_join()自己会阻塞直到task1线程结束
     * 线程结束后其占用的资源并不会随着线程结束而释放，所以需要
     * pthread_join函数等待线程结束并释放资源
     */
    pthread_join(task1,NULL);       
    pthread_join(task2,NULL);

    int *result=0;
    pthread_join(task3,(void**)&result);
    printf("result is %d\n",result);           


    printf("finish\r\n");

    return 0;

}