#ifndef PWR_HNDL
#define PWR_HNDL

#include <linux/gpio.h>
#include "../dev_dt_struct.h"

#define PIOA 0
#define PIOB ( 1 * 32 )
#define PIOC ( 2 * 32 )
#define PIOD ( 3 * 32 )

#define PIO_CREATE_PIN(base,no) (base + no)

#define GSM_VBAT_EN_LED (PIO_CREATE_PIN(PIOA,16)) // flx-4.3 (PC-31) //green
#define GSM_ON_OFF_LED  (PIO_CREATE_PIN(PIOA,14)) //  flx-4.4 (PD-00) 

#define GSM_ON_OFF      (PIO_CREATE_PIN(PIOD,00)) // flx-4.4 (pd0)
#define GSM_VBAT_EN     (PIO_CREATE_PIN(PIOC,31)) //flx-4.3 (pc31)

#define GSM_STS_PWR    ( 1 << 0 )

#define GSM_DL_1    10  //mili Sec
#define GSM_DL_2    110 //mili Sec
#define GSM_DL_3    13 // Sec




int qktl4g_pwr_cycle(struct GS_mdm *gs_mdm);
void gsm_free_gpio(void);
void gsm_pwr_off(void);
int gsm_onoff(struct GS_mdm *gs_mdm);
int gsm_operate(struct GS_mdm *gs_mdm);

#endif