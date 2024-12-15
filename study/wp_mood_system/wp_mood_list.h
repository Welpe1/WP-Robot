#ifndef __WP_MOOD_LIST_H
#define __WP_MOOD_LIST_H

#include <stdint.h>

struct Mood_List{
    char name[10];
    uint8_t id;
    struct Mood_List *next;
};

struct Mood_List stMood_List={
    .name="BASE",
    .id=0,
    .next=NULL,
};


enum MOOD_TYPE{ 
    BASE=0,
    HAPPY,
    PEACE,
    SAD,
    FEAR,
    ANGRY,
};


#endif


