#ifndef FILE_OPS_H
#define FILE_OPS_H

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);


#endif