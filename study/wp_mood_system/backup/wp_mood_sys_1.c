#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wp_mood_sys.h"

#define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif


/**
 * @brief 根据邻接表名称匹配邻接表,返回邻接表地址
 * @param name 邻接表名
 *      
 * 
 */
struct Mood_List* mood_list_locate_by_name(char *mood_name)
{
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){        
        if(strcmp(list->property.name,mood_name)==0){
            return list;
        } 
        list=list->next;
    }
    return NULL;
}

/**
 * @brief 根据邻接表ID匹配邻接表,返回邻接表地址
 * @param id 邻接表ID
 * 
 */
struct Mood_List* mood_list_locate_by_id(uint8_t id)
{
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){        
        if(list->property.id==id){
            return list;
        } 
        list=list->next;
    }
    return NULL;
}

uint8_t mood_list_get_by_name(char *mood_name)
{
    struct Mood_List *list=mood_list_locate_by_name(mood_name);
    if(list==NULL){
        return 1;
    }
    printf("[%d]:%s,status=%d\r\n",list->property.id,list->property.name,list->property.status);
    return 0;
    
}

uint8_t mood_list_get_by_id(uint8_t id)
{
    struct Mood_List *list=mood_list_locate_by_id(id);
    if(list==NULL){
        return 1;
    }
    printf("[%d]:%s,status=%d\r\n",list->property.id,list->property.name,list->property.status);
    return 0;
    
}

void mood_list_get_all(void)
{
    printf("----------------------------------\r\n");
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){
        printf("[%d]:%s,status=%d\r\n",list->property.id,list->property.name,list->property.status);
        list=list->next;
    }
    printf("----------------------------------\r\n");


}

void mood_node_get_all(void) 
{
    printf("-------------------start print the node----------------------\r\n");
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){    
        printf("[%d]:%s\r\n",list->property.id,list->property.name);
        struct Mood_Node* node = list->head;
        while (node != NULL) {
                struct Mood_List *list1=mood_list_locate_by_id(node->dst);
                if(list1!=NULL){
                    printf("->[%d]:%s\n",node->dst,list1->property.name);
                }
                node = node->next;
            }

        list=list->next;

    }

    printf("-------------------end print the node----------------------\r\n");
    printf("\r\n");
}


/**
 * @brief 添加一个新的邻接表节点到图中,只能添加一次
 * @param src 源邻接表索引
 * @param dst 目标邻接表索引
 * 
 */
uint8_t mood_node_create(char *src_mood,char *dst_mood)
{

    struct Mood_List *src=mood_list_locate_by_name(src_mood);
    if(src==NULL){
        printf("mood [%s] not exist\r\n",src_mood);
        return 2; 
    }
    struct Mood_List *dst=mood_list_locate_by_name(dst_mood);
    if(dst==NULL){
         printf("mood [%s] not exist\r\n",dst_mood);
        return 2; 
    }
   
    /* 检查是否已经被添加过,单方向检查*/
    struct Mood_List *list=&stMood_List;
    struct Mood_Node *node1=list->head,*node2=NULL;
    while(node1!=NULL){
        if(node1->dst!=dst->property.id){
            node2=node1;
            node1=node1->next;
        }else{
            printf("node already added\r\n");
            return 3;
        }
    }

    node1=(struct Mood_Node*)malloc(sizeof(struct Mood_Node));
    if(node1 == NULL){
        printf("alloc fail\r\n");
        return 1;
    }
    node2=(struct Mood_Node*)malloc(sizeof(struct Mood_Node));
    if(node2 == NULL){
        printf("alloc fail\r\n");
        free(node1);
        return 1;
    }

    node1->dst=src->property.id;
    node1->next=dst->head;
    dst->head=node1;

    node2->dst=dst->property.id;
    node2->next=src->head;
    src->head=node2;

    return 0;
}


uint8_t mood_node_delete(char *src_mood,char *dst_mood)
{

    struct Mood_List *src=mood_list_locate_by_name(src_mood);
    if(src==NULL){
        printf("mood [%s] not exist\r\n",src_mood);
        return 2; 
    }
    struct Mood_List *dst=mood_list_locate_by_name(dst_mood);
    if(dst==NULL){
         printf("mood [%s] not exist\r\n",dst_mood);
        return 2; 
    }
   
    /* 检查两情绪是否存在关系,单方向检查*/
    struct Mood_List *list=&stMood_List;
    struct Mood_Node *node1=list->head,*node2=NULL;
    while(node1!=NULL){
        if(node1->dst!=dst->property.id){
            node2=node1;
            node1=node1->next;
        }else{
            printf("node already added\r\n");
            return 3;
        }
    }

    node1=src->head;
    node2=NULL;
    while(node1!=NULL){
        if(node1->dst!=dst->property.id){
            node2=node1;
            node1=node1->next;
        }else{
            if(node2 == NULL){
                src->head=node1->next;
            }else{
                node2->next=node1->next;
            }
            free(node1);
            break;
        }
    }


    node1=dst->head;
    node2=NULL;
    while(node1!=NULL){
        if(node1->dst!=src->property.id){
            node2=node1;
            node1=node1->next;
        }else{
            if(node2 == NULL){
                dst->head=node1->next;
            }else{
                node2->next=node1->next;
            }
            free(node1);
            break;
        }
    }

    return 0;
}


uint8_t mood_list_create(char *mood_name)
{
    struct Mood_List *list1=(struct Mood_List*)malloc(sizeof(struct Mood_List));
    if(list1==NULL){
        printf("malloc fail\r\n");
        return 1;
    }
    strncpy(list1->property.name,mood_name,sizeof(list1->property.name));
    list1->property.id=gMood_Num;
    list1->property.status=MOOD_PEND;
    list1->property.value=100;
    list1->head=NULL;
    list1->next=NULL;

    struct Mood_List *list2=&stMood_List;
    while(list2->next!=NULL){
        list2=list2->next;
    }
    list2->next=list1;
    gMood_Num++;
    return 0;

}


/**
 * @brief 删除情绪链表(删除情绪)
 *        默认传入的参数正确
 * @param mood_name 情绪名
 * 
 * 
 */
uint8_t mood_list_delete(char *mood_name)
{

    struct Mood_List *list1=&stMood_List,*list2=NULL;
    while(list1!=NULL){        
        if(strcmp(list1->property.name,mood_name)==0){
            break;
        }else{
            list2=list1;
            list1=list1->next;
        }
    }
    list2->next=list1->next;
    free(list1);

    list1=list2->next;
    while(list1!=NULL){  
        list1->property.id--;
        list1=list1->next;
    }
    gMood_Num--;
    return 0;
}


/**
 * @brief 创建新情绪
 *        1.检查情绪是否已被注册
 *        2.将新情绪添加到情绪链表中
 * 
 * 
 * 
 */
uint8_t mood_create(char *mood_name)
{

    struct Mood_List *list1=&stMood_List;
    while(list1!=NULL){        
        if(strcmp(list1->property.name,mood_name)==0){
            printf("create mood %s fail,mood %s already exist\r\n",mood_name,mood_name);
            return 1;
        } 
        list1=list1->next;
    }

    

    if(mood_list_create(mood_name)){
        return 1;
    }

    return 0;

}


/**
 * @brief 删除情绪,即删除邻接表
 *        1.检查情绪是否存在
 *        2.删除该链表的各节点
 *        3.情绪数量-1
 *        4.重新计算各情绪id   
 * 
 */
uint8_t mood_delete(char *mood_name)
{

    // if(strcmp("BASE",mood_name)==0){
    //     return 1;
    // }

    // while(list1!=NULL){        
    //     if(strcmp(list1->property.name,mood_name)==0){
    //         break;
    //     }else{
    //         list2=list1;
    //         list1=list1->next;
    //     }
    // }
    // if(list1==NULL){
    //     printf("delete mood %s fail,mood %s not exist\r\n",mood_name,mood_name);
    //     return 1;
    // }
  
} 


//    struct Mood_Node *node=list1->head;
//     while(node!=NULL){
//         list2=mood_list_locate_by_id(node->dst);
//         node = node->next;
//         if(list2!=NULL){
//             mood_node_delete(list1->property.name,list2->property.name);
//         }
        
//     }

int main()
{


    mood_create("BASE");
    mood_create("HAPPY");
    mood_create("PEACE");
    mood_create("SAD");
    mood_create("FEAR");
    mood_create("ANGRY");
    mood_list_get_all();


    // mood_node_create("BASE","HAPPY");
    // mood_node_create("HAPPY","PEACE");
    // mood_node_create("PEACE","SAD");
    // mood_node_create("PEACE","FEAR");
    // mood_node_create("PEACE","ANGRY");
    // mood_node_create("SAD","FEAR");
    // mood_node_create("SAD","ANGRY");
    // mood_node_get_all() ;

    // // mood_node_delete("PEACE","SAD");
    // // mood_node_delete("PEACE","ANGRY");
    // // mood_node_delete("SAD","ANGRY");
    // // mood_node_get_all() ;

    // mood_list_delete("SAD");
    // mood_node_get_all() ;


    // mood_list_delete("HAPPY");
    // mood_list_get_all();
    // mood_node_get_all() ;


    // struct Mood_List* list=mood_list_locate_by_name("PEACE");
    // printf("name=%s,num=%d\r\n",list->property.name,list->property.id);


    // list=mood_list_locate_by_id(3);
    //  printf("name=%s,num=%d\r\n",list->property.name,list->property.id);


    //  mood_list_get_by_name("HAPPY");
    //  mood_list_get_by_id(1);

    //  struct Mood_List* list1=mood_list_locate_by_name("ANGRY");
    // printf("----num=%d\r\n",list1->id);


    //mood_node_get_all();



    return 0;
}






