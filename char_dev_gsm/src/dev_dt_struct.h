#ifndef __DEV_DT_STRUCT
#define __DEV_DT_STRUCT

#include <linux/semaphore.h>
#include <linux/cdev.h>

#define GSM_BANK      1
#define DEV_NAME    "modem"
#define CLASS       "modem-class"
#define DEV_DBG     "MDM" // Apear in the kernel debugging.

#define RBT_Q(gen) ("rbt=Quectel,"#gen)
#define RBT_T(gen) ("rbt=Telit,"#gen)

#define MDM_Q   "Quectel"
#define MDM_T   "Telit"


#define DBG_NON 0
#define DBG_LOW 1
#define DBG_MED 2
#define DBG_MAX 3

#define DBG_LVL     DBG_LOW

struct GS_mdm
{
    char mk[15];
    uint8_t typ;
    uint8_t sts;
    uint8_t sts_tmp;
    uint8_t rbt;
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

extern struct EGS_SEM
{
    struct semaphore sem;
}EGS_sem[3];


#endif