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

/**
 * 无设备树
 */

void device_release(struct device*dev)
{
    printk("device_release\r\n");
}


static struct platform_device led_device={
    .name="111111111111111111111111111",    //设备名
    .id=-1,     //表示该设备无id
    .dev={
        .release=device_release,
    },

};




static int __init platform_led_init(void)
{
    int ret=0;
    /* 注册platform设备 */
    ret=platform_device_register(&led_device);
    return ret;

}


static void __exit platform_led_exit(void)
{
    /* 卸载platform设备 */
    platform_device_unregister(&led_device);

}



module_init(platform_led_init);
module_exit(platform_led_exit);
MODULE_LICENSE("GPL");




