#ifndef __WP_MOOD_SYS_H
#define __WP_MOOD_SYS_H

#include <stdint.h>
#include "wp_mood_sys_config.h"


#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define LIMIT_MAX(x, max) ((x) > (max) ? (max) : (x))
#define LIMIT_MIN(x, min) ((x) < (min) ? (min) : (x))


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
    char name[MAX_NAME_LEN];          
    uint8_t id;             
    uint8_t status;               
    uint8_t trigger; 
    float value;          
};

/**
 * @param group 组别(最多8组,每组最多10帧)
 *              每一位代表每一组的状态,0:关闭 1:启用
 *              如bit[0]=1代表第0组启用
 * @param count 帧总数
 *              每一位代表每一组的帧总数
 * @param addr 地址
 *             每一位代表每组所有帧的地址
 *             如addr[0][0]代表第0组第0帧的地址
 */
struct Frame_Info{
    uint8_t group;
    uint8_t count[MAX_MOOD_GROUP];
    uint16_t addr[MAX_MOOD_GROUP][MAX_FRAME_NUM];
};

struct Mood_List{        
    struct Mood_BindNode* head;         //横向,指向当前邻接链表表头的指针
    struct Mood_List* next;         //纵向,指向下一个链表的指针,即指向下一个情绪 
    struct Frame_Info frame;  
    struct Mood_Property property;  
};

struct Mood_List stMood_List={
    .property={
        .name="BASE",
        .id=0,
        .status=0x00,
        .value=BASE_MOOD_VALUE,
        .trigger=0,
    },
    .next=NULL,
    .head=NULL,
};



#endif


