#ifndef PWR_HNDL
#define PWR_HNDL

#include <linux/gpio.h>

#define PIOA 0
#define PIOB ( 1 * 32 )
#define PIOC ( 2 * 32 )
#define PIOD ( 3 * 32 )

#define PIO_CREATE_PIN(base,no) (base + no)

#define GSM_VBAT_EN (PIO_CREATE_PIN(PIOA,14)) // flx-4.3 (PC-31)
#define GSM_ON_OFF  (PIO_CREATE_PIN(PIOA,16)) //  flx-4.4 (PD-00)
#define GSM_STS_PWR    ( 1 << 0 )



int gsm_pwr_cycle(void);
void gsm_free_gpio(void);

#endif