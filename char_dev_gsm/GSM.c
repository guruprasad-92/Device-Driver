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

#include "src/device/device.h"
#include "src/pwr_hndl/pwr_hndl.h"
#include "src/file_ops/file_ops.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Guruprasad");
MODULE_DESCRIPTION("A simple driver");
MODULE_VERSION("0.1");


static char *name = "GSM";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description


/*
    ----------------------------------------------------------------------------
*/

static int __init gsm_init (void)
{    
    printk(KERN_INFO "GSM: Initialising the %s module.\n",name);    
    if(register_dev() >= 0)
    {
        if (mdm_init() == 0) 
        return 0;
    }
    else
    {

        return -1;
    }
    return 0;
}

static void __exit gsm_exit (void)
{
    printk(KERN_INFO "GSM: Removing the %s module.\n",name);
    unregister_dev();
    gsm_pwr_off();
}

module_init(gsm_init);
module_exit(gsm_exit);
