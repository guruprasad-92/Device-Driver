#include <linux/kernel.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

char G_perr[100] = {0};

int pwr_hndl(char *pth, int val)
{
    memset(G_perr,0,100);
    int rt_op = open(pth,O_RDWR);
    if(rt_op < 0)
    {
        printk(KERN_ERR "GSM: Unable to open ");
    }
}