/*
*********************************************************************************************************
*                                             Microchip dsPIC33FJ
*                                            Board Support Package
*
*                                                   Micrium
*                                    (c) Copyright 2005, Micrium, Weston, FL
*                                              All Rights Reserved
*
*
* File : bsp_lcd.c
* By   : Eric Shufro
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                              VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                 DispInitPort()
*
* Description : DispInitPort() is responsible for initializing the hardware used to interface with the
*               LCD module. DispInitPort() is called by DispInit().
*
* Arguments   : None
*
* Callers     : DispInit() from lcd.c
*
* Notes       : none.
*********************************************************************************************************
*/

void  DispInitPort (void)
{
//	OpenUART1(0x8000,0x400,6);
	/* Stop UART port */
	U1MODEbits.UARTEN = 0; //Disable UART for configuration

	/* Disable Interrupts */
	IEC0bits.U1RXIE = 0;
	IEC0bits.U1TXIE = 0;

	/* Clear Interrupt flag bits */
	IFS0bits.U1RXIF = 0;
	IFS0bits.U1TXIF = 0;

	/* Set IO pins */
	TRISFbits.TRISF2 = 1; //set as input UART1 RX pin
	TRISFbits.TRISF3 = 0; //set as output UART1 TX pin

	/* baud rate */
	U1MODEbits.BRGH = 0; // set baud rate to 115200
	U1BRG = 6;

	/* Operation settings and start port */
	U1MODE = 0;
	U1MODEbits.UARTEN = 1; 	//enable UART
	U1STA = 0;
	U1STAbits.UTXEN = 1; 	// enable transmit

    DispDly_uS(20000);                  /* Delay at least  20 mS                                        */
}

/*
*********************************************************************************************************
*                                   DispDataWr()
*
* Description : DispDataWr() is used to write a single byte to the LCD module. Depending on the state
*               of the RS line, the byte will be either sent to the data (RS is 1) or control register
*               (RS is 0) of the LCD controller.
*
* Arguments   : data is the byte value to write to the LCD controller module. The destination ‘inside’
*               the HD44780 or compatible controller depends on whether RS is high or low.
*
* Callers     : Various from lcd.c
*
* Notes       : 1) RS is determined prior to this function call. Therefore, software must
*                  not overwrite the current state of this bit.
*********************************************************************************************************
*/

void  DispDataWr (CPU_INT08U data)
{
	while (U1STAbits.UTXBF);
	U1TXREG = data;
	while (!U1STAbits.TRMT);
}


/*
*********************************************************************************************************
*                                 uC/LCD Delay Functionality
*
* Description : DispDly_uS() allows the code to delay for the execution for a certain amount of time to
*               allow the data to ‘stabilize’ and for the HD44780 to complete it’s operation. The delay
*               is specified in microseconds (uS). This operation is easy to perform if you have a
*               real-time kernel such as uC/OS-II.
*
* Arguments   : us determines the amount of delay (in microseconds)
*
* Callers     : lcd.c: DispInit(), DispClrScr()
*********************************************************************************************************
*/

void  DispDly_uS (CPU_INT32U us)
{
    CPU_INT32U  us_per_tick;
    CPU_INT32U  ticks;
							

    us_per_tick =  1000000L / OS_TICKS_PER_SEC;
    ticks       =  us / us_per_tick + 1;
    OSTimeDly(ticks);
}


