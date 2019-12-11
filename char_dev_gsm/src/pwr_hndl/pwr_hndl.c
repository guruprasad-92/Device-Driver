#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/delay.h>


#include "pwr_hndl.h"

//char G_perr[100] = {0};

struct gpio GSM_PINS[] = {
    {GSM_VBAT_EN, GPIOF_OUT_INIT_LOW, "vbatt_en"},
    {GSM_ON_OFF, GPIOF_OUT_INIT_LOW, "on_off"},
};

int qktl4g_pwr_cycle(void)
{
    int err = 0;
    int dl = 500;
    int i;
    err = gpio_request_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
    printk(KERN_INFO "GSM : gpio_request_array() = %d\n",err);
    if (err >= 0)
    {
        gpio_set_value(GSM_PINS[1].gpio,1);
        for(i=0;i<3;i++)
        {            
            gpio_set_value(GSM_PINS[0].gpio,1);
            printk(KERN_INFO "GSM:----\nGSM : PA16 = 1\n");
            mdelay(dl);
            gpio_set_value(GSM_PINS[0].gpio,0);
            printk(KERN_INFO "GSM : PA16 = 0\nGSM:----\n"); 
            mdelay(dl);
        }
        return 0;
    }
    else
    {
        return -1;
    }
}

void gsm_free_gpio(void)
{
    gpio_free_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
}

void gsm_pwr_off(void)
{
    gpio_set_value(GSM_PINS[0].gpio,0);
    gpio_set_value(GSM_PINS[1].gpio,0);
    gpio_free_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
}
