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

