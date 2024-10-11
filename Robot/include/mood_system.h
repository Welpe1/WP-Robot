#ifndef __MOOD_SYSTEM_H
#define __MOOD_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>



#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define LIMIT_MAX(x, max) ((x) > (max) ? (max) : (x))
#define LIMIT_MIN(x, min) ((x) < (min) ? (min) : (x))


#define MAX_MOOD_TYPES	6
#define MAX_PR_NUM      999
#define THRESHOLD_DIFF 2
#define STATIC  0
#define DYNAMICS    1

#define HAPPY   0
#define PEACE   1
#define SAD     2
#define FEAR    3
#define ANGRY   4
#define SPECIAL 5



char *mood_types[MAX_MOOD_TYPES] = {   
    "happy",
	"peace", 
	"sad",
	"fear", 
    "angry"
};



int16_t mood_pr_end[MAX_MOOD_TYPES]={299,799,899,949,999};

struct Mood_Operation{
    void (*mood_increase[MAX_MOOD_TYPES])(struct Mood_Info *); 
    void (*mood_update)(struct Mood_Operation *,struct Mood_Info *); 
    void (*mood_rand_convert)(struct Mood_Info *); 
    void (*mood_pr_limit)(struct Mood_Info* );
};

struct EMO_Info{
    uint8_t state;               //0:静态表情 1:动态表情
    uint8_t last_state;
    uint8_t static_select;       //静态表情组选择器(0~9)
    uint8_t dynamics_count;     //动态表情计数器
    uint8_t dynamics_select;     //动态表情组选择器(0~4)
    uint8_t select_pointer;     //表情选择器

};

struct Mood_Info{
    
    char name[MAX_MOOD_TYPES][8];
    uint16_t pr_pointer[MAX_MOOD_TYPES];   //对应每个情绪的开头 special不参与其中，special永远指向999
	float pr[MAX_MOOD_TYPES-1];
    uint8_t state;
    uint8_t last_state;
    uint8_t event_trigger;     //一共5位,[0]对应happy++,[1]对应peace++,...
    struct EMO_Info stEMO_Info;
};




void happy_increase(struct Mood_Info *stMood_Info);
void peace_increase(struct Mood_Info *stMood_Info);
void sad_increase(struct Mood_Info *stMood_Info);
void fear_increase(struct Mood_Info *stMood_Info);
void angry_increase(struct Mood_Info *stMood_Info);
void mood_system_init(struct Mood_Operation *stMood_Operation,struct Mood_Info *stMood_Info);
void mood_update(struct Mood_Operation *stMood_Operation,struct Mood_Info *stMood_Info);
void mood_rand_convert(struct Mood_Info *stMood_Info);
void mood_pr_limit(struct Mood_Info *stMood_Info);

 

#endif
