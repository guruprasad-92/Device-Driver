// check the link :
//https://www.includehelp.com/c-programs/c-program-print-weekday-of-date.aspx
/*C program to validate date and print weekday of given date.*/
#include <stdio.h>
#include <math.h>
 
/*
 * function: validateDate
 * arguments: d- day, m- month, y- year
 * return type: 0 - invalid, 1 - valid
 * */
int validateDate(int d,int m,int y)
{
    //check year validity
    if(y>=1800 && y<=2999)
    {
        //check month validity
        if(m>=1 && m<=12)
        {
            //check day validity
            if(d>=1 && d<=31)
            {
                if( (d>=1 && d<=30) && (m==4||m==6||m==9||m==11))
                    return 1;   //valid date
                else if((d>=1 && d<=30) && (m==1||m==3||m==5||m==7||m==8||m==10||m==12))
                    return 1;   //valid date
                else if((d>=1 && d<=28) && (m==2))
                    return 1;   //valid date
                else if(d==29 && m==2 && ((y%400==0)||(y%4==0 && y%4!=0)))
                    return 1;   //valid date
                else
                    return 0;   //invalid day
            }
            else
            {
                return 0;   //day is invalid
            }
        }
        else
        {
            return 0; //month is invalid
        }
    }
    else
    {
        return 0; //year is invalid
    }
}
// This function will return week day number from 0 to 6
int wd(int year, int month, int day) 
{
    int wday=0;
    wday=(day  + ((153 * (month + 12 * ((14 - month) / 12) - 3) + 2) / 5) \
        + (365 * (year + 4800 - ((14 - month) / 12)))              \
        + ((year + 4800 - ((14 - month) / 12)) / 4)                \
        - ((year + 4800 - ((14 - month) / 12)) / 100)              \
        + ((year + 4800 - ((14 - month) / 12)) / 400)              \
        - 32045                                                    \
      );
      printf("\nwday = %d\n",wday);
     return wday%7;   
}

//----- Century codes --------

int get_century_code(int yr)
{
    int ret = 0;
    int a = 0;
    switch (yr)
    {
        case 1900 ... 1999 :
            ret = 0;
        break;
        case 2000 ... 2099 :
            ret = 6;
        break;
        case 2100 ... 2199 :
            ret = 4;
        break;
        case 2200 ... 2299 :
            ret = 2;
        break;
        default :
            ret = -1;
        break;
    }
    return ret;
}

int get_fst2(int no)
{
    int arr[4];
    for(int i=0;i<4;i++)
    {
        arr[i] = no - 
    }
}

int mn_code[] = {1,4,4,0,2,5,0,3,6,1,4,6};
//Start day = saturday = 0
 
int main()
{
/*
    int day,month,year;
    int wDayNo=0;
    char dayNames[][12]={"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};
 
    //input date 
    printf("Input date (DD-MM-YYYY): ");
    scanf("%d-%d-%d",&day,&month,&year);
     
    //check date is correct or not
    if(validateDate(day,month,year)==1){
        printf("Date is correct [%02d/%02d/%02d].\n",day,month,year);
        //get weekday number
        wDayNo=wd(year,month,day);
        //print weekday according to wDayNo
        printf("week day is: %s\n",dayNames[wDayNo]);
    }
    else
        printf("Date is in-correct.\n");        
    //return 0;

    int zy=0;
   for(int month=1;month<13;month++)
   {
       zy = (month + 12 * ((14 - month) / 12) - 3);
       printf("\ni=%d : %d",month,(153 * zy + 2 ));
   }
s*/
printf("%d\n",123 >> 4);
}