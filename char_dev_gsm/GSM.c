/*
    main()
*/

#include <linux/init.h> /* module_init(), module_exit() */
#include <linux/module.h> /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include "device.h"

MODULE_DESCRIPTION("GSM driver");
MODULE_LICENSE("Dual BSD/GPL");
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
    register_dev();    
    return 0;
}

static void __exit gsm_exit (void)
{
    printk(KERN_INFO "GSM: Removing the %s module.\n",name);
    unregister_dev();
}


module_init(gsm_init);
module_exit(gsm_exit);
