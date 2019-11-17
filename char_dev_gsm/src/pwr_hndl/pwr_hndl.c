#include <linux/kernel.h>

#include "pwr_hndl.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

char G_perr[100] = {0};

void blink_led(int fd, float tm_p, float duty, int no)
{
    int t_on = (int)( (duty*tm_p)/100 );
    int t_off = (int)tm_p - t_on;
    for(int i=0;i<no;i++)
    {
        write(fd,"1",1);
        usleep((u_int32_t)t_on * 1000);
        write(fd,"0",1);
        usleep((u_int32_t)t_off * 1000);
    }
}

int pwr_handl(char *pth, float tm_p, float duty, int no)
{
    memset(G_perr,0,100);
    int fd = open(pth,O_RDWR);
    if(fd < 0)
    {
        printk(KERN_ERR "GSM: Unable to open %s\n",pth);
    }
    else
    {
        printk(KERN_INFO "GSM: LED has been opened\n");
        blink_led(fd,duty,tm_p,val);
    }
    
}