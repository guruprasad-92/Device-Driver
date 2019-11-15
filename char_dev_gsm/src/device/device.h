#ifndef DEVICE_H
#define DEVICE_H

#define GSM_BANK      1
#define DEV_NAME    "gsm"
#define CLASS       "gsm-class"


#include <linux/compiler.h> // required for "__must_check"

__must_check int register_dev(void);

void    unregister_dev(void);

#endif
