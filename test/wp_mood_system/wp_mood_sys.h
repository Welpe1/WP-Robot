#ifndef __WP_MOOD_SYS_H
#define __WP_MOOD_SYS_H

#include <stdint.h>

#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define LIMIT_MAX(x, max) ((x) > (max) ? (max) : (x))
#define LIMIT_MIN(x, min) ((x) < (min) ? (min) : (x))

#define MAX_MOOD_NUMS   20      //最大情绪数量
#define VALUE_MIN   10
#define VALUE_BASE  100
#define VALUE_MAX   200
#define VALUE_INCREASE  3

/*status*/
#define RUN_BIT     0
#define TRIGGER_BIT 1
#define READY      0
#define PEND       1
#define TRIGGER_ON      0
#define TRIGGER_OFF     1


enum MOOD_TYPES{ 
    BASE=0,
    HAPPY,
    PEACE,
    SAD,
    FEAR,
    ANGRY,
};


/**
 * 参考图结构实现
 * 全局变量 gMood_Num 记录情绪的总数
 * Mood_BindNode 邻接链表的节点(边)
 *           用于描述两个情绪之间的关系
 *          
 * Mood_List 情绪链表(邻接链表)
 *           是一个情绪的具体表现
 *           next 指向下一个情绪
 *           head 当前情绪的头指针
 * 
 * 全局变量 Mood_List stMood_List
 * 
 * 
 */

static uint8_t gMood_Num=1;


/**
 * @param dst 指向目标情绪id
 * @param rel(relevance) 与目标情绪的关联性
 *        取值范围-1~1
 *        -1~<0:负相关,如happy和sad
 *        0:不相关
 *        0~1:正相关,如sad和fear
 * @param next 指向下一个节点的指针
 * 
 * 
 * 
 */
struct Mood_BindNode{
    uint8_t dst;                
    float rel;
    struct Mood_BindNode* next;     
};


/**
 * @param name 情绪名(邻接链表名)
 * @param id   情绪ID(邻接链表ID)
 * @param status 运行状态,触发状态...
 *               [0]:运行状态
 *               同一时刻只能有一种情绪的状态为READY,其它为PEND
 *               若此情绪开启了外界触发,则当有外界事件触发后,置为READY,处理完毕后置为PEND
 *               若未开启,则随机决定
 *               [1]:触发状态
 *               TRIGGER_ON     表示开启触发,即此情绪需要外界触发才能操作 
 *               TRIGGER_OFF    表示关闭触发
 *               ...
 * @param value 情绪值,范围VALUE_MIN~VALUE_MAX
 *              由value值决定最终显示哪一个表情
 * @param trigger 触发id,手动分配
 *                必须在触发状态置为TRIGGER_ON生效
 * 
 * 
 * 
 */
struct Mood_Property{
    char name[10];          
    uint8_t id;             
    uint8_t status;               
    uint8_t trigger; 
    float value;          
};

struct Mood_List{
    struct Mood_Property property;              
    struct Mood_BindNode* head;         //横向,指向当前邻接链表表头的指针
    struct Mood_List* next;         //纵向,指向下一个链表的指针,即指向下一个情绪 
};

struct Mood_List stMood_List={
    .property={
        .name="BASE",
        .id=0,
        .status=PEND,
        .value=100,
        .trigger=0,
    },
    .next=NULL,
    .head=NULL,
};



#endif


