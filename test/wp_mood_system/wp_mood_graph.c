#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "wp_mood_graph.h"


/**
 * @brief 根据邻接表名称匹配邻接表,返回邻接表索引
 * @param name 邻接表名
 * 
 */
int locate_adjlist(char *name)
{
    for(uint8_t i=0;i<stMood_Graph.adjlist_num;i++){
        if(strcmp(stMood_Graph.adjlist[i].name,name)==0){
            return i;
        }
    }
    return -1;
}

/**
 * @brief 添加邻接表,默认加在最后一个
 * @param name 邻接表名称
 * 
 */
uint8_t mood_adjlist_add(char *name)
{
    struct Mood_AdjList *list;
    list= &stMood_Graph.adjlist[stMood_Graph.adjlist_num];
    strncpy(list->name,name,sizeof(list->name));
    list->head=NULL;
    list->id=stMood_Graph.adjlist_num;
    stMood_Graph.adjlist_num++;
    return 0;
} 



/**
 * @brief 添加一个新的邻接表节点到图中,只能添加一次
 * @param src 源邻接表索引
 * @param dst 目标邻接表索引
 * 
 */
uint8_t mood_adjnode_add(uint8_t src,uint8_t dst)
{
    if (src >= stMood_Graph.adjlist_num || dst >= stMood_Graph.adjlist_num) {
        printf("out of max adjlist num\r\n");
        return 2; 
    }

    /* 检查是否已经被添加过,单方向检查*/
    struct Mood_AdjList *list=&stMood_Graph.adjlist[src];
    struct Mood_AdjNode *node1=list->head,*node2=NULL;
    while(node1 !=NULL){
        if(node1->dst!=dst){
            node2=node1;
            node1=node1->next;
        }else{
            printf("node already added\r\n");
            return 3;
        }
    }

    node1=(struct Mood_AdjNode*)malloc(sizeof(struct Mood_AdjNode));
    if(node1 == NULL){
        printf("alloc fail\r\n");
        return 1;
    }

    node2=(struct Mood_AdjNode*)malloc(sizeof(struct Mood_AdjNode));
    if(node2 == NULL){
        printf("alloc fail\r\n");
        free(node1);
        return 1;
    }

    node1->dst=src;
    node1->next=stMood_Graph.adjlist[dst].head;
    stMood_Graph.adjlist[dst].head=node1;

    node2->dst=dst;
    node2->next=stMood_Graph.adjlist[src].head;
    stMood_Graph.adjlist[src].head=node2;

    return 0;
}   


/**
 * @brief 删除邻接表节点
 * @param src 源邻接表索引
 * @param dst 目标邻接表索引
 * 
 */
uint8_t mood_adjnode_delete(uint8_t src,uint8_t dst)
{


//     if(src>=stMood_Graph.adjlist_num || dst>=stMood_Graph.adjlist_num){
//         printf("out of max adjlist num\r\n");
//         return 2; 
//     }
//     struct Mood_AdjList *list=&stMood_Graph.adjlist[src];
//     struct Mood_AdjNode *node1=list->head,*node2=NULL;
//     while(node1 !=NULL && node1->dst!=dst){
//         node2=node1;
//         node1=node1->next;
//     }
//     if(node1 !=NULL && node1->dst==dst){
//         if(node2 == NULL){
//             list->head=node1->next;
//         }else{
//             node2->next=node1->next;
//         }
//         free(node1);
//     }
//     list=&stMood_Graph.adjlist[dst];
//     node1=list->head;
//     node2=NULL;
//     while(node1 !=NULL && node1->dst!=src){
//         node2=node1;
//         node1=node1->next;
//     }
//     if(node1 !=NULL && node1->dst==src){
//         if(node2 == NULL){
//             list->head=node1->next;
//         }else{
//             node2->next=node1->next;
//         }
//         free(node1);
//     }
//     return 0;



    if(src>=stMood_Graph.adjlist_num || dst>=stMood_Graph.adjlist_num){
        printf("out of max adjlist num\r\n");
        return 2; 
    }

    struct Mood_AdjList *list=&stMood_Graph.adjlist[src];
    struct Mood_AdjNode *node1=list->head,*node2=NULL;
    while(node1 !=NULL){
        if(node1->dst!=dst){
            node2=node1;
            node1=node1->next;
        }else{
            if(node2 == NULL){
                list->head=node1->next;
            }else{
                node2->next=node1->next;
            }
            free(node1);
            break;
        }
    }

    list=&stMood_Graph.adjlist[dst];
    node1=list->head;
    node2=NULL;
    while(node1 !=NULL){
        if(node1->dst!=src){
            node2=node1;
            node1=node1->next;
        }else{
            if(node2 == NULL){
                list->head=node1->next;
            }else{
                node2->next=node1->next;
            }
            free(node1);
            break;
        }
    }
    return 0;

}


/**
 * @brief 删除邻接表,包括该表链接的各个节点
 * @param name 邻接表名称
 * 
 */
uint8_t mood_adjlist_delete(char *name)
{
    int list_id=locate_adjlist(name);
    if(list_id<0){
        printf("no list named %s\r\n",name);
        return 1;
    }

    struct Mood_AdjList *list=&stMood_Graph.adjlist[list_id];
    struct Mood_AdjNode* node1=list->head,*node2=NULL;
    while(node1!=NULL){
        //printf("->[%d]%s\n",node1->dst,stMood_Graph.adjlist[node1->dst].name);
        node2=node1;
        node1=node1->next;
        mood_adjnode_delete(list_id,node2->dst);
    }

    stMood_Graph.adjlist_num--;
    for(uint8_t i=0;i<stMood_Graph.adjlist_num;i++){
        if(i>=list_id)
        {
            stMood_Graph.adjlist[i]=stMood_Graph.adjlist[i+1];
            stMood_Graph.adjlist[i].id--;
        }
        list=&stMood_Graph.adjlist[i];
        node1=list->head;
        while(node1!=NULL){
            if(node1->dst>=list_id) node1->dst--;
            node1=node1->next;
        }
    }
    return 0;
} 




void printGraph(void) 
{
    printf("-------------------start print the graph----------------------\r\n");
    for (int i = 0; i < stMood_Graph.adjlist_num; i++) {
        struct Mood_AdjList *list=&stMood_Graph.adjlist[i];
        printf("Vertex[%d]=%s:\n", list->id,list->name);
        struct Mood_AdjNode* node = list->head;
        while (node != NULL) {
            printf("->[%d]%s\n",node->dst,stMood_Graph.adjlist[node->dst].name);
            node = node->next;
        }
    }
    printf("-------------------end print the graph----------------------\r\n");
    printf("\r\n");
}

uint8_t mood_graph_init()
{

    /* 添加邻接表 */
    mood_adjlist_add("BASE");
    mood_adjlist_add("HAPPY");
    mood_adjlist_add("PEACE");
    mood_adjlist_add("SAD");
    mood_adjlist_add("FEAR");
    mood_adjlist_add("ANGRY");

    /* 初始化边 */
    mood_adjnode_add(BASE,HAPPY);
    mood_adjnode_add(HAPPY,PEACE);  
    mood_adjnode_add(PEACE,ANGRY);
    mood_adjnode_add(PEACE,FEAR);
    mood_adjnode_add(PEACE,SAD);
    mood_adjnode_add(SAD,ANGRY);
    mood_adjnode_add(SAD,FEAR);

    printGraph();

    mood_adjlist_delete("BASE");

    printGraph();


    // mood_adjlist_add("BASE");
    // mood_adjlist_add("BASE1");
    // mood_adjnode_add(BASE,HAPPY);
    // mood_adjnode_add(BASE,PEACE);
    // mood_adjnode_add(BASE1,HAPPY);
    // mood_adjnode_add(BASE1,PEACE);
    // printGraph();


    printf("\r\n");


    return 0;
}





int main()
{

    mood_graph_init();
    return 0;
}