/*
*********************************************************************************************************
*                                               uC/LCD
*                                         LCD Module Driver
*
*                              (c) Copyright 2005; Micrium; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  DISP_MODULE_PRESENT        1

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void  DispInit();
void  DispStr(CPU_INT08U row, CPU_INT08U col, CPU_INT08U *s);
void  DispClrScr(void);


//void  DispChar(CPU_INT08U row, CPU_INT08U col, CPU_INT08U c);
//void  DispClrLine(CPU_INT08U line);

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*                                            HARDWARE SPECIFIC
*********************************************************************************************************
*/

void  DispInitPort(void);
void  DispDataWr(CPU_INT08U data);

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*                                               OS SPECIFIC
*********************************************************************************************************
*/

void  DispInitOS(void);                       /* Initialize RTOS services                              */
void  DispLock(void);                         /* Ensure  exclusive access to the display               */
void  DispUnlock(void);                       /* Release exclusive access to the display               */
void  DispDly_uS (CPU_INT32U us);
