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
 * @brief 根据名称匹配情绪
 * @param name 情绪名称
 * @return 情绪对应的邻接表地址
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
 * @brief 根据名称检查情绪是否存在
 * @param name 情绪名称
 * @return 0 存在
 *         1 不存在
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
 * @brief 根据ID匹配情绪
 * @param id 情绪ID
 * @return 情绪(邻接表)地址
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

/**
 * @brief 根据名称匹配情绪,若存在则打印属性信息
 * @param name 情绪名称
 *      
 */
uint8_t mood_list_print_by_name(char *mood_name)
{
    struct Mood_List *list=mood_list_locate_by_name(mood_name);
    if(list==NULL){
        return 1;
    }
    printf("[%d]:%s,status=%d\r\n",list->property.id,list->property.name,list->property.status);
    return 0;
    
}

/**
 * @brief 根据ID匹配情绪,若存在则打印属性信息
 * @param name 情绪名称
 *      
 */
uint8_t mood_list_print_by_id(uint8_t id)
{
    struct Mood_List *list=mood_list_locate_by_id(id);
    if(list==NULL){
        return 1;
    }
    printf("[%d]:%s,status=%d\r\n",list->property.id,list->property.name,list->property.status);
    return 0;
    
}

/**
 * @brief 打印所有情绪的属性信息
 *      
 */
void mood_list_print_all(void)
{
    printf("----------------------------------\r\n");
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){
        printf("[%d]:%s,status=%d\r\n",list->property.id,list->property.name,list->property.status);
        list=list->next;
    }
    printf("----------------------------------\r\n");


}

/**
 * @brief 打印所有情绪节点的属性信息
 *      
 */
void mood_node_print_all(void) 
{
    printf("-------------------start print the node----------------------\r\n");
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){    
        printf("[%d]:%s\r\n",list->property.id,list->property.name);
        struct Mood_BindNode* node = list->head;
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
 * @brief 设置情绪状态,即Mood_Property.status
 * @param list 要操作的情绪
 * @param position 具体位
 *                 第[0]位 运行状态
 *                 第[1]位 触发状态
 *  
 */
void mood_set_status(struct Mood_List *list,uint8_t position,uint8_t status)
{
    if(status){
        list->property.status |= (1 << position);
    }else{
        list->property.status &= ~(1 << position);
    }
   
}

/**
 * @brief 获取情绪状态,即Mood_Property.status
 * @param list 要操作的情绪
 * @param position 具体位
 *                 第[0]位 运行状态
 *                 第[1]位 触发状态
 *  
 */
uint8_t mood_get_status(struct Mood_List *list,uint8_t position)
{
    if((list->property.status >> position) & 1) return 1;
    else return 0;

}


/**
 * @brief 设置触发器id,即Mood_Property.status
 * @param list 要操作的情绪
 * @param position 具体位
 *                 第[0]位 运行状态
 *                 第[1]位 触发状态
 *  
 */
void mood_set_trigger(struct Mood_List *list,uint8_t position,uint8_t status)
{
    if(status){
        list->property.status |= (1 << position);
    }else{
        list->property.status &= ~(1 << position);
    }
   
}

/**
 * @brief 获取情绪状态,即Mood_Property.status
 * @param list 要操作的情绪
 * @param position 具体位
 *                 第[0]位 运行状态
 *                 第[1]位 触发状态
 *  
 */
uint8_t mood_get_trigger(struct Mood_List *list,uint8_t position)
{
    if((list->property.status >> position) & 1) return 1;
    else return 0;

}

/**
 * @brief 绑定两个情绪
 *        默认传入参数正确
 * @param src 源情绪
 * @param dst 目标情绪
 * @param relevance 相关性参数,描述源情绪与目标情绪的相关性
 *                  
 * 
 */
uint8_t mood_node_create(struct Mood_List *src,struct Mood_List *dst,float relevance)
{
    relevance=LIMIT(relevance,-1,1);
    struct Mood_BindNode *node1=(struct Mood_BindNode*)malloc(sizeof(struct Mood_BindNode));
    if(node1 == NULL){
        printf("alloc fail\r\n");
        return 1;
    }
    struct Mood_BindNode *node2=(struct Mood_BindNode*)malloc(sizeof(struct Mood_BindNode));
    if(node2 == NULL){
        printf("alloc fail\r\n");
        free(node1);
        return 1;
    }

    node1->dst=src->property.id;
    node1->rel=relevance;
    node1->next=dst->head;
    dst->head=node1;

    node2->dst=dst->property.id;
    node2->rel=relevance;
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
    struct Mood_BindNode *node1=src->head,*node2=NULL;
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
    list->property.value=VALUE_BASE;
    mood_set_status(list,RUN_BIT,PEND);
    mood_set_status(list,TRIGGER_BIT,TRIGGER_OFF);
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
 * @param relecance 源情绪与目标情绪的相关性
 * 
 */
uint8_t mood_bind(char *src,char *dst,float relevance)
{

    uint8_t ret=0;

    struct Mood_List *list1=mood_list_locate_by_name(src);
    struct Mood_List *list2=mood_list_locate_by_name(dst);
    if(list1==NULL || list2==NULL){
        printf("mood %s or %s not exist\r\n",src,dst);
        return 1;
    }

    /* 检查是否已经被添加过,单方向检查*/
    struct Mood_BindNode *node1=list1->head,*node2=NULL;
    while(node1 !=NULL){
        if(node1->dst!=list2->property.id){
            node2=node1;
            node1=node1->next;
        }else{
            printf("node already added\r\n");
            return 3;
        }
    }

    ret=mood_node_create(list1,list2,relevance);
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
    struct Mood_BindNode *node1=list1->head,*node2=NULL;
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
    struct Mood_BindNode *node=list1->head;
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
void mood_print(void)
{

    struct Mood_List *list=&stMood_List;
    while(list!=NULL){    
        printf("[%d]:%s=%.2f\r\n",list->property.id,list->property.name,list->property.value);

        struct Mood_BindNode* node = list->head;
        while (node != NULL) {
                struct Mood_List *list1=mood_list_locate_by_id(node->dst);
                if(list1!=NULL){
                    printf("--->[%d]:%s,rel=%.2f\n",node->dst,list1->property.name,node->rel);
                }
                node = node->next;
            }

        list=list->next;

    }
    printf("\r\n");


}

/**
 * @brief 设置情绪属性
 *        设置触发器状态(status[1]),trigger,value属性
 * 
 */
uint8_t mood_set_property(char *mood_name,struct Mood_Property* property)
{
    struct Mood_List *list=mood_list_locate_by_name(mood_name);
    if(list==NULL){
        return 1;
    }
    if((property->status >> TRIGGER_BIT) & 1) {
        mood_set_status(list,TRIGGER_BIT,1);
    }else{
        mood_set_status(list,TRIGGER_BIT,0);
    }
    list->property.trigger=property->trigger;
    list->property.value=property->value;

    return 0;
}




/**
 * @brief 1.将输入情绪的value+VALUE_INCREASE
 *        2.对value限幅
 *        3.同时遍历该情绪的所有节点(相关情绪),更改对应的value
 * 
 */
void mood_increase(struct Mood_List *list)
{
    list->property.value+=VALUE_INCREASE;
    list->property.value=LIMIT(list->property.value,VALUE_MIN,VALUE_MAX);
    struct Mood_BindNode *node=list->head;
    while (node!=NULL){
        struct Mood_List *list1 =mood_list_locate_by_id(node->dst);
        if(list1!=NULL){
            list1->property.value+=VALUE_INCREASE * node->rel;
        }
        node=node->next;
    }
}

/**
 * @brief 获取当前状态为READY的情绪
 * 
 */
struct Mood_List* mood_locate_ready(void)
{
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){
        if(mood_get_status(list,RUN_BIT)!=READY){
            list=list->next;
        }else{
            return list;
        }
    }
    return NULL;

}

/**
 * @brief 解析触发标志位,判断是哪一个事件触发
 *        并将对应的情绪的状态置为READY
 * 
 */
struct Mood_List* mood_locate_trigger(uint16_t trigger)
{
    struct Mood_List *list=&stMood_List;
    
}


/**
 * @brief 1.mood_locate_ready
 *        2.对当前状态为READY的情绪执行mood_increase操作
 *        2.将当前情绪的状态置为PEND
 *        
 */
uint8_t mood_update(uint16_t trigger)
{
    if(trigger){//有事件触发
        struct Mood_List *list=mood_locate_trigger(trigger);


    }else{//无事件触发


    }













    struct Mood_List *list=mood_locate_ready();
    if(list==NULL){
        return 1;
    }





    mood_increase(list);


    mood_set_status(list,RUN_BIT,PEND);
    return 0;
}


int main()
{

    mood_create("HAPPY");
    mood_create("PEACE");
    mood_create("SAD");
    mood_create("FEAR");
    mood_create("ANGRY");
    mood_create("SPECAIL");
    

    mood_bind("BASE","HAPPY",0.1);
    mood_bind("HAPPY","PEACE",0.7);
    mood_bind("HAPPY","SAD",-0.5);
    mood_bind("PEACE","SAD",-0.2); 
    mood_bind("PEACE","FEAR",-0.11);
    mood_bind("PEACE","ANGRY",-0.1);
    mood_bind("SAD","FEAR",0.11);
    mood_bind("SAD","ANGRY",0.2);

    mood_print();

    // mood_unbind("HAPPY","SAD");
    // mood_unbind("ANGRY","SAD");

    mood_destroy("ANGRY");
    mood_print();


    // mood_set_trigger_status(&stMood_List,1);
    // printf("1111111111111=%d\r\n",stMood_List.property.status);



    uint8_t i=10;
    while(i)
    {
        mood_set_status(stMood_List.next,RUN_BIT,READY);
        //stMood_List.next->property.status=READY;
        printf("i=%d\r\n",i);
        mood_update(1);
        mood_print();
        i--;
    }

    


    // mood_unbind("SAD","FEAR");
    // mood_unbind("BASE","HAPPY");
    // mood_unbind("PEACE","FEAR");
    // mood_node_get_all();




    return 0;
}






