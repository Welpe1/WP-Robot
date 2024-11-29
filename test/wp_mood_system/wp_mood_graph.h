#ifndef __WP_MOOD_GRAPH_H
#define __WP_MOOD_GRAPH_H

#include <stdint.h>

#define MAX_MOOD_NUMS   20      //最大顶点(情绪)个数


enum MOOD_TYPE{ 
    BASE=0,
    HAPPY,
    PEACE,
    SAD,
    FEAR,
    ANGRY,
    BASE1,
};

/* 邻接表节点 */
struct Mood_AdjNode {
    uint8_t dst;       //目标顶点索引
    struct Mood_AdjNode* next;
};

/* 邻接表 */
struct Mood_AdjList{
    int id;                         //顶点信息
    char name[10];                  //顶点信息
    struct Mood_AdjNode* head;         //邻接表头
};

/* 图 */
struct Mood_Graph{
    uint8_t adjlist_num;                            //邻接表数目
    struct Mood_AdjList adjlist[MAX_MOOD_NUMS];     //邻接表列表

};

struct Mood_Graph stMood_Graph={0};







#endif


