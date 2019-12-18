#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>


#include "pwr_hndl.h"


static struct gpio GSM_PINS[] = {
    {GSM_VBAT_EN, GPIOF_OUT_INIT_LOW, "vbatt_en"},
    {GSM_ON_OFF, GPIOF_OUT_INIT_LOW, "on_off"},
    {GSM_VBAT_EN_LED, GPIOF_OUT_INIT_LOW, "vbatt_en_led"},
    {GSM_ON_OFF_LED, GPIOF_OUT_INIT_LOW, "on_off_led"},
};

//#define GPIO_RQST ( gpio_request_array(GSM_PINS, ARRAY_SIZE(GSM_PINS)) )

int qktl4g_pwr_cycle(struct GS_mdm *gs_mdm)
{    
    static int chk;
    static int rt_gpio_rqst = -1;
    int i;    
    if (!chk)
    {
        rt_gpio_rqst = gpio_request_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
        chk = 1;
    }
    
    //rt_rqst_arr = gpio_request_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
    printk(KERN_INFO DEV_DBG" : gpio_request_array() = %d\n",rt_gpio_rqst);
    if (rt_gpio_rqst >= 0)
    {
        for(i=0;i<4;i++)
        {
            gpio_set_value(GSM_PINS[i].gpio,1);
        }
        msleep(GSM_DL_1);

        gpio_set_value(GSM_PINS[1].gpio,0);
        gpio_set_value(GSM_PINS[3].gpio,0);
        msleep(GSM_DL_2);
        
        gpio_set_value(GSM_PINS[1].gpio,1);
        gpio_set_value(GSM_PINS[3].gpio,1);
        ssleep(GSM_DL_3);

        return 0;
    }
    else
    {
        return -EBUSY;
    }
}

void gsm_free_gpio(void)
{
    gpio_free_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
}

void gsm_pwr_off(void)
{
    uint8_t i=0;
    gpio_set_value(GSM_PINS[1].gpio,0);
    gpio_set_value(GSM_PINS[3].gpio,0);
    msleep(700);

    gpio_set_value(GSM_PINS[1].gpio,1);
    gpio_set_value(GSM_PINS[3].gpio,1);
    ssleep(30);

    for(i=0;i<4;i++)
    {
        gpio_set_value(GSM_PINS[i].gpio,0);
    }
    msleep(50);
    //No need to free the array, we are making it free at device removing stage.
}

int gsm_onoff(struct GS_mdm *gs_mdm)
{
    int rt_val = -EINVAL;
    if( !strcmp(gs_mdm->mk,MDM_Q) )
    {
        if ( gs_mdm->typ == 4 )
        {
            if(gs_mdm->sts)
            {
                if (qktl4g_pwr_cycle(gs_mdm) == -1 )
                    rt_val = -EBUSY;
                else
                {
                    rt_val = 1;
                }
            }
            else
            {
                gsm_pwr_off();
                rt_val = 0;
            }
        }
        else if( gs_mdm->typ == 3 )
        {
            printk(KERN_DEBUG DEV_DBG" : Unsupported model-%d\n",gs_mdm->typ);
            rt_val = -EINVAL;
        }
        else if( gs_mdm->typ == 2 )
        {
            printk(KERN_DEBUG DEV_DBG" : Unsupported model-%d\n",gs_mdm->typ);
            rt_val = -EINVAL;
        }
        else
        {
            printk(KERN_DEBUG DEV_DBG" : Unsupported model-%d\n",gs_mdm->typ);
            rt_val = -EINVAL;
        }
        
    }
    else if (!strcmp(gs_mdm->mk,MDM_T))
    {
        if ( gs_mdm->typ == 4 )
        {
            rt_val = -EINVAL;
        }
        else if( gs_mdm->typ == 3 )
        {
            rt_val = -EINVAL;
        }
        else if( gs_mdm->typ == 2 )
        {
            rt_val = -EINVAL;
        }
    }
    else
    {
        printk(KERN_DEBUG DEV_DBG" : Unsupported modem \"%s\"\n",gs_mdm->mk);
        rt_val = -EINVAL;
    }    
    return rt_val;
}

int gsm_operate(struct GS_mdm *gs_mdm)
{
    int rt_val = -1;    
    if (gs_mdm->rbt)
    {
        if (gs_mdm->sts == 0 )
        {
            gs_mdm->sts = 1;
            gs_mdm->sts_tmp = 1;
            
            if ( gsm_onoff(gs_mdm) < 0 )
            {
                rt_val = -EBUSY;
                //goto ret;
            }
            else
            {
                rt_val = 1;
            }
            
        }
        else
        {
            gs_mdm->rbt = 0;
            gs_mdm->sts = 1;
            gs_mdm->sts_tmp = 1;
            gsm_pwr_off();
            rt_val = gsm_onoff(gs_mdm);
        }
    }
    else if (gs_mdm->sts != gs_mdm->sts_tmp)
    {
        gs_mdm->sts = gs_mdm->sts_tmp;
        rt_val = gsm_onoff(gs_mdm);
    }
    else if ( (gs_mdm->sts >= 0) && (gs_mdm->sts <= 1) )
    {
        rt_val = gs_mdm->sts;
    }
    else
    {
        rt_val = -EINVAL;
    }    
    gs_mdm->rbt = 0;
    return rt_val;
}