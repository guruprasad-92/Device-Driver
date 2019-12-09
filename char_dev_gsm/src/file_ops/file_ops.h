#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <linux/kernel.h>
#include <linux/module.h>

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

int scull_open(struct inode *inode, struct file *filp);

#endif