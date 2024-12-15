#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "wp_mood_sys.h"
#include <sys/time.h>

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

uint8_t mood_get_bit(uint64_t *value,uint8_t position)
{

    if((*value >> position) & 1) return 1;
    else return 0;

}

/**
 * @brief 获取情绪状态,即Mood_Property.status
 * @param list 要操作的情绪
 * @param position 具体位
 *                 第[0]位 运行状态
 *                 第[1]位 触发状态
 *  
 */
uint8_t mood_get_status(struct Mood_Property *property,uint8_t position)
{
    if((property->status >> position) & 1) return 1;
    else return 0;

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

void mood_set_bit(uint64_t *value,uint8_t position,uint8_t status)
{
    if(status){
        *value |= (1 << position);
    }else{
        *value &= ~(1 << position);
    }

}


/**
 * @brief 设置情绪状态,即Mood_Property.status
 * @param list 要操作的情绪
 * @param position 具体位
 *                 第[0]位 运行状态
 *                 第[1]位 触发状态
 *  
 */
void mood_set_status(struct Mood_Property *property,uint8_t position,uint8_t status)
{
    mood_set_bit((uint64_t*)&property->status,position,status);
    // if(status){
    //     property->status |= (1 << position);
    // }else{
    //     property->status &= ~(1 << position);
    // }
   
}

/**
 * @brief 设置触发器id,即Mood_Property.status
 * @param list 要操作的情绪
 * @param position 具体位
 *                 第[0]位 运行状态
 *                 第[1]位 触发状态
 *  
 */
void mood_set_trigger(struct Mood_Property *property,uint8_t position,uint8_t status)
{
    mood_set_bit((uint64_t*)&property->trigger,position,status);
    // if(status){
    //     property->status |= (1 << position);
    // }else{
    //     property->status &= ~(1 << position);
    // }
   
}

/**
 * @brief 设置group状态,即Frame_Info.group
 * @param frame 
 * @param position 具体位
 *  
 */
void mood_set_group(struct Frame_Info *frame,uint8_t position,uint8_t status)
{
    mood_set_bit((uint64_t*)&frame->group,position,status);
    // if(status){
    //     property->status |= (1 << position);
    // }else{
    //     property->status &= ~(1 << position);
    // }
   
}


/**
 * @brief 打印所有情绪的帧信息
 * 
 */
void mood_frame_print_all(void)
{
    struct Mood_List *list=&stMood_List;
    while(list!=NULL){
        printf("[%s]:\r\n",list->property.name);
        for(uint8_t i=0;i<MAX_FRAME_NUM;i++){
            if(mood_get_bit((uint64_t*)&list->frame.group,i)==ENABLE){
                printf("group=%d\r\n",i);
                printf("count=%d\r\n",list->frame.count[i]);
                for(uint8_t j=0;j<list->frame.count[i];j++){
                    printf("addr[%d]=%d\r\n",j,list->frame.addr[i][j]);
                }
            }
        }
        list=list->next;
    }
    printf("\r\n");

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
    list->property.value=BASE_MOOD_VALUE;
    mood_set_status(&list->property,RUN_BIT,PEND);
    mood_set_status(&list->property,TRIGGER_BIT,DISABLE);
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

/**
 * @brief 在文件中寻找指定的数据,并将找到的数据的位置返回至buf中
 *        1.一次性读取BLOCK_SIZE字节数据(注意两数据的间隔必须大于BLOCK_SIZE)
 *        2.如果有目标字符串,则返回目标字符串相对于文件头的位置,存在buf中
 *        3.buf第0位一开始置为-1
 *        4.没有则将文件指针前移-target位置,以防目标字符串跨越两个读取块
 *        默认传入参数正确
 * @param fp
 * @param target
 * @param buf
 * 
 * 
 * 
 */
void file_search_data(FILE *fp,char *target,long int *buf) 
{
    #define BLOCK_SIZE  256

    uint8_t target_len=strlen(target);
    char r_buf[BLOCK_SIZE+1];
    memset(r_buf,0,sizeof(r_buf));
    fseek(fp, 0, SEEK_SET);
    long int count = 0;
    buf[count]=-1;

    while(1){
        size_t r_len=fread(r_buf,1,BLOCK_SIZE,fp);
        r_buf[r_len]='\0';
        if(feof(fp)) break;
        char *strx = strstr(r_buf,target);
        if(strx!=NULL){
            long int pos=ftell(fp)-r_len+(strx-r_buf);
            buf[count++] = pos;  
        }else{
            fseek(fp,-target_len,SEEK_CUR); //调整文件指针位置,以防目标字符串跨越两个读取块
        }
    }
}

/**
 * @brief 从字符串中提取第一个整数
 */
int extract_first_int(const char *str)
{
    int num = 0; // 存储提取的数字
    int sign = 1; // 用于处理可能的负号
    int start = -1; // 记录数字开始的索引位置
    int i = 0;
    while(str[i]!='\0') {
        if(str[i]=='-'&&start==-1){
            sign = -1;
            i++;
            continue;
        }
        if(isdigit(str[i])){
            if(start==-1) start = i;
            num=num*10+(str[i]-'0'); 
        }else{
            if(start!=-1) break; 
        }
        i++;
    }
    return sign*num;
}

/***************************************************************************************************/


/**
 * @brief 初始化情绪帧配置
 *        1.遍历FRAME_ROOT文件夹,查找情绪有几个组别,赋值count(最大为MAX_FRAME_NUM),并将对应的group位使能
 *        如:FRAME_ROOT文件夹下只有HAPPY0 HAPPY1,则count[1]=2,group[0]和[1]为ENABLE
 *        2.读取每一个组别文件,获取每一帧的地址,赋值给addr
 *  
 * @param list 情绪
 * 
 */
void mood_get_frame_config(struct Mood_List *list)
{
    char frame_dir[50];
    long int data_buf[MAX_FRAME_NUM];
    for(uint8_t i=0;i<MAX_FRAME_NUM;i++){
        snprintf(frame_dir,sizeof(frame_dir),"%s%s%u",FRAME_ROOT,list->property.name,i); 
        FILE *fp=fopen(frame_dir,"r");
        if(fp!=NULL){
            /* 读取帧总数 */
            printf("open %s success\r\n",frame_dir);
            file_search_data(fp,"count=",data_buf);
            long int offset=data_buf[0];
            memset(data_buf,0,sizeof(data_buf));
            if(offset!=-1){//不等于-1证明读取到
                char buf1[10]; 
                fseek(fp,offset,SEEK_SET);
                fread(buf1,1,10,fp);
                uint8_t frame_num=(uint8_t)extract_first_int(buf1);
                if(frame_num>=MAX_FRAME_NUM) frame_num=MAX_FRAME_NUM;
                list->frame.count[i]=frame_num;
                printf("count[%d]=%d\r\n",i,list->frame.count[i]);
            }
            if(list->frame.count[i]>0){
                /* 读取帧地址 */
                file_search_data(fp,"{",data_buf);
                for(uint8_t j=0;j<list->frame.count[i];j++){
                    list->frame.addr[i][j]=data_buf[j];
                }
            }
            fclose(fp);
            mood_set_group(&list->frame,i,ENABLE);
        }else{
            printf("open %s fail\r\n",frame_dir);
            mood_set_group(&list->frame,i,DISABLE);
        }
    }
}




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
 *        3.初始化帧配置
 * @param mood_name 创建的情绪名
 * 
 */
uint8_t mood_create(char *mood_name)
{
    if(strlen(mood_name)>=10){
        printf("mood_name length need to <= 10\r\n");
        return 1;
    }
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

    mood_get_frame_config(list);

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
        mood_set_status(&list->property,TRIGGER_BIT,1);
    }else{
        mood_set_status(&list->property,TRIGGER_BIT,0);
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
    list->property.value=LIMIT(list->property.value,MIN_MOOD_VALUE,MAX_MOOD_VALUE);
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
        if(mood_get_status(&list->property,RUN_BIT)!=READY){
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
 * 1.有事件触发,
 *        
 */
uint8_t mood_update(uint16_t trigger)
{

// void mood_get_frame(struct Mood_List *list,FILE *fp,uint8_t *buf)
// {
    
//     for(uint8_t i=0;i<list->frame.count[group_id];i++){
//         fseek(fp,list->frame.addr[group_id][i],SEEK_SET);
//         fread(buf,1,128*8,fp);
//     }
    
// }
    struct Mood_List *list1=NULL,*list2=NULL;

    if(trigger){//有事件触发
        list1=mood_locate_trigger(trigger);
        list2=mood_locate_ready();
        if(list1!=list2){

        }


        //mood_set_status(&list->property,RUN_BIT,READY);

    }else{//无事件触发
        list2=mood_locate_ready();
        if(list2==NULL){
            return 1;
        }




    }

    struct Mood_List *list=mood_locate_ready();
    if(list==NULL){
        return 1;
    }
    mood_increase(list);
    mood_set_status(&list->property,RUN_BIT,PEND);
    return 0;
}



struct Current_Frame{
    uint8_t group_id;
    uint8_t count;
    struct timeval last_t;
    struct timeval cur_t;
    double time; 

};

struct Current_Frame stFrame={0};




int main()
{
    gettimeofday(&stFrame.last_t, NULL);

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
    sleep(2);

    gettimeofday(&stFrame.cur_t, NULL);
    stFrame.time=stFrame.cur_t.tv_sec-stFrame.last_t.tv_sec;
    printf("代码执行时间: %f 秒\n",stFrame.time);


   //mood_frame_print_all();

    

    return 0;
}






