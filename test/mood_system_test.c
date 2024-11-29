#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define MOOD_TYPES	6  
#define DURATION_MAX   100
#define DURATION_MIN    1

#define HAPPY   0
#define SAD     1
#define FEAR    2
#define ANGRY   3
#define SPECIAL 4
#define NORMAL  5


#define LIMIT( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define LIMIT_MAX(x, max) ((x) > (max) ? (max) : (x))
#define LIMIT_MIN(x, min) ((x) < (min) ? (min) : (x))



const uint8_t emo_group[6][4][9]={
    {//HAPPY
        {11,22,33,0,0,0,0,0,3},    //这里的每个数字代表帧在表情数据组的序号
        {11,22,33,0,0,0,0,0,3},
        {11,22,33,0,0,0,0,0,3},       //若元素一样，则为静态表情
        {11,22,33,0,0,0,0,0,3},
    },
    {//SAD
        {10,20,30,40,0,0,0,0,4},   
        {10,20,30,40,0,0,0,0,4},
        {10,20,30,40,0,0,0,0,4},
        {10,20,30,40,0,0,0,0,4},
    },

    {//ANGRY
        {19,29,39,49,59,0,0,0,5},   
        {19,29,39,49,59,0,0,0,5},
        {19,29,39,49,59,0,0,0,5},
        {19,29,39,49,59,0,0,0,5},
    },

    {//FEAR
        {11,22,33,44,55,66,0,0,6},   
        {11,22,33,44,55,66,0,0,6},
        {11,22,33,44,55,66,0,0,6},
        {11,22,33,44,55,66,0,0,6},
    },
    {//SPECIAL
        {11,22,33,44,0,0,0,0,4},   
        {11,22,33,44,0,0,0,0,4},
        {11,22,33,44,0,0,0,0,4},
        {11,22,33,44,0,0,0,0,4},
    },
     {//NORMAL
        {11,22,33,44,55,0,0,0,5},   
        {11,22,33,44,55,0,0,0,5},
        {11,22,33,44,55,66,77,0,7},
        {11,22,33,44,55,66,77,0,7},
    },

};


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



uint8_t read_bit(uint8_t value,uint8_t position)
{
    return value & (0x01 << position);

}


uint8_t count_bit(uint16_t data) 
{  
    uint8_t count = 0;  
    while (data) {  
        count += data & 1;  
        data >>= 1;  
    }  
    return count;  
} 

/**
 * 随机输出一个在start~end-1的数
 */
uint16_t rand_section(uint16_t start,uint16_t end)
{

    return rand() % end+start;

}


#define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif


void happy_increase(struct Mood_Info *stMood_Info)
{
    /**
     * happy    +=2
     * sad      -=1
     * fear     -=1
     * angry    -=1
     */
    stMood_Info->duration[HAPPY] +=2;
    stMood_Info->duration[SAD]   -=1;
    stMood_Info->duration[FEAR]  -=1;
    stMood_Info->duration[ANGRY] -=1;
    
}


void sad_increase(struct Mood_Info *stMood_Info)
{
    /**
     * happy    -=2
     * sad      +=2
     * fear     +=1
     * angry    +=1
     */

    stMood_Info->duration[HAPPY] -=2;
    stMood_Info->duration[SAD]   +=2;
    stMood_Info->duration[FEAR]  +=1;
    stMood_Info->duration[ANGRY] +=1;
}

void fear_increase(struct Mood_Info *stMood_Info)
{
    /**
     * happy    -=2
     * sad      +=1
     * fear     +=2
     * angry    +=1
     * 
     */
    stMood_Info->duration[HAPPY] -=2;
    stMood_Info->duration[SAD]   +=1;
    stMood_Info->duration[FEAR]  +=2;
    stMood_Info->duration[ANGRY] +=1;

}

void angry_increase(struct Mood_Info *stMood_Info)
{
    /**
     * happy    -=2
     * sad      +=1
     * fear     +=1
     * angry    +=2
     * 
     */
    stMood_Info->duration[HAPPY] -=2;
    stMood_Info->duration[SAD]   +=1;
    stMood_Info->duration[FEAR]  +=1;
    stMood_Info->duration[ANGRY] +=2;

}


void mood_pr_limit(struct Mood_Info *stMood_Info)
{
    stMood_Info->duration[HAPPY]=LIMIT(stMood_Info->duration[HAPPY],DURATION_MIN,DURATION_MAX);
    stMood_Info->duration[SAD]=LIMIT(stMood_Info->duration[SAD],DURATION_MIN,DURATION_MAX);
    stMood_Info->duration[FEAR]=LIMIT(stMood_Info->duration[FEAR],DURATION_MIN,DURATION_MAX);
    stMood_Info->duration[ANGRY]=LIMIT(stMood_Info->duration[ANGRY],DURATION_MIN,DURATION_MAX);

}


void mood_increase(struct Mood_Info *stMood_Info)
{
    switch(stMood_Info->state)
    {
        case HAPPY:
        {
            happy_increase(stMood_Info);
            break;
        }
        case SAD:
        {
            sad_increase(stMood_Info);
            break;
        }
        case FEAR:
        {
            fear_increase(stMood_Info);
            break;
        }
        case ANGRY:
        {
            angry_increase(stMood_Info);
            break;
        }
    }

}

void helper(uint8_t mood)
{
    switch(mood)
    {

         case HAPPY:
            {
                debug_printf("当前状态是HAPPY\r\n");
                break;
            }
            case SAD:
            {
                debug_printf("当前状态是SAD\r\n");
                break;
            }
            case FEAR:
            {
                debug_printf("当前状态是FEAR\r\n");
                break;
            }
            case ANGRY:
            {
                debug_printf("当前状态是ANGRY\r\n");
                break;
            }
            case SPECIAL:
            {
                debug_printf("当前状态是SPECIAL\r\n");
                break;
            }
            case NORMAL:
            {
                debug_printf("当前状态是NORMAL\r\n");
                break;
            }

    }
}


void mood_system_update(struct Mood_Info *stMood_Info)
{
    
    if(stMood_Info->event_trigger){
        debug_printf("有事件触发\r\n");
        for(uint8_t i=0;i<10;i++){//检查标志位
            if(read_bit(stMood_Info->event_trigger,i)){
                if(i<4){
                    stMood_Info->state=i;
                    //mood_increase(stMood_Info);
                    
                }else{
                    stMood_Info->state=SPECIAL;
                }
            }
        }
        mood_pr_limit(stMood_Info);
        stMood_Info->event_trigger=0;

    
    }else{
        if(stMood_Info->stEMO_Info.loop_count==0)
        {
            debug_printf("无事件触发\r\n");
            stMood_Info->state=NORMAL;
        }

    }
    
    if(stMood_Info->stEMO_Info.loop_count){

        debug_printf("剩余循环次数为%d\r\n",stMood_Info->stEMO_Info.loop_count);
        debug_printf("帧组[%d]剩余%d帧\r\n",stMood_Info->stEMO_Info.frame_group,stMood_Info->stEMO_Info.frame_count);
      

        if(stMood_Info->stEMO_Info.frame_count){
            //stMood_Info->stEMO_Info.frame_select=emo_group[stMood_Info->state][stMood_Info->stEMO_Info.frame_group][stMood_Info->stEMO_Info.frame_count];
            stMood_Info->stEMO_Info.frame_count--;
        }else{
            stMood_Info->stEMO_Info.frame_group=rand_section(0,4);  //随机选择帧组
            //debug_printf("帧组为%d\r\n",stMood_Info->stEMO_Info.frame_group);
            //debug_printf("重装载帧组帧数为%d\r\n",emo_group[stMood_Info->state][stMood_Info->stEMO_Info.frame_group][8]);
            stMood_Info->stEMO_Info.frame_count=emo_group[stMood_Info->state][stMood_Info->stEMO_Info.frame_group][8]-1;
            stMood_Info->stEMO_Info.loop_count--;
        }
        

    }else{
        debug_printf("循环次数清零\r\n");
        stMood_Info->stEMO_Info.loop_count=stMood_Info->duration[stMood_Info->state];
    }
    

    helper(stMood_Info->state);

    debug_printf("stMood_Info->state=%d\r\n",stMood_Info->state);
    debug_printf("stMood_Info->stEMO_Info.frame_group=%d\r\n",stMood_Info->stEMO_Info.frame_group);
    debug_printf("stMood_Info->stEMO_Info.frame_count=%d\r\n",stMood_Info->stEMO_Info.frame_count);

    stMood_Info->stEMO_Info.frame_select=emo_group[stMood_Info->state][stMood_Info->stEMO_Info.frame_group][stMood_Info->stEMO_Info.frame_count];
    
}


void mood_system_init(struct Mood_Info *stMood_Info)
{
    debug_printf("初始化\r\n");
    stMood_Info->state=NORMAL;
    stMood_Info->stEMO_Info.frame_select=0;
    stMood_Info->stEMO_Info.frame_group=0;
    for(uint8_t i=0;i<5;i++)
    {
        stMood_Info->duration[i]=i+2;
        debug_printf("stMood_Info->duration[%d]=%d\r\n",i,stMood_Info->duration[i]);
        
    }
    stMood_Info->stEMO_Info.loop_count=3;
    debug_printf("                   \r\n");

}

int main()
{
    int x;
    struct Mood_Info stMood_Info;
    mood_system_init(&stMood_Info);
    while(1)
    {
        scanf("%d",&x);
        printf("x=%d\r\n",x);
        switch(x)
        {
            case 0:
            {
                stMood_Info.event_trigger=0x00;
                break;
            }
            case 1:
            {
                stMood_Info.event_trigger=0x01;
                break;
            }
            case 2:
            {
                stMood_Info.event_trigger=0x02;
                break;
            }
            case 3:
            {
                stMood_Info.event_trigger=0x04;
                break;
            }
            case 4:
            {
                stMood_Info.event_trigger=0x08;
                break;
            }
            case 5:
            {
                stMood_Info.event_trigger=0x10;
                break;
            }
        }
        mood_system_update(&stMood_Info);
        debug_printf("帧选择器为%d\r\n",stMood_Info.stEMO_Info.frame_select);
        debug_printf("                   \r\n");

    }



    return 0;


}
