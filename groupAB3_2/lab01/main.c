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

void main(){
	//Init LCD
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
	lcd_locate(0,0);
	lcd_printf("James\r");
        lcd_printf("Leon\r");
        lcd_printf("Nihal\r");
        lcd_locate(0,3);
        lcd_printf("%d\r", clicked);
        lcd_printf("%x\r", clicked);

        led_initialize();

        // led
        CLEARBIT(T1CONbits.TON);
        CLEARBIT(T1CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T1CONbits.TGATE); // Disable Gated Timer mode
        TMR1 = 0x00; // Clear timer register
        T1CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
        PR1 = 200; // Load the period value
        IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
        CLEARBIT(IFS0bits.T1IF); // Clear Timer1 Interrupt Flag
        SETBIT(IEC0bits.T1IE); // Enable Timer1 interrupt
        SETBIT(T1CONbits.TON); // Start Timer
	
        // enable button 1
        SETBIT(AD1PCFGHbits.PCFG20);
        SETBIT(TRISEbits.TRISE8);
        trigger_cur = 1;
        while(1){
            LED4_PORT ^= 1;
            if (trigger_cur == 0) {
                LED1_PORT = 1;
            } else {
                LED1_PORT = 0;
            }

            if (PORTDbits.RD10 == 0) {
                LED2_PORT = 1;
            } else {
                LED2_PORT = 0;
            }

            if (PORTDbits.RD10 == trigger_cur) {
                LED3_PORT = 0;
            } else {
                LED3_PORT = 1;
            }
	}
}

