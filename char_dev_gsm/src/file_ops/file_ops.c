#include "file_ops.h"
#include <linux/fs.h>


ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

int scull_open(struct inode *inode, struct file *filp)
{
    return 0;
}
