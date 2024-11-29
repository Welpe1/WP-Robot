#ifndef __WP_MOOD_SYS_H
#define __WP_MOOD_SYS_H

#include <stdint.h>

#define SET_TRIGGER(sw, id) ((id)<<1 | (sw) & 0x0001)

#define MAX_MOOD_NUMS   20      //最大情绪数量
#define READY      0
#define PEND       1
#define TRIGGER_ON      0
#define TRIGGER_OFF     1
#define POSITIVE        0
#define NORMAL          1
#define NEGATIVE        2

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
 * Mood_Node 邻接链表的节点(边)
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

struct Mood_Node{
    uint8_t dst;                //指向目标情绪id
    struct Mood_Node* next;     //指向下一个节点的指针
};


/**
 * @param name 情绪名(邻接链表名)
 * @param id   情绪ID(邻接链表ID)
 * @param status 情绪状态
 *               同一时刻只能有一种情绪的状态为READY,其它为PEND
 *               若此情绪开启了外界触发,则当有外界事件触发后,置为READY,处理完毕后置为PEND
 *               若未开启,则随机决定
 * @param value 情绪值,范围10~200
 *              由value值决定最终显示哪一个表情
 * @param group 情绪组别
 *              将情绪分为POSITIVE NORMAL NEGATIVE两组
 * @param trigger 触发状态,
 *                [0]表示是否开启外界触发,开启表示此情绪必须由外界事件触发
 *                [15:1]表示触发id
 * 
 * 
 */
struct Mood_Property{
    char name[10];          
    uint8_t id;             
    uint8_t status;         
    uint8_t value;
    uint8_t group;          
    uint16_t trigger;          
};

struct Mood_List{
    struct Mood_Property property;              
    struct Mood_Node* head;         //横向,指向当前邻接链表表头的指针
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


