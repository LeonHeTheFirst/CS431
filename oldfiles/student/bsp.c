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
* File : BSP.C
* By   : Eric Shufro
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                       MPLAB CONFIGURATION MACROS
*********************************************************************************************************
*/

// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF);  


/*
*********************************************************************************************************
*                                              CONSTANTS
*********************************************************************************************************
*/
#define  LED1   0x010                      /* Port A pin 4                                             */
#define  LED2   0x020                      /* Port A pin 5                                             */
#define  LED3   0x200                      /* Port A pin 9                                             */
#define  LED4   0x400                      /* Port A pin 10                                            */
#define  LED5   0x001                      /* Port A pin 0                                             */
#define  LED_ALL	(LED1 | LED2 | LED3 | LED4 | LED5)

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

static  void  Tmr_TickInit(void);

/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/

void  BSP_Init (void)
{
    LED_Init();                                                         /* Initialize the I/Os for the LED controls                 */
    Tmr_TickInit();                                                     /* Initialize the uC/OS-II tick interrupt                   */
}

/*
*********************************************************************************************************
*                                      BSP_CPU_ClkFrq()

* Description : This function determines the CPU clock frequency (Fcy)
* Returns     : The CPU frequency in (HZ)
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFrq (void)
{
	return 12800000UL;
}

/*
*********************************************************************************************************
*                                     DISABLE ALL INTERRUPTS
*
* Description : This function disables all interrupts from the interrupt controller.
*
* Arguments   : none
*********************************************************************************************************
*/
void  BSP_IntDisAll (void)
{
}

/*
*********************************************************************************************************
*                                         LED I/O INITIALIZATION
*
* Description : This function initializes the I/O Pins used by the onboard LEDs
*
* Arguments   : none
*********************************************************************************************************
*/

void  LED_Init (void)
{
    TRISA     =  0;                          /* Set all PORTA pins to output                             */
    AD1PCFGH |= LED_ALL;                     /* Set LED pins from analog to digital I/O    				 */

    LED_Off(0);                              /* Shut off all LEDs                                        */
}

/*
*********************************************************************************************************
*                                             LED ON
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to control
*                      0    indicates that you want ALL the LEDs to be ON
*                      1    turns ON LED1
*                      2    turns ON LED2
*                      ...
*                      5    turns ON LED5
*
*********************************************************************************************************
*/

void  LED_On (CPU_INT08U led)
{
    if (led == 0) {
		PORTA |= LED_ALL;					/* Turn on all of the LEDs if a 0 is passed in              */
		return;
    }

    if (led == 1) {
		PORTA |= LED1;
	}
    else if (led == 2) {
		PORTA |= LED2;
	}
    else if (led == 3) {
		PORTA |= LED3;
	}
    else if (led == 4) {
		PORTA |= LED4;
	}
    else if (led == 5) {
		PORTA |= LED5;
	}
}

/*
*********************************************************************************************************
*                                             LED OFF
*
* Description : This function is used to control any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to turn OFF
*                      0    indicates that you want ALL the LEDs to be OFF
*                      1    turns OFF LED1
*                      2    turns OFF LED2
*                      .
*                      8    turns OFF LED8
*
* Notes       : 1) Jumper JP2 on the Explorer 16 board must be connected to enable the onboard LEDs
*********************************************************************************************************
*/

void  LED_Off (CPU_INT08U led)
{
    if (led == 0) {
 		PORTA &= ~LED_ALL;					/* Turn off all of the LEDs if a 0 is passed in              */
		return;
    }

    if (led == 1) {
		PORTA &= ~LED1;
	}
    else if (led == 2) {
		PORTA &= ~LED2;
	}
    else if (led == 3) {
		PORTA &= ~LED3;
	}
    else if (led == 4) {
		PORTA &= ~LED4;
	}
    else if (led == 5) {
		PORTA &= ~LED5;
	}
}

/*
*********************************************************************************************************
*                                             LED TOGGLE
*
* Description : This function is used to toggle any or all the LEDs on the board.
*
* Arguments   : led    is the number of the LED to control
*                      0    indicates that you want to toggle ALL the LEDs
*                      1    toggles LED1
*                      2    toggles LED2
*                      .
*                      5    toggles LED5
*
* Notes       : 1) Jumper JP2 on the Explorer 16 board must be connected to enable the onboard LEDs
*********************************************************************************************************
*/

void  LED_Toggle (CPU_INT08U led)
{
    if (led == 0) {
 		PORTA ^= LED_ALL;					 /* Toggle all of the LEDs if a 0 is passed in              */
		return;
    }

    if (led == 1) {
		PORTA ^= LED1;
	}
    else if (led == 2) {
		PORTA ^= LED2;
	}
    else if (led == 3) {
		PORTA ^= LED3;
	}
    else if (led == 4) {
		PORTA ^= LED4;
	}
    else if (led == 5) {
		PORTA ^= LED5;
	}
}

/*
*********************************************************************************************************
*                                   OSProbe_TmrInit()
*
* Description : This function is called to by uC/Probe Plug-In for uC/OS-II to initialize the
*               free running timer that is used to make time measurements.
*
* Arguments   : none
*
* Returns     : none
*
* Note(s)     : 1) This timer is shared with the uC/OS-II time tick and is initialized
*                  from Tmr_TickInit().
*********************************************************************************************************
*/

#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN == 1)
void  OSProbe_TmrInit (void)
{
#if OS_PROBE_TIMER_SEL == 3
    T3CON  =   0;                                                       /* Use Internal Osc (Fosc / 4), 16 bit mode, prescaler = 1  */
    TMR3   =   0;                                                       /* Start counting from 0 and clear the prescaler count      */
    PR3    =   0xFFFF;                                                  /* Set the period register to its maximum value             */
    T3CONbits.TON = 1;                                                  /* Start the timer                                          */
#endif

#if OS_PROBE_TIMER_SEL == 5
    T5CON  =   0;                                                       /* Use Internal Osc (Fosc / 4), 16 bit mode, prescaler = 1  */
    TMR5   =   0;                                                       /* Start counting from 0 and clear the prescaler count      */
    PR5    =   0xFFFF;                                                  /* Set the period register to its maximum value             */
    T5CONbits.TON = 1;                                                  /* Start the timer                                          */
#endif
}
#endif

/*
*********************************************************************************************************
*                                   OSProbe_TmrRd()
*
* Description : This function is called to read the current counts of a 16 bit free running timer.
*
* Arguments   : none
*
* Returns     ; The 16 bit count (in a 32 bit variable) of the timer assuming the timer is an UP counter.
*********************************************************************************************************
*/

#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN == 1)
CPU_INT32U  OSProbe_TmrRd (void)
{
#if OS_PROBE_TIMER_SEL == 3
    return ((CPU_INT32U)TMR3);                                          /* Return the value of timer 3 if selected                  */
#endif

#if OS_PROBE_TIMER_SEL == 5
    return ((CPU_INT32U)TMR5);                                          /* Return the value of timer 5 if selected                  */
#endif
}
#endif

/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
*
* Arguments   : none
*
* Note(s)     : 1) The timer operates at a frequency of Fosc / 4
*               2) The timer resets to 0 after period register match interrupt is generated
*********************************************************************************************************
*/

static  void  Tmr_TickInit (void)
{
    CPU_INT32U  tmr_frq;
    CPU_INT16U  cnts;


    tmr_frq   =   BSP_CPU_ClkFrq();                                     /* Get the CPU Clock Frequency (Hz) (Fcy)                   */
    cnts      =   (tmr_frq / OS_TICKS_PER_SEC) - 1;                     /* Calaculate the number of timer ticks between interrupts  */

#if BSP_OS_TMR_SEL == 2
    T2CON     =   0;                                                    /* Use Internal Osc (Fcy), 16 bit mode, prescaler = 1  		*/
    TMR2      =   0;                                                    /* Start counting from 0 and clear the prescaler count      */
    PR2       =   cnts;                                                 /* Set the period register                                  */
    IPC1bits.T2IP = 0;                                                  /* Clear all timer 2 interrupt priority bits                */
    IPC1     |=  (TIMER_INT_PRIO << 12);                                /* Set timer 2 to operate with an interrupt priority of 4   */
    IFS0bits.T2IF = 0;                                                  /* Clear the interrupt for timer 2                          */
    IEC0bits.T2IE = 1;                                                  /* Enable interrupts for timer 2                            */
    T2CONbits.TON = 1;                                                  /* Start the timer                                          */
#endif

#if BSP_OS_TMR_SEL == 4
    T4CON     =   0;                                                    /* Use Internal Osc (Fcy), 16 bit mode, prescaler = 1  		*/
    TMR4      =   0;                                                    /* Start counting from 0 and clear the prescaler count      */
    PR4       =   cnts;                                                 /* Set the period register                                  */
    IPC6bits.T4IP = 0;                                                 /* Clear all timer 4 interrupt priority bits                */
    IPC6     |=  (TIMER_INT_PRIO << 12);                                /* Set timer 4 to operate with an interrupt priority of 4   */
    IFS1bits.T4IF = 0;                                                  /* Clear the interrupt for timer 4                          */
    IEC1bits.T4IE = 1;                                                  /* Enable interrupts for timer 4                            */
    T4CONbits.TON = 1;                                                  /* Start the timer                                          */
#endif
}

/*
*********************************************************************************************************
*                                     OS TICK INTERRUPT SERVICE ROUTINE
*
* Description : This function handles the timer interrupt that is used to generate TICKs for uC/OS-II.
*********************************************************************************************************
*/

void OS_Tick_ISR_Handler (void)
{
#if  BSP_OS_TMR_SEL == 2
    IFS0bits.T2IF = 0;
#endif

#if  BSP_OS_TMR_SEL == 4
    IFS1bits.T4IF = 0;
#endif

    OSTimeTick();
}

