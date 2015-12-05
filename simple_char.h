#ifndef __SIMPLE_CHAR_H_INCLUDED__ /* { */
#define __SIMPLE_CHAR_H_INCLUDED__ /* }{ */

#include <linux/device.h>
#include <linux/cdev.h>

#define DEVICE_NAME		("simple_char")
#define MINOR_NR		(1)


struct simple_char_device
{
	struct cdev		c_dev;
	struct device	dev;
};

#endif /* } */ 
