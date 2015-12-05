#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include "simple_char.h"

static int major = 0;

static void simple_char_device_release(struct device* dev) {}

static struct simple_char_device simple_char_dev;

static struct class simple_char_class = {
	.name  = "simple_char"
  , .owner = THIS_MODULE
};

static int
simple_char_open(struct inode* inode, struct file* filp)
{
	struct simple_char_device*	spl_char_dev;
	spl_char_dev = container_of(inode->i_cdev, struct simple_char_device, c_dev);

	filp->private_data = spl_char_dev; 
	printk("open\n");
	return 0;
}

static ssize_t
simple_char_write(struct file* filp, const char* __user buf, size_t len, loff_t* off)
{
	printk("write\n");
	return len;
}

static ssize_t
simple_char_read(struct file* filp, char* __user buf, size_t len, loff_t* off)
{
	printk("read\n");
	return 0;
}

static int
simple_char_release(struct inode* inode, struct file* filp)
{
	printk("release\n");
	return 0;
}

static struct file_operations fops = {
	.owner    = THIS_MODULE
  , .open     = simple_char_open
  , .write    = simple_char_write
  , .read     = simple_char_read
  , .release  = simple_char_release
};

static int
simple_char_setup_chardev(struct simple_char_device* dev)
{
	int				ret   = 0;
	dev_t			devno = 0;

	ret = alloc_chrdev_region(&devno, 0, MINOR_NR, DEVICE_NAME);
	if(ret < 0) {
		printk("cannot allocate char device region.\n");
		goto err1;
	}
	major = MAJOR(devno);
	simple_char_dev.dev.devt = devno;
	cdev_init(&dev->c_dev, &fops);
	dev->c_dev.owner = THIS_MODULE;
	ret = cdev_add(&dev->c_dev, dev->dev.devt, 1);
	if(ret < 0) {
		printk("cannot add char device.\n");
		goto err2;
	}
	return 0;
err2 :
	unregister_chrdev_region(devno, MINOR_NR);
err1 :
	return ret;
}

static void
simple_char_clear_chardev(struct simple_char_device* dev)
{
	cdev_del(&dev->c_dev);
	unregister_chrdev_region(MKDEV(major, 0), MINOR_NR);
}

static int __init
simple_char_init(void)
{
	int				ret = 0;

	// キャラクタデバイスの登録
	ret = simple_char_setup_chardev(&simple_char_dev);
	if(ret < 0) {
		printk("cannot register charactor device.\n");
		goto err1;
	}
	// クラスの登録
	ret = class_register(&simple_char_class);
	if(ret < 0) {
		printk("cannot register class.\n");
		goto err2;
	}
	// クラスの登録
	simple_char_dev.dev.class   = &simple_char_class;
	simple_char_dev.dev.release = simple_char_device_release;
	// デバイス名
	dev_set_name(&(simple_char_dev.dev), "%s", DEVICE_NAME);
	ret = device_register(&(simple_char_dev.dev));
	if(ret < 0) {
		printk("cannot register device.\n");
		goto err3;
	}

	return ret;
err3 :
	class_unregister(&simple_char_class);
err2 :
	simple_char_clear_chardev(&simple_char_dev);
err1 :
	return ret;
}

static void __exit
simple_char_exit(void)
{
	device_unregister(&(simple_char_dev.dev));
	class_unregister(&simple_char_class);
	simple_char_clear_chardev(&simple_char_dev);
}

module_init(simple_char_init);
module_exit(simple_char_exit);
MODULE_LICENSE("GPL");
