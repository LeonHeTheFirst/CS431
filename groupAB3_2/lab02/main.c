#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>

#include "lcd.h"
#include "led.h"

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection

_FGS(GCP_OFF);

/*
int trigger_cur;
int global_counter = 0;
int clicked = 0;

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    if(trigger_cur != PORTEbits.RE8)
    {
         global_counter++; // Increment a global counter
         if(global_counter == 5)
         {
             trigger_cur ^= 1;
             global_counter = 0;

             if (trigger_cur == 0) {
                 clicked++;
                 lcd_locate(0,3);
                 lcd_printf("%d\r", clicked);
                 lcd_printf("%x\r", clicked);
             }
         }
    }
   
    IFS0bits.T1IF = 0; // clear the interrupt flag
}
*/

uint8_t led1;
uint32_t globaltime;

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    LED2_PORT ^= 1;
    Nop();
    IFS0bits.T1IF = 0; // clear the interrupt flag
}

void __attribute__((__interrupt__)) _T2Interrupt(void)
{
    led1 ^= 1;
    globaltime++;
    if(led1){
        LED1_PORT ^= 1;
        Nop();
    }
    IFS0bits.T2IF = 0; // clear the interrupt flag
}

void __attribute__((__interrupt__)) _INT1Interrupt(void)
{
    globaltime = 0;
    IFS1bits.INT1IF = 0; // clear the interrupt flag
}

void main(){
	//Init LCD
	__C30_UART=1;
	lcd_initialize();
	lcd_clear();
	lcd_locate(0,0);
        globaltime = 0;
        led_initialize();

        //enable LPOSCEN
        __builtin_write_OSCCONL(OSCCONL | 2);
        T1CONbits.TON = 0; //Disable Timer
        T1CONbits.TCS = 1; //Select external clock
        T1CONbits.TSYNC = 0; //Disable Synchronization
        T1CONbits.TCKPS = 0b00; //Select 1:1 Prescaler
        TMR1 = 0x00; //Clear timer register
        PR1 = 32767; //Load the period value
        IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
        IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
        IEC0bits.T1IE = 1;// Enable Timer1 interrupt
        T1CONbits.TON = 1;// Start Timer

        // timer2
        CLEARBIT(T2CONbits.TON);
        CLEARBIT(T2CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
        TMR2 = 0x00; // Clear timer register
        T2CONbits.TCKPS = 0b11; // Select 1:256 Prescaler
        PR2 = 50; // Load the period value
        IPC1bits.T2IP = 0x02; // Set Timer2 Interrupt Priority Level
        CLEARBIT(IFS0bits.T2IF); // Clear Timer2 Interrupt Flag
        SETBIT(IEC0bits.T2IE); // Disable Timer2 interrupt !
        SETBIT(T2CONbits.TON); // Start Timer

        // timer3
        CLEARBIT(T3CONbits.TON);
        CLEARBIT(T3CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T3CONbits.TGATE); // Disable Gated Timer mode
        TMR3 = 0x00; // Clear timer register
        T3CONbits.TCKPS = 0b00; // Select 1:1 Prescaler
        PR3 = 0xffff; // Load the period value
        IPC2bits.T3IP = 0x03; // Set Timer3 Interrupt Priority Level
        CLEARBIT(IFS0bits.T3IF); // Clear Timer3 Interrupt Flag
        CLEARBIT(IEC0bits.T3IE); // Disable Timer3 interrupt !
        SETBIT(T3CONbits.TON); // Start Timer
	
        // enable button 1
        SETBIT(AD1PCFGHbits.PCFG20);
        SETBIT(TRISEbits.TRISE8);
        IEC1bits.INT1IE = 1;
        INTCON2bits.INT1EP = 1;
        IPC5bits.INT1IP = 0x01;

        int count = 0;
        int minutes = 0;
        int seconds = 0;
        int ms = 0;
        uint16_t prev_time = 0;

        while(1){
            TMR3 = 0x00;

            LED4_PORT ^= 1;
            count++;



            if (count % 2000 == 0) {
                count = 0;
                minutes = globaltime / 60000;
                seconds = (globaltime / 1000 ) % 60;
                ms = globaltime % 1000;
                lcd_locate(0,0);
                lcd_printf("%02d:%02d.%03d\r", minutes, seconds, ms);

                lcd_printf("%d\r", prev_time);
                lcd_printf("%.4f\r", prev_time/12800.0);

                prev_time = TMR3;
            }

	}
}

