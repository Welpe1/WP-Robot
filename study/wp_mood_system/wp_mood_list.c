#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "wp_mood_list.h"

#define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif

/**
 * @brief 添加链表节点(默认插在最后一个)并自动分配id
 * @param name 添加的节点的名称
 * 
 */
uint8_t mood_list_add(char *name)
{
    struct Mood_List* node = (struct Mood_List*)malloc(sizeof(struct Mood_List));
    if (node==NULL){
        printf("malloc fail\r\n");
        return 1;  // 检查内存分配是否成功
    }
    node->next=NULL;
    strncpy(node->name,name,sizeof(node->name));

    struct Mood_List *temp_node;
    temp_node=&stMood_List;
    while(temp_node->next!=NULL){
        temp_node=temp_node->next;
    }
    temp_node->next=node;
    node->id=temp_node->id+1;
    return 0;
}

/**
 * @brief 删除节点并重新计算id,默认不可以删除头节点
 * @param name 删除的节点的名称
 * 
 */
uint8_t mood_list_delete(char *name)
{
    if(strcmp(name,"BASE")==0){
        printf("can not delete the head node\r\n");
        return 1;
    }
    struct Mood_List* node,*temp_node;
    node=&stMood_List;
    while(node->next != NULL && strcmp(node->next->name, name) != 0) {
        node = node->next;
    }
    if (node->next==NULL) {//如果没有找到节点,返回错误
        printf("no node found\r\n");
        return 1;
    }
    temp_node=node->next->next;
    free(node->next);
    node->next=temp_node;
    while (node->next != NULL) {
        node = node->next;
        node->id -= 1;
    }
    return 0;
}


uint8_t mood_list_init()
{
    mood_list_add("HAPPY");
    mood_list_add("PEACE");
    mood_list_add("SAD");
    mood_list_add("FEAR");
    mood_list_add("ANGRY");

    return 0;
}



int main()
{

    mood_list_init();
    struct Mood_List *current = &stMood_List;
    for (int i = 0; i < 6; i++) {
        printf("id[%d]=%d, name=%s\n", i, current->id, current->name);
        current = current->next;
    }
    printf("\r\n");

    mood_list_add("SPECIAL");
    current = &stMood_List;
    for (int i = 0; i < 7; i++) {
        printf("id[%d]=%d, name=%s\n", i, current->id, current->name);
        current = current->next;
    }
    printf("\r\n");

    mood_list_delete("HAPPY");
    current = &stMood_List;
    for (int i = 0; i < 6; i++) {
        printf("id[%d]=%d, name=%s\n", i, current->id, current->name);
        current = current->next;
    }
    printf("\r\n");

     mood_list_delete("1");
    current = &stMood_List;
    for (int i = 0; i < 6; i++) {
        printf("id[%d]=%d, name=%s\n", i, current->id, current->name);
        current = current->next;
    }
    printf("\r\n");




   // free_mood_list(&stMood_List);  // 释放链表内存

    return 0;
}