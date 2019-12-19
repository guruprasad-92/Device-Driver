#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>


#include "pwr_hndl.h"
#include "../dev_dt_struct.h"


/*
    Use of semaphores : 
        sem-1 : interruptible semaphore, in file operations.
        sem-2 : Un-interruptible semaphore in, gsm_pwr_off() 
        sem-3 : Un-interruptible semaphore in, qktl4g_pwr_cycle()
*/
struct EGS_SEM EGS_sem[3];



static struct gpio GSM_PINS[] = {
    {GSM_VBAT_EN, GPIOF_OUT_INIT_LOW, "vbatt_en"},
    {GSM_ON_OFF, GPIOF_OUT_INIT_LOW, "on_off"},
    {GSM_VBAT_EN_LED, GPIOF_OUT_INIT_LOW, "vbatt_en_led"},
    {GSM_ON_OFF_LED, GPIOF_OUT_INIT_LOW, "on_off_led"},
};

//#define GPIO_RQST ( gpio_request_array(GSM_PINS, ARRAY_SIZE(GSM_PINS)) )

/*
    Success : 0
    error   : -Ve
*/
int qktl4g_pwr_cycle(struct GS_mdm *gs_mdm)
{    
    static int chk;
    static int rt_gpio_rqst = -1;
    int i;
    int rt_val=-1;

    if ( down_trylock(&(EGS_sem[1].sem)) != 0 )
    {
        rt_val = -ERESTARTSYS;
        #if ( DBG_LVL >= DBG_LOW )
        printk(KERN_ERR DEV_DBG" : Could not get the semaphore\n");
        #endif
        goto ret;
    }

    if (!chk)
    {
        rt_gpio_rqst = gpio_request_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
        chk = 1;
    }
    
    //rt_rqst_arr = gpio_request_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
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

        rt_val = 0;
    }
    else
    {
        rt_val = -EBUSY;
    }
    ret : ;
    up(&EGS_sem[1].sem);
    return rt_val;
}

void gsm_free_gpio(void)
{
    gpio_free_array(GSM_PINS, ARRAY_SIZE(GSM_PINS));
}

void gsm_pwr_off(void)
{
    uint8_t i=0;
    if ( down_trylock(&(EGS_sem[2].sem)) != 0 )
    {
        
        #if ( DBG_LVL >= DBG_LOW )
        printk(KERN_ERR DEV_DBG" : Could not get the semaphore\n");
        #endif
        goto ret;
    }
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
    ret : ;
    up(&EGS_sem[2].sem);
    //No need to free the array, we are making it free at device removing stage.
}

/*
    Turn on / off the modem :
    Return :
        Success : 1 / 0 {modem status}
        Error : -Ve
*/
int gsm_onoff(struct GS_mdm *gs_mdm)
{
    int rt_val = -EINVAL;
    if( !strcmp(gs_mdm->mk,MDM_Q) ) //Quectell modems
    {
        if ( gs_mdm->typ == 4 ) //4g modems
        {
            if(gs_mdm->sts)
            {
                if ( (rt_val = qktl4g_pwr_cycle(gs_mdm) ) < 0 ) // 
                    rt_val = -EIO;
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

/*
    Return : 
        Success : 0 { modem status }
                : 1 { modem status }
        Error : negetive {modem status = previous status}
*/
int gsm_operate(struct GS_mdm *gs_mdm)
{
    int rt_val = -1;    
    if (gs_mdm->rbt)
    {
        gs_mdm->rbt = 0; // Irrespective of Success / Error
        // Because for the retry after error case.
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
            //gs_mdm->rbt = 0;
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
    
    return rt_val;
}