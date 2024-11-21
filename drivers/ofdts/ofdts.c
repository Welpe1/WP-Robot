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


/**
 * 使用of函数读取设备树的内容
 * 
 */

static int __init dtsof_init(void)
{
    int ret=0;
    struct device_node *i2c1_nd=NULL;
    struct property *property=NULL;
    const char *str;
    uint32_t value;

    /* 通过路径找到i2c1节点 */
    i2c1_nd=of_find_node_by_path("/i2c@04010000");
    if(i2c1_nd==NULL){
        ret=1;
        goto FAIL_FINDND;
    }

    /* 获取属性 status clock-frequency*/
    ret=of_property_read_string(i2c1_nd,"status",&str);
    if(ret){
        goto FAIL_READSTRING;
    }
    printk("status=%s\r\n",str);


    ret=of_property_read_u32(i2c1_nd,"clock-frequency",&value);
    if(ret){
        goto FAIL_READU32;
    }
    printk("clock-frequency=%d\r\n",value);

    return ret;

FAIL_READU32:
FAIL_READSTRING:
FAIL_FINDND:
    return ret;


}

static void __exit dtsof_exit(void)
{

}

module_init(dtsof_init);
module_exit(dtsof_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WP-Robot");