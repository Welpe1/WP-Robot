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
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/ioctl.h>


/**
 * 使用内核定时器实现定时中断
 * 
 */

#define DEVICE_NAME     "timer"

static char readbuf[100];   /* 读缓冲 */
static char writebuf[100];  /* 写缓冲 */


uint32_t count=0;
uint32_t period=500;

struct timer_dev{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct timer_list timer;    //定时器
};
struct timer_dev stTimer_dev={0};

static int timer_open(struct inode *inode, struct file *file)
{
    file->private_data=&stTimer_dev;
    printk("timer_open\r\n");

    return 0;
}

static int timer_release(struct inode *inode, struct file *file)
{
    struct timer_dev *dev=(struct timer_dev *)file->private_data;
    printk("timer_release(close)\r\n");

    return 0;
}



static ssize_t timer_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    struct timer_dev *dev=(struct timer_dev *)file->private_data;
    int ret=0;
    printk("timer_write\r\n");
    ret=copy_from_user(writebuf,buf,count);
    if(ret==0){
        printk("user write:%s\r\n",writebuf);
    }else{

    }
    
    return 0;

}

#define OPEN_CMD    _IO('r',1)
#define CLOSE_CMD   _IO('r',2)
#define SET_PERIOD  _IOW('r',3,unsigned long)

static long timer_ioctl(struct file *file, unsigned int cmd,unsigned long arg)
{
    int ret=0;
    unsigned long value;
    struct timer_dev *dev=(struct timer_dev *)file->private_data;
    switch(cmd)
    {
        case OPEN_CMD:
        {
            printk("OPEN_CMD\r\n");
            mod_timer(&dev->timer,jiffies + msecs_to_jiffies(period));
            break;
        }
        case CLOSE_CMD:
        {
            printk("CLOSE_CMD\r\n");
            del_timer(&dev->timer);
            break;
        }
        case SET_PERIOD:
        {
            printk("SET_PERIOD\r\n");
            ret=copy_from_user(&value,(unsigned long *)arg,sizeof(unsigned long));
            printk("value=%d\r\n",value);
            if(ret<0){
                return 1;
            }
            period=value;
            
            mod_timer(&dev->timer,jiffies + msecs_to_jiffies(period));
            break;
        }

    }
    return 0;



}


/* 定时器处理函数 */
static void timer_function(struct timer_list *timer)
{
    count++;

    printk("timer function 500ms =%d\r\n",count);
    mod_timer(&stTimer_dev.timer,jiffies + msecs_to_jiffies(period));    //重新开启定时器
}

static const struct file_operations sttimer_fops={
    .owner=THIS_MODULE,
    .open=timer_open,
    .release=timer_release,
    .write=timer_write,
    .unlocked_ioctl=timer_ioctl,    //对应应用程序的ioctl

};



static int __init timer_init(void)
{
    int ret=0;
    const char *str;

    printk("timer init\r\n");
    ret=alloc_chrdev_region(&stTimer_dev.devid,0,1,DEVICE_NAME);
    if(ret<0){
        printk("timer region fail\r\n");
        goto FAIL_DEVID;
    }
    stTimer_dev.cdev.owner=THIS_MODULE;
    cdev_init(&stTimer_dev.cdev,&sttimer_fops);
    ret=cdev_add(&stTimer_dev.cdev,stTimer_dev.devid,1);
    if(ret<0){
        printk("timer cdev fail\r\n");
        goto FAIL_CDEV;
    }
    stTimer_dev.class=class_create(THIS_MODULE,DEVICE_NAME);
    if(IS_ERR(stTimer_dev.class)){
        ret = PTR_ERR(stTimer_dev.class);
        goto FAIL_CLASS;
        
    }
    stTimer_dev.device=device_create(stTimer_dev.class,NULL,stTimer_dev.devid,NULL,DEVICE_NAME);
    if(IS_ERR(stTimer_dev.device)){
        ret = PTR_ERR(stTimer_dev.device);
        goto FAIL_DEVICE;
        
    }

    /* timer初始化 */
    stTimer_dev.timer.function=timer_function;    //定时器处理函数
    stTimer_dev.timer.expires=jiffies + msecs_to_jiffies(period);  //期望值(jiffies表示当前时间,msecs_to_jiffies(500)表示500ms,即期望值为当前时间+500ms,可以理解为周期为500ms)
    /* 注册timer(添加到系统) */
    add_timer(&stTimer_dev.timer);

    return 0;


FAIL_DEVICE:
    class_destroy(stTimer_dev.class);
FAIL_CLASS:
    cdev_del(&stTimer_dev.cdev);
FAIL_CDEV:
    unregister_chrdev_region(stTimer_dev.devid,1);
FAIL_DEVID:
    return ret;
}

static void __exit timer_exit(void)
{
    del_timer(&stTimer_dev.timer);
    cdev_del(&stTimer_dev.cdev);
    unregister_chrdev_region(stTimer_dev.devid,1);
    device_destroy(stTimer_dev.class,stTimer_dev.devid);
    class_destroy(stTimer_dev.class);

}


module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");
