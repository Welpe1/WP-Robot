#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "wp_mood_queue.h"

#define MAX_SIZE    100

struct Circular_Queue{
    char data[MAX_SIZE];
    uint8_t front;
    uint8_t rear;
    uint8_t size;
};


struct Circular_Queue stCirQueue;


void circular_queue_init(struct Circular_Queue *queue)
{
    memset(queue, 0, sizeof(struct Circular_Queue));
}


bool circular_queue_isempty(struct Circular_Queue *queue)
{
    return queue->size == 0;
}

bool circular_queue_isfull(struct Circular_Queue *queue)
{
    return queue->size == MAX_SIZE;
}

uint8_t circular_queue_haduse(struct Circular_Queue *queue)
{
    return queue->size;

}

uint8_t circular_queue_nouse(struct Circular_Queue *queue)
{
    return (MAX_SIZE-queue->size);

}



bool circular_queue_write(struct Circular_Queue *queue,uint8_t *buf,uint8_t len)
{
    
    if(circular_queue_nouse(queue)<len){
        printf("no enough space \r\n");
        return false;
    }   
    for(uint8_t i=0;i<len;i++){
        queue->data[queue->rear]=buf[i];
        queue->rear=(queue->rear+1)%MAX_SIZE;
        queue->size++;
    }
    return true;
}

bool circular_queue_read(struct Circular_Queue *queue,uint8_t *buf,uint8_t len)
{
    if(circular_queue_haduse(queue)<len){
        return false;
    }
    for(uint8_t i=0;i<len;i++){
        buf[i]=queue->data[queue->front];
        queue->front=(queue->front+1)%MAX_SIZE;
        queue->size--;
    }
    return true;
}


uint8_t mood_check_file(char *target,uint8_t target_len)
{
    char read_buf[10];
    char *strx=NULL;
    struct Circular_Queue *queue=&stCirQueue;
    uint8_t len=0;

    if(queue->size==0){//队列为空

    }else{
        len=queue->size;
        if(target_len<len) len = target_len;



    }


    while(strx==NULL){
        if(queue->size!=0){//队列不为空
            
            for(uint8_t i=0;i<target;i++){
                read_buf[i]=queue->data[queue->front];
                queue->front=(queue->front+1)%MAX_SIZE;
                queue->size--;
    }
            
        
        }
        strx=strstr("hell",target);
    }









    if(circular_queue_isempty(&stCirQueue)==0){//队列不为空
        uint8_t len=circular_queue_haduse(&stCirQueue);
        



    }


    // char *strx = NULL;
    // uint8_t read_buf[MAX_SIZE];
    // if(circular_queue_isempty(&stCirQueue)==0){//对列不为空
    //     uint8_t len=circular_queue_haduse(&stCirQueue);
    //     memset(read_buf,0,MAX_SIZE);
    //     circular_queue_read(&stCirQueue,read_buf,len);
    //     read_buf[MAX_SIZE]='\0';
    //     printf("read_buf=%s\r\n",read_buf);

    // }
    // strx=strstr(read_buf,start);
    // printf("strx=%s\r\n",strx);
    // if(strx==NULL) return 1;
    // return 0;

}




int main()
{

    circular_queue_init(&stCirQueue);
    circular_queue_write(&stCirQueue,"this is happy test\r\n",MAX_SIZE);
    mood_check_format("happy");
    circular_queue_write(&stCirQueue,"ppy test\r\n",MAX_SIZE);
    mood_check_format("happy");

 


    


    // circular_queue_write(&queue,buf,10);
    // memset(buf,0,sizeof(buf));
    // circular_queue_read(&queue,buf,10);


    //   for(uint8_t i=0;i<10;i++){
    //     printf("buf[%d]=%d\r\n",i,buf[i]);
    // }




    return 0;

}
