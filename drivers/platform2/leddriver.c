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
 * 有设备树
 * 不再需要向总线注册设备
 * 只需直接修改设备树,编写对应驱动即可
 */



/**
 * 当驱动和设备匹配成功后自动执行probe函数
 */
static int driver_probe(struct platform_device *dev)
{
    int ret=0;
    printk("driver_probe\r\n");
    struct device_node *nd;

    /* 获取设备节点 */
    nd=dev->dev.of_node;

    memset(dev->name,0,sizeof(dev->name));
    sprintf(dev->name,"key");
    

    int gpio=of_get_named_gpio(nd,"led_gpio",0);
    /* 申请一个GPIO管脚(管脚号 给对应的管脚起名) */
    gpio_request(gpio,"led");
    gpio_direction_output(gpio,1);

    printk("gpio=%d\r\n",gpio);

    return ret;

}


/**
 * 当卸载驱动后自动执行remove函数
 */
static int driver_remove(struct platform_device *dev)
{
    printk("driver_remove\r\n");

    return 0;
}


/* 设备树匹配表 */
const struct of_device_id led_of_match[]={
    {.compatible="led-A14"},
    {/* sentinel */},

};


static struct platform_driver led_driver={
    .driver={
        .name="111111111111111111leddriver",    //驱动名
        .of_match_table=led_of_match,       //设备树匹配表,有设备树时使用of_match_table与设备匹配

    },
    .probe=driver_probe,
    .remove=driver_remove,
};

static int __init platform_led_init(void)
{
     /* 注册platform驱动 */
    return platform_driver_register(&led_driver);

}


static void __exit platform_led_exit(void)
{
    /* 卸载platform驱动 */
    platform_driver_unregister(&led_driver);

}



module_init(platform_led_init);
module_exit(platform_led_exit);
MODULE_LICENSE("GPL");