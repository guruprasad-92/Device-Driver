#ifndef DEVICE_H
#define DEVICE_H

#include <linux/compiler.h> // required for "__must_check"

__must_check int register_dev(void);

void    unregister_dev(void);

#endif
