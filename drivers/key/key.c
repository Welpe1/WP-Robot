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
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/string.h>
#include <linux/irq.h>
#include <linux/of_irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/jiffies.h>


#define DEVICE_NAME     "key"

struct key_dev{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    struct device_node *nd;
    unsigned char value;    //引脚值
    int gpio;           //gpio编号
    int irq_num;        //中断号
    char name[10];      //中断名
    irqreturn_t (*irq_handler)(int,void*);//中断处理函数
    struct timer_list key_timer;


};

struct key_dev stKey_Dev={0};

static int key_open(struct inode *inode, struct file *file)
{
    file->private_data=&stKey_Dev;
    printk("key_open\r\n");
    return 0;
}

static int key_release(struct inode *inode, struct file *file)
{
    struct key_dev *dev=(struct key_dev *)file->private_data;
    printk("key_release(close)\r\n");
    return 0;
}

static irqreturn_t key_irq_handler(int irq,void *dev)
{
    struct key_dev *p_dev=dev;
    mod_timer(&p_dev->key_timer,msecs_to_jiffies(10));    //开启10ms定时
    return IRQ_HANDLED;

}

/**
 * 按键按下后触发key_irq_handler中断
 * key_irq_handler中断中开启10ms定时(消抖)
 * 10ms后进入timer_func
 */
static void timer_func(struct timer_list *timer)
{
    int value =0;
    struct key_dev *p_dev =from_timer(p_dev,timer,key_timer);
    value=gpio_get_value(p_dev->gpio);
    if(value==0){   //按键按下
        printk("push\r\n");
    }else{  //按键释放
        printk("release\r\n");
    }

}

static const struct file_operations stkey_fops={
    .owner=THIS_MODULE,
    .open=key_open,
    .release=key_release,
};


static int key_gpio_init(struct key_dev *dev)
{
    int ret=0;
    /* gpio初始化 */
    
    memset(dev->name,0,sizeof(dev->name));
    sprintf(dev->name,"key");
    dev->nd=of_find_node_by_path("/led_gpioA14");
    
    if(dev->nd==NULL){
        ret =- EINVAL;
        goto FAIL_ND;
    }

    dev->gpio=of_get_named_gpio(dev->nd,"led_gpio",0);
    gpio_request(dev->gpio,dev->name);
    gpio_direction_input(dev->gpio);

    printk("dev->gpio=%d\r\n", dev->gpio);

    /* 中断初始化 */
    dev->irq_num=gpio_to_irq(dev->gpio); //获取中断号
    printk("dev->irq_num1=%d\r\n", dev->irq_num);
    // dev->irq_num=irq_of_parse_and_map(dev->nd,0); //获取中断号
    // printk("dev->irq_num2=%d\r\n", dev->irq_num);

    ret=request_irq(dev->irq_num,key_irq_handler,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,dev->name,&stKey_Dev);
    if(ret){
        printk("irq request fail\r\n");
        goto FAIL_IRQ;

    }
    /* 定时器初始化 */
    timer_setup(&stKey_Dev.key_timer,timer_func, 0);//设置定时器
    add_timer(&stKey_Dev.key_timer);
    stKey_Dev.value=123;
    printk("key timer init.\n");
    return 0;


FAIL_IRQ:
    gpio_free(dev->gpio);
FAIL_ND:
    return ret;

}



static int __init mykey_init(void)
{
    int ret=0;
    const char *str;

    printk("key init\r\n");

    /* 分配设备号(设备号,次设备号,申请的设备数量,设备名)*/
    ret=alloc_chrdev_region(&stKey_Dev.devid,0,1,DEVICE_NAME);
    if(ret<0){
        printk("key region fail\r\n");
        goto FAIL_DEVID;
    }

    /* 注册字符设备 */
    stKey_Dev.cdev.owner=THIS_MODULE;
    cdev_init(&stKey_Dev.cdev,&stkey_fops);
    ret=cdev_add(&stKey_Dev.cdev,stKey_Dev.devid,1);
    if(ret<0){
        printk("key cdev fail\r\n");
        goto FAIL_CDEV;
    }

    /* 创建类 */
    stKey_Dev.class=class_create(THIS_MODULE,DEVICE_NAME);
    if(IS_ERR(stKey_Dev.class)){
        ret = PTR_ERR(stKey_Dev.class);
        goto FAIL_CLASS;
        
    }

    /* 创建设备 */
    stKey_Dev.device=device_create(stKey_Dev.class,NULL,stKey_Dev.devid,NULL,DEVICE_NAME);
    if(IS_ERR(stKey_Dev.device)){
        ret = PTR_ERR(stKey_Dev.device);
        goto FAIL_DEVICE;
        
    }

     /* key初始化 */

    ret=key_gpio_init(&stKey_Dev);
    if(ret<0){
        goto FAIL_GPIO_INIT;
    }


    return 0;

FAIL_GPIO_INIT:
    device_destroy(stKey_Dev.class,stKey_Dev.devid);
FAIL_DEVICE:
    class_destroy(stKey_Dev.class);
FAIL_CLASS:
    cdev_del(&stKey_Dev.cdev);
FAIL_CDEV:
    unregister_chrdev_region(stKey_Dev.devid,1);
FAIL_DEVID:
    return ret;
}

static void __exit mykey_exit(void)
{
    /* 释放中断 */
    free_irq(stKey_Dev.irq_num,&stKey_Dev);
    /* 释放IO */
    gpio_free(stKey_Dev.gpio);
    /* 释放定时器(同步) 在删除定时器时等待,直到定时器不再运行在任何处理器上 */
    del_timer_sync(&stKey_Dev.key_timer);
     /* 注销字符设备 */
    cdev_del(&stKey_Dev.cdev);
    /* 释放设备号 */
    unregister_chrdev_region(stKey_Dev.devid,1);
    /* 摧毁设备 */
    device_destroy(stKey_Dev.class,stKey_Dev.devid);
    /* 摧毁类 */
    class_destroy(stKey_Dev.class);



}


module_init(mykey_init);
module_exit(mykey_exit);
MODULE_LICENSE("GPL");
