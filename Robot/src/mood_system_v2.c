/**
 * mood_control_system v2
 * 正常情况 normal(包含peace与用户自定义表情)
 * 事件触发 happy sad fear angry special,对应mood++
 * 取消概率跳转
 * 概率负责情绪显示时长
 * 
 */

#include "robot_utils.h"
#include "mood_system_v2.h"
#include "oledfont_v2.h"


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