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
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*/

#define  DISP_CMD            0x7C  		/* signify the start of a command								*/
#define  DISP_CMD_CLS		 0x00		/* clears display and returns cursor home                       */	
#define	 DISP_CMD_SET_COL	 0x18		/* sets the cursor position to specific col						*/
#define	 DISP_CMD_SET_ROW	 0x19		/* sets the cursor position to specific row						*/

/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  DispCursorSet(CPU_INT08U row, CPU_INT08U col);

/*
*********************************************************************************************************
*                                      DISPLAY DRIVER INITIALIZATION
*
* Description : This function initializes the display driver.
* Arguments   : None.
* Returns     : None.
* Notes       : - DispInit() MUST be called only when multitasking has started.  This is because
*                 DispInit() requires time delay services from the operating system.
*               - DispInit() MUST only be called once during initialization.
*********************************************************************************************************
*/

void  DispInit (void)
{
    DispInitOS();                      /* Initialize the RTOS services                                 */
    DispInitPort();                    /* Initialize I/O ports used in display driver                  */
    DispDly_uS(2000);                  /* Delay at least  2 mS                                         */
}

/*
*********************************************************************************************************
*                                            CLEAR THE SCREEN
*
* Description : This function clears the display
* Arguments   : none
* Returns     : none
*********************************************************************************************************
*/

void  DispClrScr (void)
{
    DispLock();
	DispDataWr(DISP_CMD);
    DispDataWr(DISP_CMD_CLS);          /* Send command to LCD display to clear the display             */
    DispDly_uS(2000);                  /* Delay at least  2 mS                                         */
    DispUnlock();
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                     POSITION THE CURSOR (Internal)
*
* Description : This function positions the cursor into the LCD buffer
* Arguments   : 'row'   is the row    position of the cursor in the LCD Display
*                       'row' can be a value from 0 to 'DispMaxRows - 1'
*               'col'   is the column position of the cursor in the LCD Display
*                       'col' can be a value from 0 to 'DispMaxCols - 1'
* Returns     : none
*********************************************************************************************************
*/

static  void  DispCursorSet (CPU_INT08U row, CPU_INT08U col)
{
	DispDataWr(DISP_CMD);
    DispDataWr(DISP_CMD_SET_ROW);
	DispDataWr(row);
	DispDataWr(DISP_CMD);
    DispDataWr(DISP_CMD_SET_COL);
	DispDataWr(col);
}

/*
*********************************************************************************************************
*                                         DISPLAY AN ASCII STRING
*
* Description : This function is used to display an ASCII string on a line of the LCD display
* Arguments   : 'row'   is the row    position of the cursor in the LCD Display
*                       'row' can be a value from 0 to 'DispMaxRows - 1'
*               'col'   is the column position of the cursor in the LCD Display
*                       'col' can be a value from 0 to 'DispMaxCols - 1'
*               's'     is a pointer to the string to write to the display at
*                       the desired row/col.
* Returns     : none
*********************************************************************************************************
*/

void  DispStr (CPU_INT08U row, CPU_INT08U col, CPU_INT08U *s)
{
    DispLock();
    DispCursorSet(row*10, col*6);       /* Position cursor at ROW/COL                              */
    while (*s) {     					/* Write all chars within str                              */
    	DispDataWr(*s++);               /* Send character to LCD display                           */
    }
    DispUnlock();
}
