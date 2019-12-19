/*
    Reference link : 
    https://embetronicx.com/tutorials/linux/device-drivers/linux-device-driver-tutorial-programming/
    http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
*/
/*
    main()
*/

#include <linux/init.h> /* module_init(), module_exit() */
#include <linux/module.h> /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/errno.h>

#include "src/device/device.h"
#include "src/file_ops/file_ops.h"
#include "src/dev_dt_struct.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Guruprasad");
MODULE_DESCRIPTION("A simple driver");
MODULE_VERSION("0.1");


static char *name = "MDM";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description


/*
    ----------------------------------------------------------------------------
*/

static void __exit gsm_exit (void)
{
    #if (DBG_LVL >= DBG_LOW)
    printk(KERN_INFO DEV_DBG" : Removing ...\n");
    #endif
    unregister_dev();    
}

static int __init gsm_init (void)
{
    int ret = -1;

    printk(KERN_INFO DEV_DBG" : Initialising... .. .\n");    

    if(register_dev() >= 0)
    {
        printk(KERN_INFO DEV_DBG" : Performing I/O operations.\n");
        ret = mdm_io_init();
        if (ret == 1)
        {
            printk(KERN_INFO DEV_DBG" : Added successfully\n");
            ret = 0;
        }
        else
        {            
            printk(KERN_ERR DEV_DBG" : Could not initialise <Error in I/O>.\n");
            gsm_exit();
            ret = -EIO;
        }
    }
    else
    {
        ret = -EBUSY;
    }
    return ret;
}

module_init(gsm_init);
module_exit(gsm_exit);
