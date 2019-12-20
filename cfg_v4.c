/*
    Config the V4 board for set and to perform the memory operations.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SET ("sudo sam-ba -p usb -d sama5d2 -a bootconfig -c writecfg:bureg0:0x60FF7 -c writecfg:bscr:0x4")

int main(void)
{
    printf("------------Wellcome to iSense-V4 -----------\n");
    printf("Supporting operations : \n");
    printf("\t( 0 )  SET");
    printf("\nEnter : ");
    int op;
    scanf("%d",&op);
    if(op == 0)
      {  //system(SET);
        int op=(int)0012;
        printf("%d\n",(int)op);}
    else
    {
        printf("\nERROR : Invalid argument\n.");
    }
    
}
