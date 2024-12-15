#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

void mood_set_bit(uint64_t *value,uint8_t position,uint8_t status)
{
    if(status){
        *value |= (1 << position);
    }else{
        *value &= ~(1 << position);
    }

}



int extractFirstInt(const char *str)
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
    return sign * num;
}

 
// 主函数
int main() {
    int a=0;
    a=extractFirstInt("abc+123a4");
    //mood_set_bit((uint64_t*)&a,2,1);
    printf("%d\r\n",a);
    return 0;
}
 




/**
 * 一次性读取block_size字节数据
 * 如果有目标字符串,则返回目标字符串相对于文件头的位置
 * 没有则将文件指针前移-target位置,以防目标字符串跨越两个读取块
 * 
 * 
 */
// long int file_seek(FILE *fp,char *target,uint8_t target_len) {
//     if (!fp||!target||target_len== 0) return 1;

//     const uint16_t block_size=256;
//     char r_buf[block_size+1];
//     memset(r_buf,0,sizeof(r_buf));
//     fseek(fp, 0, SEEK_SET);

//     while (1) {
//         size_t r_len=fread(r_buf,1,block_size,fp);
//         r_buf[r_len]='\0';

//         printf("r_len=%d\r\n",r_len);
//         printf("r_buf=%s\r\n",r_buf);
//         printf("\r\n");

//         char *strx = strstr(r_buf,target);
//         if(strx!=NULL){
//             long int pos=ftell(fp)-r_len+(strx-r_buf);
//             fseek(fp,pos,SEEK_SET);
//             printf("strx=%s\r\n",strx);
//             printf("pos=%ld\r\n",pos);
//             return pos; 
//         }
//         if(feof(fp)) break;
//         fseek(fp,-target_len,SEEK_CUR); //调整文件指针位置,以防目标字符串跨越两个读取块
//     }
//     return -1;
// }




// /** 
//  * @brief 在文件中寻找指定的数据,并将找到的数据的位置返回至buf中
//  *        1.一次性读取BLOCK_SIZE字节数据(注意两数据的间隔必须大于BLOCK_SIZE)
//  *        2.如果有目标字符串,则返回目标字符串相对于文件头的位置,存在buf中
//  *        3.没有则将文件指针前移-target位置,以防目标字符串跨越两个读取块
//  *        4.最后将buf最后一位置为-1
//  *          
//  * @param fp
//  * @param target
//  * @param buf
//  * 
//  * 
//  * 
//  */
// void file_search_data(FILE *fp,char *target,long int *buf) 
// {
//     #define BLOCK_SIZE  256

//     uint8_t target_len=strlen(target);
//     char r_buf[BLOCK_SIZE+1];
//     memset(r_buf,0,sizeof(r_buf));
//     fseek(fp, 0, SEEK_SET);
//     long int count = 0;

//     while(1){
//         if(feof(fp)) break;
//         size_t r_len=fread(r_buf,1,BLOCK_SIZE,fp);
//         r_buf[r_len]='\0';
//         char *strx = strstr(r_buf,target);
//         if(strx!=NULL){
//             long int pos=ftell(fp)-r_len+(strx-r_buf);
//             buf[count++] = pos;  
//         }else{
//             fseek(fp,-target_len,SEEK_CUR); //调整文件指针位置,以防目标字符串跨越两个读取块
//         }
//     }
//     buf[count]=-1;
// }

// int main()
// {
//     const char *file="/home/welpe/Desktop/test.txt";
//     FILE *fp=NULL;
//     fp=fopen(file,"r");
//     if(fp==NULL){
//         printf("open file %s fail\r\n",file);
//         return 1;
//     }
//     long int buf[5];
//     file_search_data(fp,"hello",buf);
//     for(uint8_t i=0;i<5;i++){
//         printf("buf[%d]=%d\r\n",i,buf[i]);
//     }
 




//     return 0;


// }

