#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <linux/kernel.h>
#include <linux/module.h>

struct scull_dev {
    struct scull_qset *data; /* Pointer to first quantum set */
    int quantum;    /* the current quantum size */
    
    int qset;   /* the current array size */
    
    unsigned long size; /* amount of data stored here */
    
    unsigned int access_key; /* used by sculluid and scullpriv */
    
    struct semaphore sem;   /* mutual exclusion semaphore   */
    
    struct cdev cdev;   /* Char device structure*/
    
};

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

int scull_open(struct inode *inode, struct file *filp);

#endif