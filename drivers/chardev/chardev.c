#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>


#define CHARDEV_NAME    "chardev"

static char readbuf[100];   /* 读缓冲 */
static char writebuf[100];  /* 写缓冲 */

struct CharDev{
    struct cdev cdev;       //字符设备
    struct class *class;     //类
    struct device *device;     //类
    dev_t devid;            //设备号

};

struct CharDev stCharDev={0};

static int chardev_open(struct inode *inode, struct file *file)
{
    printk("chardev_open\r\n");

    return 0;
}

static int chardev_release(struct inode *inode, struct file *file)
{
    printk("chardev_release(close)\r\n");
    
    return 0;
}

/**
 * 应用程序读取底层硬件数据(对应用户态的read)
 * 但应用程序不能直接访问内核数据,必须使用copy_to_user
 * @param buf 应用程序提供的buf
 * @param count 读取的字节数(有应用程序决定)
 * 
 */
static ssize_t chardev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    int ret=0;
    printk("chardev_read\r\n");
    memcpy(readbuf,"user read",sizeof("user read"));
    ret=copy_to_user(buf,readbuf,count);
    if(ret == 0){

    }else{

    }
    
    return 0;

}

/**
 * 应用程序写入底层硬件数据(对应用户态的write)
 * 但应用程序不能直接访问内核数据,必须使用copy_from_user
 * @param buf 应用程序提供的buf
 * @param count 写入的字节数(有应用程序决定)
 * 
 */
static ssize_t chardev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    int ret=0;
    printk("chardev_write\r\n");
    ret=copy_from_user(writebuf,buf,count);
    if(ret==0){
        printk("user write:%s\r\n",writebuf);
    }else{

    }
    
    return 0;

}


const struct file_operations stCharDev_fops={
    .owner=THIS_MODULE,
    .open=chardev_open,
    .release=chardev_release,
    .read=chardev_read,
    .write=chardev_write,
};

static int __init chardev_init(void)
{
    int ret=0;
    printk("chardev init\r\n");
    /* 分配设备号(自动) */
    ret=alloc_chrdev_region(&stCharDev.devid,0,1,CHARDEV_NAME);
    if(ret<0){
        printk("chardev region fail\r\n");
        goto FAIL_DEVID;
    }

    /* 注册字符设备 */
    stCharDev.cdev.owner=THIS_MODULE;
    cdev_init(&stCharDev.cdev,&stCharDev_fops);     //将stCharDev_fops赋值给stCharDev.cdev.ops
    ret=cdev_add(&stCharDev.cdev,stCharDev.devid,1);
    if(ret<0){
        printk("cdev add fail\r\n");
        goto FAIL_CDEV;
    }

    /* 创建类 (在/sys目录下创建类) */
    stCharDev.class = class_create(THIS_MODULE, CHARDEV_NAME);
    if(IS_ERR(stCharDev.class)){
        printk("class create fail\r\n");
        ret=PTR_ERR(stCharDev.class);
        goto FAIL_CLASS;
        
    }
   
     /* 创建设备 (在/dev目录下创建设备 /dev/chardev) */
    stCharDev.device = device_create(stCharDev.class,NULL,stCharDev.devid,NULL,CHARDEV_NAME);
    if(IS_ERR(stCharDev.device)){
        ret=PTR_ERR(stCharDev.device);
        goto FAIL_DEVICE;

    }

    return 0;

FAIL_DEVICE:
    class_destroy(stCharDev.class);
FAIL_CLASS:
    cdev_del(&stCharDev.cdev);
FAIL_CDEV:
    unregister_chrdev_region(stCharDev.devid,1);
FAIL_DEVID:
    return ret;



}

static void __exit chardev_exit(void)
{
    printk("chardev exit\r\n");
    /* 注销字符设备 */
    cdev_del(&stCharDev.cdev);

    /* 注销设备号 */
    unregister_chrdev_region(stCharDev.devid,1);

    /* 删除设备 (因为当初创建设备的时候用到了类) */
    device_destroy(stCharDev.class,stCharDev.devid);

    /* 删除类 */
    class_destroy(stCharDev.class);
}


/* 模块出入口 */
module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WP_Robot");


