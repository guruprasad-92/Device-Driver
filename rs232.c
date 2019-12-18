/*
    Link for interrupt : https://stackoverflow.com/questions/15119412/setting-serial-port-interruption-in-linux
*/

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


//#define SET_FLX1
#define OPTM_INF
//#define C2C
#define GD_BND 4

unsigned int band_dly = 0;

//#define __USE_MISC

typedef enum RTS {
    OFF,
    ON
}RTS_ONTX;

int tty_open (char *port, int op_as)
{
    int fd = -1;
    char dev[20] = {0};
    sprintf(dev,"/dev/tty%s",port);
    printf("device = %s\n",dev);
    fd = open(dev,op_as | O_NOCTTY);
    printf("fd = %d\n",fd);
    if (fd < 0)
    {
        printf("Unable to open tty%s ",port);
        perror("due to ");
        printf("\nExiting ... .. .");
        exit (0);
    }
    return fd;
}
int bd_rt_hld;
int get_bdrt(void)
{
    printf("\nPlease enter baudrate : ");
    int bd_rt = 0;
    while((getchar() != '\n'));
    scanf("%d",&bd_rt);
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
    printf("\nrt_tcflush = %d\n",rt_tcflush);
    

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
    printf("re_set = %d\n",re_set);
    printf("\nc_iflag = 0x%x, c_oflag = 0x%x\n",tty.c_iflag,tty.c_oflag);
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

int mode_rs485(int *fd, RTS_ONTX rts,  int rts_dly)
{
    static struct serial_rs485 rs485conf;
    rs485conf.flags |= SER_RS485_ENABLED;

    if(rts == ON)
    {
        /* Set logical level for RTS pin equal to 1 when sending: */
	    rs485conf.flags |= SER_RS485_RTS_ON_SEND;

        /* set logical level for RTS pin equal to 0 after sending: */
	    rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);
    }
    else if(rts == OFF)
    {
        rs485conf.flags &= ~SER_RS485_RTS_ON_SEND;
        /* set logical level for RTS pin equal to 1 after sending: */
	    rs485conf.flags |= (SER_RS485_RTS_AFTER_SEND);
    }

    rs485conf.delay_rts_after_send = rts_dly; /*  Delay in milisec */
    //rs485conf.delay_rts_before_send = rts_dly;/*  Delay in milisec */

    if (ioctl (*fd, TIOCSRS485, &rs485conf) < 0) 
    {
		/* Error handling. See errno. */
        return -1;
	}
    else
    {
        return 0;
    }
}

static void nano_msleep(long dl)
{
    static struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = dl * 1000L;
    nanosleep(&req, (struct timespec*)NULL);
}

void msleep(unsigned int sl)
{
    while(usleep((unsigned int)1000*sl));
}

int usart_write(int fd, char *str, unsigned int dl)
{
    int sz = strlen(str);
    char c;
    for(int i=0;i<sz; i++)
    {
        
        c = *(str+i);
        write(fd,&c,1);
        nano_msleep((long)dl);        
        //usleep((unsigned int)dl);
    }
}

/*********************** LED ************************** */
#define PTH_LED "/sys/class/leds"
#define GRN "/green/brightness"
#define RED "/red/brightness"

typedef struct 
{
    char pth[100];
}LED;

static LED led[2] = {0};

void led_init(void)
{
    sprintf(led[0].pth,PTH_LED);
    sprintf(led[1].pth,PTH_LED);
    strcat(led[0].pth,GRN);
    strcat(led[1].pth,RED);
    printf("path = %s\n",led[0].pth);
    printf("path = %s\n",led[1].pth);
}

int led_blink(int pin, float tm, float duty, int rpt)
{
    float on = tm * (duty/100);
    float off = tm - on;
    unsigned int t_on = (unsigned int)on;
    unsigned int t_off = (unsigned int)off;
    printf("t_on = %d\n",t_on);
    printf("t_off = %d\n",t_off);
    int fd = 0;
    switch (pin)
    {
        case 0 :
            fd = open(led[0].pth,O_WRONLY);
            printf("fd = %d\n",fd);
            if(fd > 0)
            {
                
                for (int i=0;i<rpt; i++)
                {
                    write(fd,"1",1);
                    printf("ON ");
                    msleep(t_on);
                    write(fd,"0",1);
                    printf("OFF");
                    msleep(t_off);
                }
                printf("\n");
                close(fd);
            }
            else{
                perror("fd failed due to ");
            }
        break;
        case 1 :             
            fd = 0;
            fd = open(led[1].pth,O_WRONLY);
            printf("fd = %d\n",fd);
            if(fd > 0)
            {
                
                for(int i=0; i<rpt; i++)
                {
                    write(fd,"1",1);
                    printf("ON ");
                    msleep(t_on);
                    write(fd,"0",1);
                    printf("OFF");
                    msleep(t_off);
                }
                printf("\n");
                close(fd);
            }
            else{
                perror("fd failed due to ");
            }           
        break;

    }
}
//-------------------------------------------------------------


int main(void)
{
/*
    int fd[7];
    
    for(int i=0; i<4; i++)
    {
        fd[i] = tty_open( i, O_WRONLY);
        printf("\nfd of ttyS%d = %d\n",i,fd[i]);
    }
    
    //fd[1] = tty_open(1,O_WRONLY);
    if( fd[0] > 0)
    {
        int set = tty_set(fd[0],3);
        if(set == 0)
        {
            for(int i=0;i<4;i++)
            {
                printf("Writing .. in uart-5\n");
                char ch[50] = {0};
                sprintf(ch,"Itronics-UART-5\n\r");
                write(fd[0],ch,sizeof(ch));
            }
        }
    }

    if( fd[1] > 0)
    {
        int set = tty_set(fd[1],3);
        if(set == 0)
        {
            for(int i=0;i<4;i++)
            {
                printf("Writing .. in uart-5\n");
                char ch[50] = {0};
                sprintf(ch,"Itronics-UART-5\n\r");
                write(fd[1],ch,sizeof(ch));
            }
        }
    }

    //close(fd[1]);
    
    for(int i=0; i<4; i++)
    {
        if(fd[i] >= 0)
        {
            close(fd[i]);
        }
    }
*/

    int u=-1;
    int op=0;
#ifndef SET_FLX1 
    printf("Check UART (0,2,3,4,5,6,7,8) \n");
    printf("Please enter : ");
    
    scanf("%d",&u);

    printf("\n1. Read only");
    printf("\n2. Write only");
    printf("\n3. read-write");
    printf("\n4. Modem-terminal");
    printf("\nPlease enter : ");   

    scanf("%d",&op);
#else
    op=2; //Write only
    u=4;  // UART-4
    printf("\nSerial %d has been selected.\n",u);
    printf("Device has choosen for Write only.\n");
#endif


    printf("\nEnter operation time : ");
    int op_tm=0;
    scanf("%d",&op_tm);
    int fd = -1;
    char dev[8] = {0};
    switch(u)
    {
        case 0:
            sprintf(dev,"S1");
            break;
        case 2 ... 8 :
            sprintf(dev,"S%d",u);
            break;
        default :
            printf("\nWrong choice.\n");
        break;
    }
    if( u>=0 && u<=8 )
        fd = tty_open(dev,O_RDWR | O_NONBLOCK);
     
    if(fd > 0)
    {
        int tty_mod=-1;

    #ifndef SET_FLX1
        printf("\nEnter communication mode : (0. rs232, 1. rs485) : ");        
        scanf("%d",&tty_mod);

        if(tty_mod)
        {
            int rts;
            printf("\nEnter RTS value on Tx (0 / 1) : ");
            scanf("%d",&rts);
            if(rts)
            {
                int gd_bnd=0;
                printf("Enter numbers of guard band : ");
                scanf("%d",&gd_bnd);
                int rt_rs485 = mode_rs485(&fd,rts,gd_bnd);
                printf("\nrt_rs485 = %d",rt_rs485);                
            }
            else
            {
                printf("Default mode = rs232\n");
            }
            
        }
    #else
        printf("Communication mode = Rs485\n");
        printf("RTS value on Tx = 1\n");
        int rt_rs485 = mode_rs485(&fd,1,GD_BND);
        printf("\nrt_rs485 = %d",rt_rs485);  
    #endif

        int set = tty_set(fd,5);
        char str_wr[35] = {0};
        char str_wr1[35] = {0};
        char str_rd[1024] = {0};
                
        sprintf(str_wr,"InvDis\n\r");
        sprintf(str_wr1,"InvDis\n\r");
        printf("\nEnter the string delay : ");
        int str_dl = 0;
        scanf("%d",&str_dl);
        led_init();
        led_blink(0,100,30,10);
        int rt_rd = 0;
        int tm=0;
        int chr=0;
        int sz_wr=0;
        int ii;
        for(int i=0;i<op_tm;i++)
        {
            printf("\nBaud-rate = %d\n\r",bd_rt_hld);
            
            switch(op)
            {
                case 4: // MODEM TERMINAL
                    memset(str_wr,0,strlen(str_wr));
                    memset(str_rd,0,strlen(str_rd));                    
                    printf("\nEnter AT cmd : ");
                    scanf("%s",str_wr);
                    sz_wr = strlen(str_wr);
                    printf("[case 4] : sz_wr = %d, str_wr[2] = %d",sz_wr,str_wr[2]);
                    str_wr[sz_wr] = '\r';
                    //str_wr[sz_wr+1] = '\n';
                    sz_wr = strlen(str_wr);
                    
                    if(tcflush(fd,TCIOFLUSH) != 0)
                    {
                        perror("\nUnable to fush tty buffer due to ");
                        
                    }
                    else
                    {
                        printf("tty I/O buffer has been flushed\n");
                    }
                    printf("To modem : \n");
                    for(int i=0;i<strlen(str_wr);i++)
                    {
                        if( (str_wr[i] == '\n') || (str_wr[i] == '\r') )
                        {
                            printf("'0x%x'",str_wr[i]);
                        }
                        else
                            printf("%c",str_wr[i]);
                    }
                                        
                    rt_rd = write(fd,str_wr,sz_wr);
                    printf("\nrt_wr = %d\n",rt_rd);

                    rt_rd =  chr = tm = 0;
                    
                    while( tm != 5000 )
                    {
                        if(rt_rd == -1)
                        {
                            rt_rd = 0;
                            chr += rt_rd;
                        }
                        printf("\nRT_RD = %d",rt_rd);
                        //printf("\nstr_rd+chr = %d",(int)str_rd+chr);
                        rt_rd =  read(fd,str_rd+strlen(str_rd),225);
                        //tcflush(fd,TCIOFLUSH);
                        if( (strstr(str_rd,"OK\r\n") ) || (strstr(str_rd,"ERROR\r\n") ) )
                        {
                            printf("Breaking\n");
                            break;
                        }
                        tm += 50;                        
                        msleep(50);
                        printf("time out = %d\n",tm);
                    }
                    printf("\n - - - from UART-%d : \n",u);
                    for(int i=0;i<strlen(str_rd);i++)
                    {
                        if( (str_rd[i]=='\n') || (str_rd[i]=='\r') )
                        {
                            printf("'0x%x'",str_rd[i]);
                        }
                        else
                        printf("%c",str_rd[i]);
                    }
                    printf("\n--------\n");
                                      
                break;

                case 1: //Modem terminal
                    memset(str_wr,0,strlen(str_wr));
                    printf("\nEnter at commands : ");
                    scanf("%s",str_wr);
                    ii = strlen(str_wr);
                    str_wr[ii-1] = '\r';
                    str_wr[ii+1] = 0;
                    printf("\nstr_wr = %s\n",str_wr);
                    rt_rd = write(fd,str_wr,strlen(str_wr));
                    printf("rt_wr = %d\n",rt_rd);
                    memset(str_wr,0,strlen(str_wr));
                    
                    rt_rd = 0;
                    chr = 0;
                    printf("tm = %d\n",tm);
                    memset(str_rd,0,1024);
                    printf("OK = %d\n",(int)strstr(str_rd,"OK"));
                    printf("ERROR = %d\n",(int)strstr(str_rd,"ERROR"));
                    while( tm != 5000 )
                    {
                        if(rt_rd == -1)
                        {
                            rt_rd = 0;
                            chr += rt_rd;
                        }
                        printf("\nchr = %d\n",chr);
                        printf("\nstr_rd+chr = %d\n",(int)str_rd+chr);
                        rt_rd =  read(fd,str_rd+chr,30);
                        if( (strstr(str_rd,"OK") ) || (strstr(str_rd,"ERROR") ) )
                        {
                            printf("Breaking\n");
                            break;
                        }
                        tm += 500;
                        printf("time out = %d\n",tm);
                        msleep(500);
                    }
                    rt_rd = -1;
                    tm = 0;
                    chr=0;
                    
                    printf("rt_rd = %d\n",rt_rd);                    
                    printf("from UART-%d : %s\n\r",u,str_rd);

                    memset(str_rd,0,1024);
                    if(tcflush(fd,TCIFLUSH) != 0)
                    {
                        perror("Unable to fush tty buffer due to ");
                    }
                    else
                    {
                        printf("tty I buffer has been flushed\n");
                    }
                break;
                case 2: //Write only
                    printf("Writing to UART-%d <%s>\n\r",u,str_wr1);
                    printf("Writing delay = 2sec\n\r");
                    #ifdef C2C
                        usart_write(fd,str_wr,band_dly);
                    #else
                        write(fd,str_wr,strlen(str_wr));
                    #endif
                    printf("Writing completed - %d\n\r",i);
                break;
                case 3: // read - write                    
                    printf("\nWriting to UART-%d <%s>",u,str_wr1);
                    write(fd,str_wr,strlen(str_wr));
                    read(fd,str_rd,(strlen(str_wr)));
                    printf("\nfrom UART-%d : %s",u,str_rd);
                    memset(str_rd,0,1024);                    
                break;
            }
            printf("\n\rcharacter delay = %d\n",band_dly);
            printf("String delay = %d\n",str_dl);
            sleep(str_dl);
        }        
        close(fd);
        printf("\n");
    }
}



