#include "file_ops.h"

#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <linux/delay.h>


#include "../dev_dt_struct.h"
#include "../pwr_hndl/pwr_hndl.h"

#define DFLT_WR "Wrote in scull_open"

static char SG_buff_rd[Kbuff_rd_sz] = {0};
static char SG_buff_wr[Kbuff_rd_sz] = {0};
//static uint8_t SG_mdm_sts = MDM_ABSNT;
//static int SG_sts_op;

static struct GS_mdm gs_mdm;
struct semaphore EGS_sem;



/*
static int str_tok_hld;
char* str_tok(char *buf, const char *tok)
{
    char *tmp;
    uint8_t sz = strlen(buf);
    
}

uint8_t count_tok(const char *buf, char tok)
{
    uint8_t rt = 0;
    uint8_t sz = strlen(buf);
    int i =0;
    for(i=0;i<sz;i++)
    {
        if(buf[i] == tok)
            rt += 1;
    }
    return rt;
}

char chk_last_tok(const char *buf, const char tok)
{
    uint8_t sz = strlen(buf);
    if( buf[sz-1] == tok )
        return 1;
    else
    {
        return 0;
    }
    
}

int process_usr_buf(char *usr_buf, struct GS_mdm *gs_tmp_mdm)
{
    uint8_t i = 0;
    char *tmp[5];
    char *st1, *st2;
    st1 = strstr(usr_buf,"rbt=");
    if( st1 && ( usr_buf[3] == '=') && (chk_last_tok(usr_buf,',') == 0) )
    {
        *st1 = ',';
        if(count_tok(usr_buf,',') == 3 )
        {
            printk(KERN_CRIT "process_usr_buf() All Passed.\n");
            while ( ( st2 = strsep(&usr_buf,",") ) != NULL)
            {
                strcpy(tmp[i],st2);
                printk(KERN_CRIT "spliting : %s\n",tmp[i]);
                i += 1;
            }
        }
        return 0;
    }
    else
        return -1;
}
*/
int mdm_init(void)
{
    memset(gs_mdm.mk,0,15);
    sprintf(gs_mdm.mk,"Quectel");
    gs_mdm.typ = 4;
    gs_mdm.sts = 1;
    gs_mdm.sts_tmp = 1;
    gs_mdm.rbt = 0;
    return (gsm_onoff(&gs_mdm));
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
    int ret = 0;
    dev = container_of(inode->i_cdev, struct scull_dev, cdev);
    filp->private_data = dev; // for other methods 
    

    if(down_interruptible(&EGS_sem))
    {
        printk(KERN_ERR DEV_DBG" : Couldn't get the semaphore\n");
        return -ERESTARTSYS;
    }

    // now trim to 0 the length of the device if open was write-only 
    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) 
    {
        scull_trim(dev); // ignore errors 
    }

/*
    if(!SG_sts_op)
    {
        SG_sts_op = 1;
        ret = 0;
    }
    else
    {
        printk(KERN_ERR DEV_DBG" : Device has already opened. Close it first.\n");
        ret = -EBUSY;
    }
*/
    up(&EGS_sem);
    return ret;
    
    /* success */
}

int scull_release(struct inode *inode, struct file *filp)
{
    printk(KERN_CRIT DEV_DBG" : In scull_release()\n");
    //SG_sts_op = 0;
    return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{    
    struct scull_dev *dev = filp->private_data;

    int ret_val=0;
    int sz = 0;
    int i=0;
    printk(KERN_CRIT DEV_DBG" : In scull_read()-%d\n",i++);
    sprintf(SG_buff_rd,"%s,%dg,%d",gs_mdm.mk,\
                                gs_mdm.typ,\
                                gs_mdm.sts);
    sz = strlen(SG_buff_rd);
    dev->size = sz;
    printk(KERN_CRIT DEV_DBG" : In scull_read()-%d\n",i++);
    
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
    
    if(down_interruptible(&EGS_sem))
    {
        printk(KERN_ERR DEV_DBG" : Couldn't get the semaphore in scull_read()\n");
        return -ERESTARTSYS;
    }    
    if( *f_pos > dev->size )
    {
        //printk(KERN_ERR DEV_DBG" : from scull_read(), dev->size = %ld *f_pos = %lld\n",dev->size,*f_pos);
        ret_val = -1;
        goto ret;
    }
    else if (*f_pos == dev->size)
    {
        ret_val = 0;
        goto ret;
    }
#ifdef KDBG
    else
    {
        printk(KERN_INFO DEV_DBG" : From scull_read(), *f_pos = %lld, dev->sz = %ld\n",*f_pos,dev->size);
    } 
#endif
    if(count > sz)
    {   
        count = sz;
#ifdef KDEBG        
        printk(KERN_ERR DEV_DBG" : Error max count = %d\n",sz);
#endif        
    }
    else if (count < 0)
    {
        count = 0;
    }

    if (copy_to_user(buf,SG_buff_rd,count))
    {
        //Error case
        ret_val = -1;
        printk(KERN_ERR DEV_DBG" : The tunnel from user to kernel space has failed.\n");        
        goto ret;
    }
    else
    {
        *f_pos += count;
    }    
    
    ret_val = count;
    ret : ;
    up(&EGS_sem);

#ifdef KDBG
    printk(KERN_CRIT DEV_DBG" : The data has been copied, *f_pos = %lld\n",*f_pos);
#endif
    return ret_val;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    ssize_t rt_val = -1;
    char *rmv = NULL;
    //struct GS_mdm gs_tmp_mdm;

    if(down_interruptible(&EGS_sem))
    {
        printk(KERN_ERR DEV_DBG" : Couldn't get the semaphore in scull_read()\n");
        return -ERESTARTSYS;
    }

    *f_pos += count;
    if( *f_pos > count)
    {        
        return count;
    }
    printk(KERN_INFO DEV_DBG" : In scull_write(), *f_pos = %lld\n",*f_pos);
    if(count > Kbuff_rd_sz)
    {
        printk(KERN_ERR DEV_DBG" : scull_write(): count exceeds the limit (%d)\n",Kbuff_rd_sz);
        rt_val = -ENOSPC;
    }
    else
    {
        printk(KERN_INFO DEV_DBG" : In scull_write()\n");
        memset(SG_buff_wr,0,strlen(SG_buff_wr));
        if ( copy_from_user(SG_buff_wr,buf,count) != 0)
        {
            printk(KERN_CRIT DEV_DBG" : copy_from_user() failed.\n");
            return -1;
        }
        else
        {
            printk(KERN_CRIT DEV_DBG" : From user : %s\n",SG_buff_wr);
            rmv = strchr(SG_buff_wr, '\n'); //The $echo will append this.
            if(rmv)
            {
                *rmv = 0;                
            }    
            rmv = strchr(SG_buff_wr, '\r');
            if(rmv)
            {
                *rmv = 0;                
            }
            //printk(KERN_CRIT "process_usr_buf = %d\n",process_usr_buf(SG_buff_wr,&gs_tmp_mdm) );
        }

        printk(KERN_CRIT DEV_DBG" : strlen(SG_buff_wr) = %d\n",strlen(SG_buff_wr));

        if(strstr(SG_buff_wr,"rbt=") && ( SG_buff_wr[3] == '=') )
        {
            memset(gs_mdm.mk,0,strlen(gs_mdm.mk));
        /*
            if( !strcmp(SG_buff_wr,"rbt=Quectel,2") )
            {
                printk(KERN_ERR DEV_DBG" : Unsupported device Quectel-2g\n");            
                sprintf(gs_mdm.mk,"Quecktel");
                gs_mdm.typ = 2;
                rt_val = -EINVAL;
            }
            else if( !strcmp(SG_buff_wr,"rbt=Quectel,3") )
            {
                printk(KERN_ERR DEV_DBG" : Unsupported device Quectel-3g\n");
                sprintf(gs_mdm.mk,"Quecktel");
                gs_mdm.typ = 3;
                rt_val = -EINVAL;
            }
        */
            if( !strcmp(SG_buff_wr,RBT_Q(4)) )
            {
                sprintf(gs_mdm.mk,MDM_Q);
                gs_mdm.typ = 4;
                gs_mdm.sts = 1;
                gs_mdm.sts_tmp = 1;
                gs_mdm.rbt = 1;

                rt_val = 1;
            }
            else
            {
                printk(KERN_ERR DEV_DBG" : Invalid data(mdm) from user space.\n");
                rt_val = -EINVAL;
            }
            
        }               
        else if( !strcmp(SG_buff_wr,"0") )
        {
            gs_mdm.sts_tmp = 0;
            printk(KERN_CRIT DEV_DBG" : MDM-%s,type-%dg turned off",gs_mdm.mk,gs_mdm.typ);
            rt_val = 0;
        }
        else if( !strcmp(SG_buff_wr,"1") )
        {
            gs_mdm.sts_tmp = 1;
            printk(KERN_CRIT DEV_DBG" : MDM-%s,type-%dg turned on",gs_mdm.mk,gs_mdm.typ);
            rt_val = 1;
        }
        else if( !strcmp(SG_buff_wr,"reboot") )
        {
            printk(KERN_CRIT DEV_DBG" : rebooting the modem... .. .\n");
            //gs_mdm.sts = 1;
            //gs_mdm.sts_tmp = 1;
            gs_mdm.rbt = 1;

            rt_val = 1;
        }
        else
        {
            printk(KERN_CRIT DEV_DBG" : Invalid data(sts) from user space.\n");
            rt_val = -EINVAL;
        }
    }

    if(rt_val >= 0)
    {
        rt_val = gsm_operate(&gs_mdm);
    }

    up(&EGS_sem);

    return rt_val;
}

loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    struct scull_dev *dev = filp->private_data;
    loff_t newpos;
    switch(whence) 
    {
        case 0: /* SEEK_SET */
            newpos = off;
        break;
            case 1: /* SEEK_CUR */
            newpos = filp->f_pos + off;
        break;
            case 2: /* SEEK_END */
            newpos = dev->size + off;
        break;
        default: /* can't happen */
            return -EINVAL;
    }
    if ( (newpos < 0) && ( newpos >= Kbuff_wr_sz ) ) 
        return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}
