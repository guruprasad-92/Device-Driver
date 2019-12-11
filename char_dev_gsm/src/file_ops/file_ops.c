#include "file_ops.h"

#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


#include "../dev_dt_struct.h"
#include "../pwr_hndl/pwr_hndl.h"

#define DFLT_WR "Wrote in scull_open"

static char SG_buff_rd[Kbuff_rd_sz] = {0};
static char SG_buff_wr[Kbuff_rd_sz] = {0};
//static uint8_t SG_mdm_sts = MDM_ABSNT;
static int SG_sts_op;

static struct GS_mdm gs_mdm;

int mdm_init(void)
{
    memset(gs_mdm.mk,0,20);
    sprintf(gs_mdm.mk,"quectel");
    gs_mdm.typ = 4;
    if ( qktl4g_pwr_cycle() == 0 )
    {
        gs_mdm.sts = 1;
        return 0;
    }
    else
    {
        gs_mdm.sts = 0;
        return -1;
    }
    return 0;
}

int scull_trim(struct scull_dev *dev)
{
    struct scull_qset *next, *dptr;
    int qset = dev->qset;    /* "dev" is not-null */
    int i;
    for (dptr = dev->data; dptr; dptr = next) /* all the list items */
    {
        if (dptr->data)
        {
            for (i = 0; i < qset; i++)
                kfree(dptr->data[i]);
            kfree(dptr->data);
            dptr->data = NULL;
        }
        next = dptr->next;
        kfree(dptr);
    }
    dev->size = 0;
    dev->data = NULL;
    return 0;
}

int scull_open(struct inode *inode, struct file *filp)
{


    struct scull_dev *dev; // device information 
    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev; // for other methods 

    // now trim to 0 the length of the device if open was write-only 
    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) 
    {
        scull_trim(dev); // ignore errors 
    }

    
    if(!SG_sts_op)
    {
        SG_sts_op = 1;
        return 0;
    }
    else
    {
        printk("GSM: Device has already opened. Close it first.\n");
        return -EBUSY;
    }
    
    
    /* success */
}

int scull_release(struct inode *inode, struct file *filp)
{
    printk(KERN_DEBUG "GSM: In scull_release()\n");
    SG_sts_op = 0;
    return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{    
    struct scull_dev *dev = filp->private_data;
    int ret_val=0;
    int sz = 0;

    sprintf(SG_buff_rd,"%s,%dg,%d",gs_mdm.mk,\
                                gs_mdm.typ,\
                                gs_mdm.sts);
    sz = strlen(SG_buff_rd);
    dev->size = sz;
    
    // We don't need the linked list at this stage.
    // We need all the data from 0th position in each read, from kernel.
    /*
        struct scull_quest *dptr; // First list
        int quantum, qset;
        int item_sz; // No of bytes in the listitem
        int item, s_pos, q_pos, rest;
        ssize_t retval = 0;
    */

    /*
        Semaphore use is simple in Linux. To attempt to acquire a semaphore, 
        call the down_interruptible() function. This function decrements the 
        usage count of the semaphore. If the new value is less than zero, 
        the calling process is added to the wait queue and blocked. 
        If the new value is zero or greater, the process obtains the semaphore and 
        the call returns 0. If a signal is received while blocking, 
        the call returns -EINTR and the semaphore is not acquired.
    */
    if(down_interruptible(&dev->sem))
    {
        return -ERESTARTSYS;
    }
    
    if( *f_pos >= dev->size )
    {
        printk(KERN_ERR "GSM: from scull_read(), dev->size = %ld *f_pos = %lld\n",dev->size,*f_pos);
        ret_val = -1;
        goto ret;
    }
    else
    {
        printk(KERN_INFO "GSM: From scull_read(), *f_pos = %lld, dev->sz = %ld\n",*f_pos,dev->size);
    }    
    if(count > sz)
    {   
        count = sz;
        printk(KERN_ERR "GSM: Error max count = %d\n",sz);
    }
    else if (count < 0)
    {
        count = 0;
    }

    if (copy_to_user(buf,SG_buff_rd,count))
    {
        //Error case
        ret_val = -1;
        printk(KERN_ERR "GSM: Error in copy_to_user().\n");
        goto ret;
    }
    printk(KERN_DEBUG "GSM: The data has been copied.\n");
    ret_val = count;
    ret : ;
    return ret_val;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t rt_val = 0;
    printk(KERN_INFO "GSM: In scull_write()\n");
    if(count > Kbuff_rd_sz)
    {
        printk(KERN_ERR"GSM: scull_write(): count exceeds the limit (%d)\n",Kbuff_rd_sz);
        rt_val = -ENOSPC;
    }
    else
    {
        printk(KERN_INFO "GSM: In scull_write()\n");
        memset(SG_buff_wr,0,strlen(SG_buff_wr));
        if ( copy_from_user(SG_buff_wr,buf,count) != 0)
        {
            printk(KERN_CRIT "GSM: copy_from_user() failed.\n");
            return -1;
        }

        printk(KERN_CRIT "GSM: From user sps = %s\n",SG_buff_wr);

        if(strstr(SG_buff_wr,"mdm"))
        {
            printk(KERN_DEBUG "GSM: Problem");
            memset(gs_mdm.mk,0,strlen(gs_mdm.mk));
            if( !strcmp(SG_buff_wr,"mdm=qecktel,2g") )
            {
                printk(KERN_ERR "GSM: Unsupported device Quectel-2g\n");            
                sprintf(gs_mdm.mk,"Quecktel");
                gs_mdm.typ = 2;
                rt_val = -1;
            }
            else if( !strcmp(SG_buff_wr,"mdm=quectel,3g") )
            {
                printk(KERN_ERR "GSM: Unsupported device Quectel-3g\n");
                sprintf(gs_mdm.mk,"Quecktel");
                gs_mdm.typ = 3;
                rt_val = -1;
            }
            else if( !strcmp(SG_buff_wr,"mdm=quectel,4g") )
            {
                sprintf(gs_mdm.mk,"Quecktel");
                gs_mdm.typ = 4;
                rt_val = 0xA;
            }
            else
            {
                printk(KERN_ERR "GSM: Invalid data(mdm) from user space.\n");
                rt_val = -1;
            }
            
        }               
        else if( !strcmp(SG_buff_wr,"0") && gs_mdm.sts )
        {
            gs_mdm.sts = 0;
            printk(KERN_INFO "GSM: MDM-%s.%d turned off",gs_mdm.mk,gs_mdm.typ);
            rt_val = 0;
        }
        else if( !strcmp(SG_buff_wr,"1") && ( !gs_mdm.sts ) )
        {
            gs_mdm.sts = 1;
            printk(KERN_INFO "GSM: MDM-%s.%d turned on",gs_mdm.mk,gs_mdm.typ);
            rt_val = 1;
        }
        else if( !strcmp(SG_buff_wr,"reboot") )
        {
            printk(KERN_DEBUG "GSM: rebooting the modem... .. .\n");
            gsm_pwr_off();
            if (qktl4g_pwr_cycle() == 0)
            {
                printk(KERN_DEBUG "GSM: MDM %s rebooted.\n",gs_mdm.mk);
                rt_val = 0xa1;
            }    
            else
            {
                printk(KERN_DEBUG "GSM: Error in MDM %s while rebooting. Err in power cycle.\n",gs_mdm.mk);
                gs_mdm.sts = 0;
                rt_val = -1;
            }
        }
        else
        {
            printk(KERN_CRIT "GSM: Invalid data(sts) from user space.\n");
            rt_val = -1;
        }
        
    }
    
    return rt_val;
}
