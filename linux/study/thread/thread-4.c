/**
 * 读写锁适合当读操作较多而写操作较少的情况，可以提高线程并发性
 * 读写锁的创建和销毁 pthread_rwlock_init pthread_rwlock_destroy
 * 上锁 pthread_rwlock_rdlock（加读锁） pyhread_rwlock_wrlock（加写锁）
 * 解锁 pthread_rwlock_unlock
 * 读写锁数据类型 pthread_rwlock_t
 * 
 * 读和读之间不排斥
 * 读和写之间排斥
 * 写和写之间排斥
 * 
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_rwlock_t rwlock;    

int main()
{   
    pthread_rwlock_init(&rwlock,NULL);      //读写锁初始化

    pthread_rwlock_rdlock(&rwlock);     //加读锁

    pthread_rwlock_wrlock(&rwlock);     //加写锁

    pthread_rwlock_destroy(&rwlock);        //销毁锁





}