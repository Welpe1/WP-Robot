#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char **argv)
{
    int fd=0,ret=0,r_or_w;
    char *filename;
    char readbuf[100];
    char *writebuf;
    writebuf="write to kernel 11111";

    if(argc<3){
        printf("usage err,need 3 params\r\n");
        printf("argv[0]:./driver_main\r\n");
        printf("argv[1]: /dev/xxx\r\n");
        printf("argv[0]:0:read 1:write\r\n");
        return 1;
    }

    filename=argv[1];
    r_or_w=atoi(argv[2]);
  
	fd=open(filename,O_RDWR);
    if(fd<0){
        printf("open %s fail\r\n",filename);
        return 1;
    }
    
    if(r_or_w == 0){

         ret=read(fd,readbuf,50);
        if(ret<0){
            printf("read fail\r\n");
            return 1;
        }else{
            printf("read:%s\r\n",readbuf);
        }

    }else{
        ret=write(fd,writebuf,50);
        if(ret<0){
            printf("write fail\r\n");
            return 1;
        }

    }


    ret=close(fd);
    if(ret<0){
        printf("close fail\r\n");
        return 1;
    }
	
	
	return 0;


}