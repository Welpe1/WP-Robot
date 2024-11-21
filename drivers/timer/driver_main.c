#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <stdio.h>


#define OPEN_CMD    _IO('r',1)
#define CLOSE_CMD   _IO('r',2)
#define SET_PERIOD  _IOW('r',3,unsigned long)


int main(int argc,char **argv)
{
    int fd=0,ret=0,cmd=0;
    char *filename;
    unsigned long arg,period;


    if(argc<3){
        printf("usage err,need 3 params\r\n");
        printf("argv[0]:./driver_main\r\n");
        printf("argv[1]: /dev/xxx\r\n");
        printf("argv[0]:0:read 1:write\r\n");
        return 1;
    }

    filename=argv[1];
    cmd=atoi(argv[2]);
    period=(unsigned long)atoi(argv[3]);
    printf("cmd=%d\r\n",cmd);
    printf("period=%d\r\n",period);
  
	fd=open(filename,O_RDWR);
    if(fd<0){
        printf("open %s fail\r\n",filename);
        return 1;
    }

    if(cmd==1){
        ioctl(fd,OPEN_CMD,&arg);
    }else if(cmd==2){
       ioctl(fd,CLOSE_CMD,&arg);
    }else if(cmd==3){
        printf("11111111\r\n");
       ret=ioctl(fd,SET_PERIOD,&period);
       if(ret < 0)
        {
            printf("ioctl error!\n");
            close(fd);
        }
    }

    
    // if(r_or_w == 0){

    //      ret=read(fd,readbuf,50);
    //     if(ret<0){
    //         printf("read fail\r\n");
    //         return 1;
    //     }else{
    //         printf("read:%s\r\n",readbuf);
    //     }

    // }else{
    //     ret=write(fd,writebuf,50);
    //     if(ret<0){
    //         printf("write fail\r\n");
    //         return 1;
    //     }

    // }


    ret=close(fd);
    if(ret<0){
        printf("close fail\r\n");
        return 1;
    }
	
	
	return 0;


}