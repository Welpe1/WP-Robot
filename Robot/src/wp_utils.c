#include "wp_utils.h"


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
 * @brief 检查给定数的某一位是否为1
 *        为1输出1,为0输出0
 * @param value 输入的值(最多64位)
 * @param position 要检查的具体位
 */
uint8_t wp_read_bit(uint64_t value,uint8_t position)
{
    if((value >> position)& 1) return 1;
    else return 0;
}


/**
 * @brief 设置给定数的某一位
 * @param value 输入的值(最多64位)
 * @param position 要写的具体位
 * @param bit 写入的值0或1
 */
void wp_set_bit(uint64_t *value,uint8_t position,uint8_t bit)
{
    if(bit){
        *value |= (1 << position);
    }else{
        *value &=~(1 << position);
    }
}


/**
 * @brief 检查给定数有多少位是1
 * @param value 输入的值(最多64位)
 */
uint8_t wp_count_bit(uint64_t value) 
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



// double cal_time_elapsed(struct timeval start, struct timeval end) {
//   double sec = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.;
//   return sec;
// }


void print_binary(uint16_t n) 
{
    unsigned int mask = 1 << (sizeof(uint16_t) * 8 - 1);
    while (mask) {
        putchar((n & mask) ? '1' : '0');
        mask >>= 1;
    }
    putchar('\n');
}


/**
 * @brief 在文件中寻找指定的数据,并将找到的数据的位置返回至buf中
 *        1.一次性读取BLOCK_SIZE字节数据(注意两数据的间隔必须大于BLOCK_SIZE)
 *        2.如果有目标字符串,则返回目标字符串相对于文件头的位置,存在buf中
 *        3.buf第0位一开始置为-1
 *        4.没有则将文件指针前移-target位置,以防目标字符串跨越两个读取块
 *        默认传入参数正确
 * @param fp
 * @param target
 * @param buf
 * 
 * 
 * 
 */
void file_search_data(FILE *fp,char *target,long int *buf) 
{
    #define BLOCK_SIZE  256

    uint8_t target_len=strlen(target);
    char r_buf[BLOCK_SIZE+1];
    memset(r_buf,0,sizeof(r_buf));
    fseek(fp, 0, SEEK_SET);
    long int count = 0;
    buf[count]=-1;

    while(1){
        size_t r_len=fread(r_buf,1,BLOCK_SIZE,fp);
        r_buf[r_len]='\0';
        if(feof(fp)) break;
        char *strx = strstr(r_buf,target);
        if(strx!=NULL){
            long int pos=ftell(fp)-r_len+(strx-r_buf);
            buf[count++] = pos;  
        }else{
            fseek(fp,-target_len,SEEK_CUR); //调整文件指针位置,以防目标字符串跨越两个读取块
        }
    }
}

/**
 * @brief 从字符串中提取第一个整数
 */
int extract_first_int(const char *str)
{
    int num = 0; // 存储提取的数字
    int sign = 1; // 用于处理可能的负号
    int start = -1; // 记录数字开始的索引位置
    int i = 0;
    while(str[i]!='\0') {
        if(str[i]=='-'&&start==-1){
            sign = -1;
            i++;
            continue;
        }
        if(isdigit(str[i])){
            if(start==-1) start = i;
            num=num*10+(str[i]-'0'); 
        }else{
            if(start!=-1) break; 
        }
        i++;
    }
    return sign*num;
}


double cal_time_elapsed(struct timeval start, struct timeval end) {
  double sec = end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.;
  return sec;
}

