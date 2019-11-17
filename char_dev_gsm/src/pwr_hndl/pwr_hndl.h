
#ifndef PWR_HNDL
#define PWR_HNDL

    #define GSM_VBAT_EN "/sys/class/leds/led1/brightness"
    #define GSM_ON_OFF  "/sys/class/leds/led2/brightness"

    #define GSM_STS_PWR    ( 1 << 0 )
    
    void blink_led(int fd, float tm_p, float duty, int no);
    int pwr_handl(char *pth, float tm_p, float duty, int no);

#endif



/*
    To cross compile a lodable kernel module, the downloded kernel
    has to be cross compiled first.

    Packages required for compiling the kernel
    1. openssl-dev <sudo apt install libssl-dev >
    2. bison <sudo apt install bison >
    3. yac <sudo apt install yac >
*/