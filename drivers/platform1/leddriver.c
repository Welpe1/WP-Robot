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
#include <linux/ide.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>


/**
 * 无设备树
 */


#define LedDriver_NAME "leddriver"

struct LedDriver{
    struct cdev cdev;       //字符设备
    struct class *class;     //类
    struct device *device;     //类
    dev_t devid;            //设备号

};

struct LedDriver stLedDriver;


const struct file_operations stLedDriver_fops={
    .owner=THIS_MODULE,
};

/**
 * 当驱动和设备匹配成功后自动执行probe函数
 */
static int driver_probe(struct platform_device *dev)
{
    int ret=0;
    printk("driver_probe\r\n");

        /* 分配设备号(自动) */
    ret=alloc_chrdev_region(&stLedDriver.devid,0,1,LedDriver_NAME);
    if(ret<0){
        printk("LedDriver region fail\r\n");
        goto FAIL_DEVID;
    }

    /* 注册字符设备 */
    stLedDriver.cdev.owner=THIS_MODULE;
    cdev_init(&stLedDriver.cdev,&stLedDriver_fops);     //将stLedDriver_fops赋值给stLedDriver.cdev.ops
    ret=cdev_add(&stLedDriver.cdev,stLedDriver.devid,1);
    if(ret<0){
        printk("cdev add fail\r\n");
        goto FAIL_CDEV;
    }

    /* 创建类 (在/sys目录下创建类) */
    stLedDriver.class = class_create(THIS_MODULE, LedDriver_NAME);
    if(IS_ERR(stLedDriver.class)){
        printk("class create fail\r\n");
        ret=PTR_ERR(stLedDriver.class);
        goto FAIL_CLASS;
        
    }
   
     /* 创建设备 (在/dev目录下创建设备 /dev/LedDriver) */
    stLedDriver.device = device_create(stLedDriver.class,NULL,stLedDriver.devid,NULL,LedDriver_NAME);
    if(IS_ERR(stLedDriver.device)){
        ret=PTR_ERR(stLedDriver.device);
        goto FAIL_DEVICE;

    }

    return 0;

FAIL_DEVICE:
    class_destroy(stLedDriver.class);
FAIL_CLASS:
    cdev_del(&stLedDriver.cdev);
FAIL_CDEV:
    unregister_chrdev_region(stLedDriver.devid,1);
FAIL_DEVID:
    return ret;

}


/**
 * 当卸载驱动后自动执行remove函数
 */
static int driver_remove(struct platform_device *dev)
{
    printk("driver_remove\r\n");

     /* 注销字符设备 */
    cdev_del(&stLedDriver.cdev);

    /* 注销设备号 */
    unregister_chrdev_region(stLedDriver.devid,1);

    /* 删除设备 (因为当初创建设备的时候用到了类) */
    device_destroy(stLedDriver.class,stLedDriver.devid);

    /* 删除类 */
    class_destroy(stLedDriver.class);


    return 0;
}




static struct platform_driver led_driver={
    .driver={
        .name="111111111111111111111111111",    //驱动名,无设备树时用于和设备匹配

    },
    .probe=driver_probe,
    .remove=driver_remove,
};

static int __init platform_led_init(void)
{
    int ret=0;
     /* 注册platform驱动 */
    ret=platform_driver_register(&led_driver);


    return ret;

}


static void __exit platform_led_exit(void)
{
    /* 卸载platform驱动 */
    platform_driver_unregister(&led_driver);

}



module_init(platform_led_init);
module_exit(platform_led_exit);
MODULE_LICENSE("GPL");