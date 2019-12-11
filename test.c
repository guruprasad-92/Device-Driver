#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

int main()
{
    int fd = open("/dev/gsm0",O_RDWR);
    printf("open : fd = %d\n\r",fd);
    char rd[150] = {0};
    int rt_rd = 0;
    if (fd >= 3)
    {
        rt_rd = read(fd,rd,20);
        printf("rt_rd = %d\n",rt_rd);
        if(rt_rd > 0)
        {
            printf("From kernel space : %s\n",rd);
        }
        char val = 3;
        char v[4] = {0};
        printf("Enter mdm : ");
        scanf("%c",&val);
        sprintf(v,"%c",val);
        rt_rd = write(fd,v,4);
        printf("rt_wr = %d\n",rt_rd);
        fd = close(fd);
        printf("close : fd = %d\n",fd);
    }
}