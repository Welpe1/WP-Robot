#include "robot_utils.h"

uint8_t face_filter(cvtdl_face_t *stFaceMeta)
{
    uint64_t face_area=0,max_face_area=0;
    uint8_t max=0;
    for(uint8_t i=0;i<stFaceMeta->size;i++)
    {
        face_area=(stFaceMeta->info[i].bbox.x2-stFaceMeta->info[i].bbox.x1)*
        (stFaceMeta->info[i].bbox.y2-stFaceMeta->info[i].bbox.y1);
        if(max_face_area<face_area)
        {
            max_face_area=face_area;
            max=i;

        }
    }
    return max;
    
}


/**
 * @brief 检查一个64位数的某一位是否为1
 *        为1输出1,为0输出0
 * @param value 输入的值(最多64位)
 * @param position 要检查的具体位
 */
uint8_t read_bit(uint64_t value,uint8_t position)
{
    if((value >> position)& 1) return 1;
    else return 0;
}


/**
 * @brief 设置一个64位数的某一位
 * @param value 输入的值(最多64位)
 * @param position 要写的具体位
 * @param bit 写入的值0或1
 */
void set_bit(uint64_t *value,uint8_t position,uint8_t bit)
{
    if(bit){
        *value |= (1 << position);
    }else{
        *value &=~(1 << position);
    }
}


/**
 * @brief 检查一个64位数有多少个1
 * @param value 输入的值(最多64位)
 */
uint8_t count_bit(uint64_t value) 
{  
    uint8_t count = 0;  
    while (value) {  
        count += value & 1;  
        value >>= 1;  
    }  
    return count;  
} 

/**
 * @brief 随机输出一个在start~end-1的数
 * @param start 起始值
 * @param end 终止值
 */
uint16_t rand_section(uint16_t start,uint16_t end)
{

    return rand() % end+start;

}

/**
 * @brief 标志位处理
 * @param flag 标志位(linux_data)
 * linux_data
 * [0]:人体存在标志位
 * [1]:短触摸(happy)
 * [2]:长触摸(拍照)
 * [3]:晃动
 * ...
 * [9]
 * [10:20]facex
 * [21:31]facey
 * 这里仅处理linux_data的前9位
 */
void flag_process(uint16_t flag)
{
    for(uint8_t i=0;i<16;i++)
    {
        if(read_bit(flag,i))
        {
            switch(i)
            {
                case 0:
                {
                    gVideoCapture=0x01;//检测到人体，开启摄像头
                    break;
                }
            }
        }
        
    }


}