#ifndef __DEV_DT_STRUCT
#define __DEV_DT_STRUCT

#include <linux/semaphore.h>
#include <linux/cdev.h>

#define GSM_BANK      1
#define DEV_NAME    "gsm"
#define CLASS       "gsm-class"

struct GS_mdm
{
    char mk[20];
    int typ;
    int sts;
};

struct scull_qset 
{
    void **data;
    struct scull_qset *next;
};

struct scull_dev 
{
    struct scull_qset *data; /* Pointer to first quantum set */
    int quantum;    /* the current quantum size */
    
    int qset;   /* the current array size */
    
    unsigned long size; /* amount of data stored here */
    
    unsigned int access_key; /* used by sculluid and scullpriv */
    
    struct semaphore sem;   /* mutual exclusion semaphore   */
    
    struct cdev cdev;   /* Char device structure*/
    
};

#endif