#ifndef __MOOD_SYSTEM_H
#define __MOOD_SYSTEM_H

#include <stdint.h>


#define MOOD_TYPES	6  
#define DURATION_MAX   100
#define DURATION_MIN    30

#define HAPPY   0
#define SAD     1
#define FEAR    2
#define ANGRY   3
#define SPECIAL 4
#define NORMAL  5

#define EMO_BASE_TYPES      6      //基类数
#define EMO_FRAME_GROUPS    4      //帧组数




struct EMO_Info{
    uint8_t loop_count;         //显示循环次数(这里暂时令loop_count=duration)
    uint8_t frame_count;        //帧计数器
    uint8_t frame_group;       //帧组选择器(0~3)
    uint8_t frame_select;      //帧选择器
};


struct Mood_Info{ 
    uint8_t duration[5];        //显示持续时长(0~100)
    uint8_t state;              //当前情绪状态
    uint8_t event_trigger;      //事件触发  
    struct EMO_Info stEMO_Info;
};



#endif


