#ifndef DEVICE_H
#define DEVICE_H

#include <linux/compiler.h> // required for "__must_check"
#include "../dev_dt_struct.h"

__must_check int register_dev(void);

void    unregister_dev(void);


#endif
