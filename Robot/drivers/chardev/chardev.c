#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>

#define CHARDEV_MAJOR   200
#define CHARDEV_NAME    "chardev"


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

static ssize_t chardev_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    printk("chardev_read\r\n");
    
    return 0;

}

static ssize_t chardev_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    printk("chardev_write\r\n");
    
    return 0;

}


const struct file_operations chardev_fops={
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
    /* 注册字符设备 (/dev/chardev) */
    ret=register_chrdev(CHARDEV_MAJOR,CHARDEV_NAME,&chardev_fops);
    if(ret<0){
        printk("chardev init fail\r\n");
    }

    return 0;
}

static void __exit chardev_exit(void)
{
    printk("chardev exit\r\n");
    /* 注销字符设备 */
    unregister_chrdev(CHARDEV_MAJOR,CHARDEV_NAME);
   
}


/* 模块入口 */
module_init(chardev_init);
/* 模块出口 */
module_exit(chardev_exit);
/* 许可证 */
MODULE_LICENSE("GPL");
/* 作者 */
MODULE_AUTHOR("WP_Robot");
