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
 * @brief 根据邻接表名称匹配邻接表
 *        如果存在,返回0
 *        不存在,返回1
 * @param name 邻接表名
 *      
 * 
 */
uint8_t mood_list_check_by_name(char *mood_name)
{
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){        
        if(strcmp(list->property.name,mood_name)==0){
            return 0;
        } 
        list=list->next;
    }
    return 1;

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
                    printf("--->[%d]:%s\n",node->dst,list1->property.name);
                }
                node = node->next;
            }

        list=list->next;

    }

    printf("-------------------end print the node----------------------\r\n");
    printf("\r\n");
}


/**
 * @brief 添加一个新的邻接表节点到图中
 *        默认传入参数正确
 * @param src 源邻接表
 * @param dst 目标邻接表
 * 
 */
uint8_t mood_node_create(struct Mood_List *src,struct Mood_List *dst)
{
    struct Mood_Node *node1=(struct Mood_Node*)malloc(sizeof(struct Mood_Node));
    if(node1 == NULL){
        printf("alloc fail\r\n");
        return 1;
    }
    struct Mood_Node *node2=(struct Mood_Node*)malloc(sizeof(struct Mood_Node));
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

/**
 * @brief 删除图中的节点
 *        默认传入参数正确
 * @param src 源邻接表
 * @param dst 目标邻接表
 * 
 */
void mood_node_destroy(struct Mood_List *src,struct Mood_List *dst)
{
    struct Mood_Node *node1=src->head,*node2=NULL;
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

}


/**
 * @brief 创建情绪链表
 *        默认传入的参数正确
 * @param list 已分配内存的链表
 * @param mood_name 情绪名
 * 
 */
void mood_list_create(struct Mood_List *list,char *mood_name)
{
    strncpy(list->property.name,mood_name,sizeof(list->property.name));
    list->property.id=gMood_Num;
    list->property.status=PEND;
    list->property.value=100;
    list->property.trigger=TRIGGER_OFF;
    list->property.group=POSITIVE;
    list->head=NULL;
    list->next=NULL;

    struct Mood_List *list1=&stMood_List;
    while(list1->next!=NULL){
        list1=list1->next;
    }
    list1->next=list;
    gMood_Num++;
}

/**
 * @brief 删除情绪链表(删除情绪)
 *        默认传入的参数正确
 * @param mood_name 情绪名
 * 
 * 
 */
void mood_list_destroy(char *mood_name)
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
}


/***************************************************************************************************/



/**
 * @brief 绑定两情绪,只能绑定一次
 *        1.检查情绪是否存在
 *        2.检查情绪是否已经被绑定
 *        3.绑定情绪
 * @param src 源情绪 
 * @param dst 目标情绪
 * 
 */
uint8_t mood_bind(char *src,char *dst)
{

    uint8_t ret=0;

    struct Mood_List *list1=mood_list_locate_by_name(src);
    struct Mood_List *list2=mood_list_locate_by_name(dst);
    if(list1==NULL || list2==NULL){
        printf("mood %s or %s not exist\r\n",src,dst);
        return 1;
    }

    /* 检查是否已经被添加过,单方向检查*/
    struct Mood_Node *node1=list1->head,*node2=NULL;
    while(node1 !=NULL){
        if(node1->dst!=list2->property.id){
            node2=node1;
            node1=node1->next;
        }else{
            printf("node already added\r\n");
            return 3;
        }
    }

    ret=mood_node_create(list1,list2);
    return ret;

}


/**
 * @brief 解绑两情绪,只能解绑绑定一次
 *        1.检查情绪是否存在
 *        2.检查情绪是否已经被绑定
 *        3.解绑情绪
 * @param src 源情绪 
 * @param dst 目标情绪
 * 
 */
uint8_t mood_unbind(char *src,char *dst)
{

    struct Mood_List *list1=mood_list_locate_by_name(src);
    struct Mood_List *list2=mood_list_locate_by_name(dst);
    if(list1==NULL || list2==NULL){
        printf("mood %s or %s not exist\r\n",src,dst);
        return 1;
    }

    /* 检查是否已经被添加过,单方向检查*/
    struct Mood_Node *node1=list1->head,*node2=NULL;
    while(node1 !=NULL){
        if(node1->dst!=list2->property.id){
            node2=node1;
            node1=node1->next;
        }else{
            break;
        }
    }
    if(node1 ==NULL){
        printf("mood %s have not binded to mood %s\r\n");
        return 2;
    }

    mood_node_destroy(list1,list2);

    return 0;

}

/**
 * @brief 创建新情绪
 *        1.检查情绪是否已被创建
 *        2.将新情绪添加到情绪链表中
 * @param mood_name 创建的情绪名
 * 
 */
uint8_t mood_create(char *mood_name)
{
    if(mood_list_check_by_name(mood_name)==0){
        printf("mood %s already exist\r\n",mood_name);
        return 1;
    }
    struct Mood_List *list=(struct Mood_List*)malloc(sizeof(struct Mood_List));
    if(list==NULL){
        printf("malloc fail\r\n");
        return 2;
    }
    mood_list_create(list,mood_name);
    return 0;

}


/**
 * @brief 删除情绪
 *        1.检查情绪是否存在
 *        2.删除该链表的各节点
 *        3.删除链表
 *        4.重置id
 * @param mood_name 删除的情绪名
 * 
 */
uint8_t mood_destroy(char *mood_name)
{
    if(strcmp("BASE",mood_name)==0){
        printf("can not destroy mood BASE\r\n");
        return 1;
    }

    struct Mood_List *list1=mood_list_locate_by_name(mood_name);
    if(list1==NULL){
        printf("destroy mood %s fail,mood %s not exist\r\n",mood_name,mood_name);
        return 1;
    }
    uint8_t delele_id=list1->property.id;
    struct Mood_List *list2=NULL;
    struct Mood_Node *node=list1->head;
    while(node!=NULL){
        list2=mood_list_locate_by_id(node->dst);
        node = node->next;
        if(list2!=NULL){
            mood_node_destroy(list1,list2);
        }
        
    }
    mood_list_destroy(mood_name);
    list1=&stMood_List;
    while(list1!=NULL){
        node=list1->head;
        while(node!=NULL){
            if(node->dst>=delele_id){
                node->dst--;
            }
            node=node->next;
        }
        list1=list1->next;
    }

    return 0;

} 

/**
 * @brief 打印情绪图
 * 
 */
void mood_get(void)
{

    struct Mood_List *list=&stMood_List;
    while(list!=NULL){    
        printf("[%d]:%s=%d\r\n",list->property.id,list->property.name,list->property.value);

        struct Mood_Node* node = list->head;
        while (node != NULL) {
                struct Mood_List *list1=mood_list_locate_by_id(node->dst);
                if(list1!=NULL){
                    printf("--->[%d]:%s\n",node->dst,list1->property.name);
                }
                node = node->next;
            }

        list=list->next;

    }
    printf("\r\n");


}

/**
 * @brief 设置情绪属性
 *        只能设置group,trigger,value属性
 * 
 */
uint8_t mood_set_property(char *mood_name,struct Mood_Property* property)
{
    struct Mood_List *list=mood_list_locate_by_name(mood_name);
    if(list==NULL){
        return 1;
    }
    list->property.group=property->group;
    list->property.trigger=property->trigger;
    list->property.value=property->value;

    return 0;
}


/**
 * @brief 情绪值value调整
 *        根据源情绪与目标情绪的关系,调整目标情绪的value,具体为
 *        源情绪为POSITIVE 目标情绪为POSITIEV value +2
 *                                 NORMAL          +1
 *                                 NEGATIVE        -2
 *        源情绪为NORMAL   目标情绪为POSITIEV value +1
 *                                  NORMAL         +2
 *                                  NEGATIVE       -1
 *        源情绪为NEGATIVE 目标情绪为POSITIEV value  -2
 *                                  NORMAL         -1
 *                                  NEGATIVE       +2
 * 
 * @param src 源情绪
 * @param dst 目标情绪
 */
void mood_value_adjust(struct Mood_List *src,struct Mood_List *dst) 
{
    if(src->property.group==dst->property.group) {
        dst->property.value+=2;
    }else{
        switch(src->property.group){
            case POSITIVE:
                if(dst->property.group==NORMAL){
                    dst->property.value+=1;
                }else if(dst->property.group==NEGATIVE){
                    dst->property.value-=2;
                }
                break;
            case NORMAL:
                if(dst->property.group==POSITIVE){
                    dst->property.value+=1;
                }else if(dst->property.group==NEGATIVE){
                    dst->property.value-=1;
                }
                break;
            case NEGATIVE:
                if (dst->property.group==POSITIVE){
                    dst->property.value-=2;
                }else if(dst->property.group==NORMAL){
                    dst->property.value-=1;
                }
                break;
        }
    }
}


/**
 * @brief 1.遍历所有情绪,对当前状态为READY的情绪执行value+3操作
 *        2.同时遍历该情绪链表的所有节点,对与其关联的情绪执行mood_value_adjust
 *        3.将当前情绪的状态置为PEND
 *        
 */
void mood_increase(void)
{
    struct Mood_List *list1=&stMood_List;
    while(list1!=NULL){
        if(list1->property.status==READY){
            list1->property.value+=3;   //当前情绪value+3
            struct Mood_Node *node=list1->head;
            while (node!=NULL){
                struct Mood_List *list2 =mood_list_locate_by_id(node->dst);
                if(list2!=NULL){
                    mood_value_adjust(list1,list2);
                }
                node=node->next;
            }
            list1->property.status=PEND;
        }else{
            list1=list1->next;
        }
    }
}


void mood_update(uint16_t trigger)
{







}


int main()
{

    mood_create("HAPPY");
    mood_create("PEACE");
    mood_create("SAD");
    mood_create("FEAR");
    mood_create("ANGRY");
    mood_create("SPECAIL");
    

    mood_bind("BASE","HAPPY");
    mood_bind("HAPPY","PEACE");
    mood_bind("PEACE","SAD");
    mood_bind("PEACE","FEAR");
    mood_bind("PEACE","ANGRY");
    mood_bind("SAD","FEAR");
    mood_bind("SAD","ANGRY");

    mood_get();
    stMood_List.property.group=NEGATIVE;

    uint8_t i=10;
    while(i)
    {
        stMood_List.next->property.status=READY;
        printf("i=%d\r\n",i);
        mood_increase();
        mood_get();
        i--;
    }

    


    // mood_unbind("SAD","FEAR");
    // mood_unbind("BASE","HAPPY");
    // mood_unbind("PEACE","FEAR");
    // mood_node_get_all();

    // mood_destroy("HAPPY");
    // mood_destroy("FEAR");
    // //mood_unbind("HAPPY","PEACE");
    // mood_node_get_all();


    return 0;
}






