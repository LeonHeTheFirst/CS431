#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>

#include "lcd.h"
#include "led.h"
#include "flexboard.h"

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF);
volatile int lock;
uint16_t xs[5];
uint16_t ys[5];

void __attribute__((__interrupt__)) _T1Interrupt(void)
{ 
    CLEARBIT(T1CONbits.TON); // Start Timer
    lock = 0;
    IFS0bits.T1IF = 0; // clear the interrupt flag
}

int cmpfunc (const void * a, const void * b)
{
   return ( *(uint16_t*)a - *(uint16_t*)b );
}

void main(){
	//Init LCD
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();
        touch_init();

        CLEARBIT(T1CONbits.TON); // Disable Timer
        CLEARBIT(T1CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T1CONbits.TGATE); // Disable Gated Timer mode
        TMR1 = 0x00; // Clear timer register
        T1CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
        PR1 = 2000; // Load the period value
        IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
        CLEARBIT(IFS0bits.T1IF); // Clear Timer1 Interrupt Flag
        SETBIT(IEC0bits.T1IE); // Enable Timer1 interrupt



        while(1) {
            int i = 0;

            
            
            touch_select_dim(0);
            lock = 1;
            TMR1 = 0x00;
            SETBIT(T1CONbits.TON); // Start Timer
            while(lock);
            
            for (i = 0; i < 5; i++) {
                xs[i] = touch_adc();
            }

            // read y
            touch_select_dim(1);
            lock = 1;
            TMR1 = 0x00;
            SETBIT(T1CONbits.TON); // Start Timer
            while(lock);
            for (i = 0; i < 5; i++) {
                ys[i] = touch_adc();
            }

            

            qsort(xs, 5, sizeof(uint16_t), cmpfunc);
            qsort(ys, 5, sizeof(uint16_t), cmpfunc);

            lcd_locate(0,0);
            lcd_printf("x position:             ");
            lcd_locate(0,0);
            lcd_printf("x position: %d", xs[2]);
            lcd_locate(0,1);
            lcd_printf("y position:             ");
            lcd_locate(0,1);
            lcd_printf("y position: %d", ys[2]);
        }
}

