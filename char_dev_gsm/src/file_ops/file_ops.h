#ifndef FILE_OPS_H
#define FILE_OPS_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kdev_t.h>

#include "../dev_dt_struct.h"

#define Kbuff_rd_sz 150
#define Kbuff_wr_sz 50

#define MDM_ABSNT   0xA1
#define MDM_ERR     0xA2
#define MDM_BSY     0xA3

#define MDM_OK      0

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
int mdm_init(void);

#endif