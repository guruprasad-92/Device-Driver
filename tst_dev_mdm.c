/*
    Purpose : Test the modem device along with modem communication
    Author : Guruprasad
*/

#include <stdint.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include <linux/serial.h>
/* Include definition for RS485 ioctls: TIOCGRS485 and TIOCSRS485 */
#include <sys/ioctl.h>


#define TST_MDM

#ifdef TST_MDM
    #define DFLT_BDRT    115200
#endif

#define MDM_DEV     "/dev/modem"
#define MDM_COM    "/dev/ttyS8"
#define PORT     8
#define SZ_AT   10

int bd_rt_hld = 0;

char mdm_rd[225] = {0};
char mdm_wr[50] = {0};

struct ST_at
{
    char cmd[50];
};


int tty_open (uint8_t port, int op_as)
{
    int fd = -1;
    char dev[20] = {0};
    sprintf(dev,"/dev/ttyS%d",port);
    printf("device = %s\n",dev);
    fd = open(dev,op_as | O_NOCTTY);
    printf("fd = %d\n",fd);
    if (fd < 0)
    {
        printf("Unable to open ttyS%d ",port);
        perror("due to ");
        printf("\nExiting ... .. .");
        exit (0);
    }
    return fd;
}


int get_bdrt(void)
{
    int bd_rt = 0;
#ifndef TST_MDM
    printf("\nPlease enter baudrate : ");
    
    while((getchar() != '\n'));
    scanf("%d",&bd_rt);
#else
    bd_rt = DFLT_BDRT;
#endif
    bd_rt_hld = bd_rt;
    switch(bd_rt)
    {
        case 0 :
            bd_rt = B0;break;
        case 50 : 
            bd_rt = B50; break;
        case 75 :
            bd_rt = B75; break;
        case 110 :
            bd_rt = B110; break;
        case 134 :
            bd_rt = B134; break;
        case 150 :
            bd_rt = B150; break;
        case 200 :
            bd_rt = B200; break;
        case 300 :
            bd_rt = B300; break;
        case 600 :
            bd_rt = B600; break;
        case 1200 :
            bd_rt = B1200; //1
            #ifdef C2C
                band_dly = 9980;// We are getting 1375 with 9980
            #endif
            break;
        case 1800 :
            bd_rt = B1800; //2
            #ifdef C2C
                band_dly = 10000; //For 4284 trying with (8000/2548) * 4248 (low resolution)
            #endif
            break;

        case 2400 :
            bd_rt = B2400; //3
            #ifdef C2C
                band_dly = 9900; //For 5749, trying with 50000
            #endif
            break;
        case 4800 :
            bd_rt = B4800; //4
            #ifdef C2C
                band_dly = 9900; //for 7928
            #endif
            break;
        case 9600 :
            bd_rt = B9600; //5
            #ifdef C2C
                band_dly = 10000; //for 9026
            #endif
            break;
        case 19200 :
            bd_rt = B19200; //6 
            #ifdef C2C
                band_dly = 9900; //for 9571
            #endif
            break;
        case 38400 :
            bd_rt = B38400; //7
            #ifdef C2C
                band_dly = 9900; //for 9842 us
            #endif
            break;
        case 57600 :
            bd_rt = B57600; //8
            #ifdef C2C
                band_dly = 9934; //for 9934
            #endif
            break;
        case 115200 :
            bd_rt = B115200; //9
            #ifdef C2C
                band_dly = 9920; // for 10030
            #endif
            break;
        case 230400 :
            bd_rt = B230400; //10
            #ifdef C2C
                band_dly = 9930; //for 10070
            #endif
            break;
        default :
            bd_rt = -1; break;        
    }
    return bd_rt;
}


int tty_set (int fd, int tm_sec)
{
    struct termios tty;
   // memset(&tty,0,sizeof(tty));
    tcgetattr(fd,&tty);
    int bd_rt;
    bd_rt = get_bdrt();
    if (bd_rt == -1)
    {
        printf("\nWrong baud rate. Exiting ... .. .\n");
        exit(0);
    }
    else
    {
        cfsetispeed(&tty, bd_rt);
        cfsetospeed(&tty, bd_rt);
    }
    int rt_tcflush = tcflush(fd,TCIOFLUSH);    

    /*  mode : 8N1 */
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);


    //tty.c_iflag |= (IXON | IXOFF | IXANY);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    tty.c_iflag = 0;
    tty.c_iflag &= ~( INLCR | ICRNL | IGNCR );

    
    tty.c_lflag &= ~NOFLSH;
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN] = 5; // 
    tty.c_cc[VTIME] = (tm_sec*10); //deci-sec

    int re_set = tcsetattr(fd, TCSANOW, &tty);
    printf("tcsetattr() = %d\n",re_set);
    if (re_set !=0 )
    {
        perror("\n  ERROR ! in Setting attributes, due to ");
        printf("\nExiting ... .. .\n");
        exit(0);
        return -1;
    }
    else
    {
        return 0;
    }
}


void msleep(unsigned int sl)
{
    while(usleep((unsigned int)1000*sl));
}

int fd_mdm_dev = -1;
int fd_mdm_com = -1;
int rt_mdm_dev, rt_mdm_com,cnt1,cnt3;

int gsm_send(char *data, int tm)
{
    printf("\nIn gsm_snd()\n");
    int sz = strlen(data);
    printf("To modem : \n");
    for(int i=0;i<strlen(data);i++)
    {
        if( data[i]==0xd || data[i]==0xa)
        {
            printf("'0x%x'",data[i]);
        }
        else
        {
            printf("%c",data[i]);
        }
    }
    printf("\n");
    
    tcflush(fd_mdm_com,TCIOFLUSH);
    lseek(fd_mdm_com,0,SEEK_SET);

    rt_mdm_com = write(fd_mdm_com,data,sz);
    
    memset(mdm_rd,0,strlen(mdm_rd));
    //memset(data,0,strlen(data)); // write is fixed. no need to clean it
    
    printf("rt_mdm_com = %d\n",rt_mdm_com);
    
    if (tm <50)
        tm = 50;

    for(int i=0;i<tm;)
    {   
        sz = strlen(mdm_rd);
        printf("size of mdm_rd = %d\n",sz);
        rt_mdm_com = read(fd_mdm_com,mdm_rd+sz,225);
        printf("rt_mdm_com = %d\n",rt_mdm_com);
        if( strstr(mdm_rd,"OK\r\n"))
        {
            printf("\n i = %d. Breaking\n",i);
            return 0;
        }
        else if ( strstr(mdm_rd,"ERROR\r\n") )
        {
            printf("\n i = %d. Breaking\n",i);
            return 1;
        }
        msleep(50);
        i+=50;
    }
    return -1;
}

int main(void)
{
    uint8_t op_tm = 0;
    uint8_t tm;
    struct ST_at st_at[SZ_AT];
    for(int i=0;i<SZ_AT;i++)
    {
        memset(st_at[i].cmd,0,50);
    }
    sprintf(st_at[0].cmd,"AT\r");
    sprintf(st_at[1].cmd,"ATI\r");
    sprintf(st_at[2].cmd,"AT+GMI\r");
    sprintf(st_at[3].cmd,"AT+GMM\r");
    sprintf(st_at[4].cmd,"AT+GMR\r");
    sprintf(st_at[5].cmd,"AT+CGMI\r");
    sprintf(st_at[6].cmd,"AT+CGMM\r");
    sprintf(st_at[7].cmd,"AT+CGMR\r");
    sprintf(st_at[8].cmd,"AT+GSN\r");
    sprintf(st_at[9].cmd,"AT+CGSN\r");

    fd_mdm_com = tty_open(PORT,O_RDWR);

    fd_mdm_dev = open(MDM_DEV,O_WRONLY);
    printf("\nopen(\"/dev/modem\") = %d\n",fd_mdm_dev);
    if(fd_mdm_dev < 3 )
    {
        perror("Could not open mdm_dev due to ");
        printf("fd_mdm_dev = %d\n",fd_mdm_dev);
        printf("Exiting ... .. .\n");
        exit(0);
    }

    if(fd_mdm_com < 3 )
    {
        perror("Could not open mdm_com due to ");
        printf("fd_mdm_com = %d\n",fd_mdm_com);
        printf("Exiting ... .. .\n");
        exit(0);
    }

    printf("Enter modem operation time : ");
    scanf("%hhd",&op_tm);
    cnt3 = 0;
    for(int i=0;i<op_tm; i++)
    {
        printf("\n--------------- Test modem (%d) -----------\n",i);
        printf("\nRebooting the modem. This will take a minute.");
        
        rt_mdm_dev = lseek(fd_mdm_dev,0,SEEK_SET);
        printf("\nlseek(fd_mdm_dev) = %d\n",rt_mdm_dev);
        if(rt_mdm_dev != 0)
        {
            perror("lseek() failed due to ");
            printf("\nExiting ... .. .\n");
            exit(0);
        }
        rt_mdm_dev = write(fd_mdm_dev,"reboot",6);
        
        printf("rt_mdm_dev = %d\n",rt_mdm_dev);
        msleep(50);

        rt_mdm_dev = 1;
        if(rt_mdm_dev != 1 )
        {
            perror("Write error in /dev/modem due to ");
            printf("rt_mdm_dev = %d\n",rt_mdm_dev);
            printf("Exiting ... .. .\n");
            exit(0);
        }
        else
        {
            //------- modem rebooted -----------
            cnt1 = 0;

            for(int i=0;i<5;i++)
            {       
                if(gsm_send(st_at[i].cmd,225) == 0 )
                {
                    cnt1 +=1;
                }                
                printf("\nFrom modem : ");
                for(int i=0;i<strlen(mdm_rd);i++)
                {
                    if( mdm_rd[i]==0xd || mdm_rd[i]==0xa)
                    {
                        printf("'0x%x'",mdm_rd[i]);
                    }
                    else
                    {
                        printf("%c",mdm_rd[i]);
                    }
                }
            }
            printf("cnt1-%d = %d\n",i,cnt1);
            if (cnt1>3)
            {
                cnt3 += 1;
            }
        }
    }
    printf("\n\r--------- RESULT -----------\n\r");
    printf("op_tm = %d\n",op_tm);
    printf("cnt3 = %d\n",cnt3);
    close(fd_mdm_dev);
    close(fd_mdm_com);
}