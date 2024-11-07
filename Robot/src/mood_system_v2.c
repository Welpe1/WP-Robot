#include "robot_utils.h"
#include "mood_system.h"
#include "oledfont.h"


#define __DEBUG__
#ifdef __DEBUG__
#define debug_printf printf
#else
#define debug_printf(...)
#endif


int16_t mood_pr_end[MAX_MOOD_TYPES]={0,299,799,899,949,999};

void happy_increase(struct Mood_Info *stMood_Info)
{
    /**
     * happy+=2
     * sad-=2
     * peace,fear,angry不变
     */
    uint16_t sad = stMood_Info->pr_pointer[SAD];  
    uint16_t fear = stMood_Info->pr_pointer[FEAR];  
    uint16_t peace = stMood_Info->pr_pointer[PEACE];  
    if (sad < fear - THRESHOLD_DIFF) stMood_Info->pr_pointer[SAD] += 2;  
    if (peace < sad - THRESHOLD_DIFF) stMood_Info->pr_pointer[PEACE] += 2;  
    
}

void peace_increase(struct Mood_Info *stMood_Info)
{
    /**
     * peace+=2
     * fear-=1 angry-=1
     * happy,sad不变
     */
    uint16_t sad = stMood_Info->pr_pointer[SAD];  
    uint16_t fear = stMood_Info->pr_pointer[FEAR];  
    uint16_t angry = stMood_Info->pr_pointer[ANGRY]; 
    if(angry < MAX_MOOD_TYPES - THRESHOLD_DIFF) stMood_Info->pr_pointer[ANGRY]+=1;
    if(fear < angry - THRESHOLD_DIFF) stMood_Info->pr_pointer[FEAR]+=2;
    if(sad < fear - THRESHOLD_DIFF) stMood_Info->pr_pointer[SAD]+=2;
}

void sad_increase(struct Mood_Info *stMood_Info)
{
    /**
     * happy-=2
     * sad+=2
     * peace,fear,angry不变
     */

    uint16_t peace = stMood_Info->pr_pointer[PEACE];  
    uint16_t sad = stMood_Info->pr_pointer[SAD];  
    if(peace > 0 + THRESHOLD_DIFF)  stMood_Info->pr_pointer[PEACE]-=2;
    if(sad > peace + THRESHOLD_DIFF) stMood_Info->pr_pointer[SAD]-=2; 
}

void fear_increase(struct Mood_Info *stMood_Info)
{
    /**
     * peace-=2
     * fear+=2
     * happy,sad,angry不变
     * 
     */
    uint16_t peace = stMood_Info->pr_pointer[PEACE];  
    uint16_t sad = stMood_Info->pr_pointer[SAD];  
    uint16_t fear = stMood_Info->pr_pointer[FEAR];  
    if(sad > peace + THRESHOLD_DIFF) stMood_Info->pr_pointer[SAD]-=2; 
    if(fear > sad + THRESHOLD_DIFF) stMood_Info->pr_pointer[FEAR]-=2;

}

void angry_increase(struct Mood_Info *stMood_Info)
{
    /**
     * peace-=1
     * happy-=1
     * angry+=2
     * sad,fear不变
     */
    uint16_t peace = stMood_Info->pr_pointer[PEACE]; 
    uint16_t sad = stMood_Info->pr_pointer[SAD];  
    uint16_t fear = stMood_Info->pr_pointer[FEAR];  
    uint16_t angry = stMood_Info->pr_pointer[ANGRY]; 
    if(peace > 0 + THRESHOLD_DIFF)  stMood_Info->pr_pointer[PEACE]-=1;
    if(sad > peace + THRESHOLD_DIFF) stMood_Info->pr_pointer[SAD]-=2; 
    if(fear > sad + THRESHOLD_DIFF) stMood_Info->pr_pointer[FEAR]-=2;
    if(angry > fear + THRESHOLD_DIFF) stMood_Info->pr_pointer[ANGRY]-=2;

}

void mood_increase(struct Mood_Info *stMood_Info,uint8_t mood)
{
    switch(mood)
    {
        case HAPPY:
        {
            happy_increase(stMood_Info);
            break;
        }
        case PEACE:
        {
            peace_increase(stMood_Info);
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


/**
 * 根据上一时刻的状态与当前随机数决定下一时刻的状态
 * 
 * */
void mood_rand_convert(struct Mood_Info *stMood_Info)
{


    uint16_t random = rand() % 1000;
    uint16_t p_num=stMood_Info->pr_pointer[SAD]-stMood_Info->pr_pointer[HAPPY];       //积极情绪数 happy+peace
    uint16_t n_num=stMood_Info->pr_pointer[SPECIAL]-stMood_Info->pr_pointer[SAD];         //消极情绪数 sad+fear+angry
    uint16_t n_random=p_num+rand()%n_num;
    uint8_t mood_temp=0;

    switch (stMood_Info->last_state)
    {
        case HAPPY:
        {
            /**
             * 上一状态为happy时
             * 下一状态只能是peace happy
             * 转换为peace的概率为 peace/(happy+peace)
             * 
             */
            int p_random= rand()%p_num;
            if(p_random > stMood_Info->pr_pointer[PEACE]) stMood_Info->state=PEACE;
            break;
        };
        case PEACE:
        {
            /**
             * 上一状态为peace时
             * 下一状态可以任意转换
             * 按各情绪占比转换
             * 
             */

            for(mood_temp=0;mood_temp<MAX_MOOD_TYPES-1;mood_temp++)//0~4
            {
                if(random >= stMood_Info->pr_pointer[mood_temp] && random <=stMood_Info->pr_pointer[mood_temp+1])
                break;
            }
            stMood_Info->state=mood_temp;
            break;
        };
        case SAD:
        {
            /**
             * 上一状态为sad时
             * 下一状态是sad angry fear peace
             * 转换为peace的概率为  1/3
             * 转换为sad的概率为    2/3 * (sad/(sad+angry+fear))
             * 转换为fear的概率为   2/3 * (fear/(sad+angry+fear))
             * 转换为angry的概率为  2/3 * (angry/(sad+angry+fear))
             * 
             */

            if(n_random % 3 >= 1)
            {
                for(mood_temp=2;mood_temp<MAX_MOOD_TYPES-1;mood_temp++)     //2 3 4 
                {
                    if(n_random >= stMood_Info->pr_pointer[mood_temp] && n_random <=stMood_Info->pr_pointer[mood_temp+1])
                    break;
                }
                stMood_Info->state=mood_temp;

            }else{
                stMood_Info->state = PEACE;
            }
        

            break;
        };
        case FEAR:
        {
            /**
             * 上一状态为fear时
             * 下一状态是sad peace fear
             * 转换为peace的概率为  angry/(sad+angry+fear)
             * 转换为sad的概率为    sad/(sad+angry+fear)
             * 转换为fear的概率为   fear/(sad+angry+fear)
             * 
             */


            for(mood_temp=2;mood_temp<MAX_MOOD_TYPES-1;mood_temp++)
            {
                if(n_random >= stMood_Info->pr_pointer[mood_temp] && n_random <=stMood_Info->pr_pointer[mood_temp+1])
                break;
            }
            stMood_Info->state=mood_temp;
            if(mood_temp == ANGRY) stMood_Info->state = PEACE;

            break;
        }
        case ANGRY:
        {
            /**
             * 上一状态为angry时
             * 下一状态是sad peace angry
             * 转换为peace的概率为  fear/(sad+angry+fear)
             * 转换为sad的概率为    sad/(sad+angry+fear)
             * 转换为angry的概率为  angry/(sad+angry+fear)
             * 
             */

            for(mood_temp=2;mood_temp<MAX_MOOD_TYPES-1;mood_temp++)
            {
                if(n_random >= stMood_Info->pr_pointer[mood_temp] && n_random <=stMood_Info->pr_pointer[mood_temp+1])
                break;
            }
            stMood_Info->state=mood_temp;
            if(mood_temp == FEAR) stMood_Info->state = PEACE;
            
            break;
        };
        case SPECIAL:
        {
            stMood_Info->state = PEACE;
            break;
        };
        
    }


}

void mood_pr_limit(struct Mood_Info *stMood_Info)
{
    uint16_t p_num=stMood_Info->pr_pointer[SAD]-stMood_Info->pr_pointer[HAPPY];       //积极情绪数 happy+peace
    uint16_t n_num=stMood_Info->pr_pointer[SPECIAL]-stMood_Info->pr_pointer[SAD];         //消极情绪数 sad+fear+angry
    stMood_Info->pr_pointer[SAD]=LIMIT(stMood_Info->pr_pointer[SAD],600,800);
    p_num=stMood_Info->pr_pointer[SAD]-stMood_Info->pr_pointer[HAPPY];       //积极情绪数 happy+peace
    n_num=stMood_Info->pr_pointer[SPECIAL]-stMood_Info->pr_pointer[SAD];         //消极情绪数 sad+fear+angry
    stMood_Info->pr_pointer[PEACE]=LIMIT(stMood_Info->pr_pointer[PEACE],0.3*p_num,0.6*p_num);
    stMood_Info->pr_pointer[ANGRY]=LIMIT(stMood_Info->pr_pointer[ANGRY],MAX_PR_NUM-0.25*n_num,MAX_PR_NUM-0.1*n_num);
    stMood_Info->pr_pointer[FEAR]=LIMIT(stMood_Info->pr_pointer[FEAR],stMood_Info->pr_pointer[ANGRY]-0.25*n_num,stMood_Info->pr_pointer[ANGRY]-0.1*n_num);

}

/**
 * 此函数决定下一时刻oled显示的表情
 * 根据上一时刻的状态与当前随机数决定下一时刻的状态
 * 如果有事件触发则事件优先级最高，处理完毕后事件触发清0
 * 当事件触发后，情绪会改变(mood++)
 */
void mood_update(struct Mood_Info *stMood_Info)
{
    //记录上一时刻的状态
    stMood_Info->last_state=stMood_Info->state;   
    stMood_Info->stEMO_Info.last_state=stMood_Info->stEMO_Info.state;

    if(stMood_Info->stEMO_Info.last_state == STATIC || stMood_Info->event_trigger){
        if(stMood_Info->event_trigger){//有事件触发
			 debug_printf("有事件触发\r\n");
            stMood_Info->state = SPECIAL;
            for(uint8_t i=0;i<64;i++){
                if(read_bit(stMood_Info->event_trigger,i)){
                    if(i<48){
                        //对应mood++
						 debug_printf("触发的事件是=%d\r\n",i);
                        mood_increase(stMood_Info,(uint8_t)(i/8));
                        
                    }
                    stMood_Info->stEMO_Info.select_pointer=emo_static_group[i/8][i%8];
                }
            }
            mood_pr_limit(stMood_Info);
            for(int i=0;i<MAX_MOOD_TYPES-1;i++){
                stMood_Info->pr[i]=(float)(stMood_Info->pr_pointer[i+1]-stMood_Info->pr_pointer[i])/(MAX_PR_NUM+1);
            }
            stMood_Info->event_trigger=0;
        }else{//上一帧是静态表情
            debug_printf("上一时刻是静态且无事件触发\r\n");
            mood_rand_convert(stMood_Info); 
            if(rand() % 3 < 1){   //静态
            //随机决定下一帧是静态表情
                 debug_printf("随机决定下一帧是静态表情\r\n");

                 switch(stMood_Info->state)
    {

        case HAPPY:
        {
		     debug_printf("当前状态是happy\r\n");
            break;
        };
        case PEACE:
        {
             debug_printf("当前状态是peace\r\n");
            break;
        };
        case SAD:
        {
             debug_printf("当前状态是sad\r\n");
            break;
        };
        case FEAR:
        {
             debug_printf("当前状态是fear\r\n");
            break;
        }
        case ANGRY:
        {
             debug_printf("当前状态是angry\r\n");
            break;
        };
        case SPECIAL:
        {
             debug_printf("当前状态是special\r\n");
            break;
        };
        
    }


    
                stMood_Info->stEMO_Info.state = STATIC;
                stMood_Info->stEMO_Info.static_select = rand() % EMO_STATIC_MAX;    //0~7
                stMood_Info->stEMO_Info.select_pointer=emo_static_group[stMood_Info->state][stMood_Info->stEMO_Info.static_select]; 

            }else{  //动态
            //随机决定下一帧是动态表情
                 debug_printf("随机决定下一帧是动态表情\r\n");
                stMood_Info->stEMO_Info.state = DYNAMICS;
                stMood_Info->stEMO_Info.dynamics_select = rand() % EMO_DYNAMICS_MAX;    //0~3
                stMood_Info->stEMO_Info.dynamics_count=emo_dynamics_group[stMood_Info->state][stMood_Info->stEMO_Info.dynamics_select][0];
            }
      
        }
    }else if(stMood_Info->stEMO_Info.last_state == DYNAMICS){  //上一帧是动态
         debug_printf("剩余动态帧数为%d\r\n",stMood_Info->stEMO_Info.dynamics_count);
        stMood_Info->stEMO_Info.dynamics_count--;
        stMood_Info->stEMO_Info.select_pointer=emo_dynamics_group[stMood_Info->state][stMood_Info->stEMO_Info.dynamics_select][emo_dynamics_group[stMood_Info->state][stMood_Info->stEMO_Info.dynamics_select][0]-stMood_Info->stEMO_Info.dynamics_count];
        if(stMood_Info->stEMO_Info.dynamics_count == 0)
        {
            stMood_Info->stEMO_Info.state = STATIC;
        }
    }

     debug_printf("帧指针为%d\r\n",stMood_Info->stEMO_Info.select_pointer);


    
     debug_printf("\r\n");

}

void mood_system_init(struct Mood_Info *stMood_Info)
{

    for(int i=0;i<MAX_MOOD_TYPES;i++)   //0~6  
	{

        /**
         * 0        1       2       3       4       5
         * 0        299     799     899     949     999
         * happy    peace   sad     fear    angry   special
         * 
         * stMood_Info->pr_pointer[HAPPY]永远等于0
         * stMood_Info->pr_pointer[SPECIAL]永远等于999
         */

        stMood_Info->pr_pointer[i]= mood_pr_end[i];
        if(i > 0) stMood_Info->pr[i-1]=(float)(stMood_Info->pr_pointer[i]-stMood_Info->pr_pointer[i-1])/(MAX_PR_NUM+1);
	}
    stMood_Info->state = PEACE;
    stMood_Info->last_state = PEACE;
    stMood_Info->stEMO_Info.state = STATIC;
    stMood_Info->stEMO_Info.last_state = STATIC;

}










