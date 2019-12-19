#include "device.h"


#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h> 
#include <linux/module.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kdev_t.h>

#include "../file_ops/file_ops.h"
#include "../pwr_hndl/pwr_hndl.h"
#include "../dev_dt_struct.h"


 struct class *gsm_class;
struct cdev gsm_dev[GSM_BANK];

 dev_t major;
unsigned int first_minor = 0;
static int sts_major = -1;

//static int gsm_sts;
 dev_t cur_dev[GSM_BANK];

int rt_cdev_add = -1;
//



struct file_operations gsm_dev_fops = 
{
    .owner = THIS_MODULE,
    .open = scull_open,
    .read = scull_read,
    .write = scull_write,
    .release = scull_release,
    .llseek = scull_llseek,
};

int register_dev(void)
{
    int i=0;
#if (DBG_LVL == DBG_MAX )    
    printk(KERN_NOTICE DEV_DBG" : In register_dev().\n");
#endif
        /* Get the mazor and minor number */
    sts_major = alloc_chrdev_region(&major,first_minor,GSM_BANK,DEV_NAME);
    if (sts_major != 0)
    {
        #if (DBG_LVL >= DBG_LOW)
        printk(KERN_ERR DEV_DBG" : Unable to get major number.\n");
        #endif
        return -1;
    }
    else
    {
        #if (DBG_LVL >= DBG_MAX)
        printk(KERN_WARNING DEV_DBG" : Major number = %d\n",MAJOR(major));
        #endif

        gsm_class = class_create(THIS_MODULE, CLASS);
        
        //for(i=0; i<GSM_BANK; i++)// Our requirement is a single device which will not expand
        //{
            cdev_init(&gsm_dev[i], &gsm_dev_fops); // return void
            cur_dev[i] = MKDEV( MAJOR(major), MINOR( major+1 ) );

            sema_init(&(EGS_sem[0].sem),1);
            sema_init(&(EGS_sem[1].sem),1);
            sema_init(&(EGS_sem[2].sem),1);

            rt_cdev_add = cdev_add(&gsm_dev[i], cur_dev[i], 1);
            if(rt_cdev_add < 0)
            {
                
                printk(KERN_ERR DEV_DBG" : Unable to register the device.\n");
                
                return rt_cdev_add;
            }
            else
            {
                
                //device_create(gsm_class, NULL, cur_dev[i], NULL, DEV_NAME "%d",i); // We have changed the name gsm(i) to modem.
                device_create(gsm_class, NULL, cur_dev[i], NULL, DEV_NAME);
                
                printk(KERN_WARNING DEV_DBG" : Registered successfully.\n");               
                
            }
        //}
    }
    return 0;
}

void unregister_dev(void)
{    
    if(sts_major == 0)
    {
        if(mdm_sts())
            gsm_pwr_off();
        gsm_free_gpio();        

        device_destroy(gsm_class,cur_dev[0]);
        class_destroy(gsm_class);
        unregister_chrdev_region(major,GSM_BANK);
        cdev_del(gsm_dev); //cdev_del(&gsm_dev[i]); // Here we were passing i=1. which is invalid
        #if (DBG_LVL >= DBG_LOW)
        printk(KERN_WARNING DEV_DBG" : Removed successfully\n");
        #endif
    }
}