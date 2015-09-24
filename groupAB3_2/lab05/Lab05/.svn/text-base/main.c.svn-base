#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>

#include "lcd.h"
#include "led.h"
#include "flexmotor.h"

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
//void __attribute__((__interrupt__)) _T1Interrupt(void)
//{
//    if(trigger_cur != PORTEbits.RE8)
//    {
//         global_counter++; // Increment a global counter
//         if(global_counter == 5)
//         {
//             trigger_cur ^= 1;
//             global_counter = 0;
//
//             if (trigger_cur == 0) {
//                 clicked++;
//                 lcd_locate(0,3);
//                 lcd_printf("%d\r", clicked);
//                 lcd_printf("%x\r", clicked);
//             }
//         }
//    }
//
//    IFS0bits.T1IF = 0; // clear the interrupt flag
//}

int sample_adc_x() {
    AD2CHS0bits.CH0SA = 0x04; //set ADC to Sample AN20 pin
    SETBIT(AD2CON1bits.SAMP); //start to sample
    while(!AD2CON1bits.DONE); //wait for conversion to finish
    CLEARBIT(AD2CON1bits.DONE); //MUST HAVE! clear conversion done bit
    return ADC2BUF0; //return sample
}

int sample_adc_y() {
    AD2CHS0bits.CH0SA = 0x05; //set ADC to Sample AN20 pin
    SETBIT(AD2CON1bits.SAMP); //start to sample
    while(!AD2CON1bits.DONE); //wait for conversion to finish
    CLEARBIT(AD2CON1bits.DONE); //MUST HAVE! clear conversion done bit
    return ADC2BUF0; //return sample
}

void main(){
	//Init LCD
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();

//        // led
//        CLEARBIT(T1CONbits.TON);
//        CLEARBIT(T1CONbits.TCS); // Select internal instruction cycle clock
//        CLEARBIT(T1CONbits.TGATE); // Disable Gated Timer mode
//        TMR1 = 0x00; // Clear timer register
//        T1CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
//        PR1 = 200; // Load the period value
//        IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
//        CLEARBIT(IFS0bits.T1IF); // Clear Timer1 Interrupt Flag
//        SETBIT(IEC0bits.T1IE); // Enable Timer1 interrupt
//        SETBIT(T1CONbits.TON); // Start Timer
	
        // enable button 1
        SETBIT(AD1PCFGHbits.PCFG20);
        SETBIT(TRISEbits.TRISE8);

        //disable ADC
        CLEARBIT(AD2CON1bits.ADON);
        //initialize PIN for x axis
        SETBIT(TRISBbits.TRISB4); //set TRISE RE8 to input
        CLEARBIT(AD2PCFGLbits.PCFG4); //set AD1 AN20 input pin as analog
        // init pin for y axis
        SETBIT(TRISBbits.TRISB5); //set TRISE RE8 to input
        CLEARBIT(AD2PCFGLbits.PCFG5); //set AD1 AN20 input pin as analog

        //Configure AD1CON1
        CLEARBIT(AD2CON1bits.AD12B); //set 10b Operation Mode
        AD2CON1bits.FORM = 0; //set integer output
        AD2CON1bits.SSRC = 0x7; //set automatic conversion
        //Configure AD1CON2
        AD2CON2 = 0; //not using scanning sampling
        //Configure AD1CON3
        CLEARBIT(AD2CON3bits.ADRC); //internal clock source
        AD2CON3bits.SAMC = 0x1F; //sample-to-conversion clock = 31Tad
        AD2CON3bits.ADCS = 0x2; //Tad = 3Tcy (Time cycles)
        //Leave AD1CON4 at its default value
        //enable ADC
        SETBIT(AD2CON1bits.ADON);

        trigger_cur = 1;
        lcd_clear();
        lcd_printf("max x: ");

        int counter = 0;
        int xmax = 0;
        while(1){
            xmax = sample_adc_x();
            lcd_locate(8, 0);
            lcd_printf("%d", xmax);
            if (counter > 0) {
                counter++;
                if (counter % 10 == 0) {
                    if (PORTEbits.RE8 == 0) {
                        break;
                    } else {
                        counter = 0;
                    }
                }
            }
            else if (PORTEbits.RE8 == 0) {
                counter++;
            }
	}

        while(PORTEbits.RE8 == 0);

        lcd_locate(0,1);
        lcd_printf("min x: ");
        counter = 0;
        int xmin = 0;
        while(1){
            xmin = sample_adc_x();
            lcd_locate(8, 1);
            lcd_printf("%d", xmin);
            if (counter > 0) {
                counter++;
                if (counter % 10 == 0) {
                    if (PORTEbits.RE8 == 0) {
                        break;
                    } else {
                        counter = 0;
                    }
                }
            }
            else if (PORTEbits.RE8 == 0) {
                counter++;
            }
	}

        while(PORTEbits.RE8 == 0);

        lcd_locate(0,2);
        lcd_printf("max y: ");
        counter = 0;
        int ymax = 0;
        while(1){
            ymax = sample_adc_y();
            lcd_locate(8, 2);
            lcd_printf("%d", ymax);
            if (counter > 0) {
                counter++;
                if (counter % 10 == 0) {
                    if (PORTEbits.RE8 == 0) {
                        break;
                    } else {
                        counter = 0;
                    }
                }
            }
            else if (PORTEbits.RE8 == 0) {
                counter++;
            }
	}

        while(PORTEbits.RE8 == 0);

        lcd_locate(0,3);
        lcd_printf("min y: ");
        counter = 0;
        int ymin = 0;
        while(1){
            ymin = sample_adc_y();
            lcd_locate(8, 3);
            lcd_printf("%d", ymin);
            if (counter > 0) {
                counter++;
                if (counter % 10 == 0) {
                    if (PORTEbits.RE8 == 0) {
                        break;
                    } else {
                        counter = 0;
                    }
                }
            }
            else if (PORTEbits.RE8 == 0) {
                counter++;
            }
	}
        while(PORTEbits.RE8 == 0); // wait until release

        motor_init(1);
        lcd_locate(0,4);
        lcd_printf("pulse width x: ");
        counter = 0;
        int pulse_width = 0;
        int servo_counter = 0;
        while(1){
            pulse_width = ((2100-900)/(float)(xmax-xmin)) * (sample_adc_x()-xmin) + 900;
            if (pulse_width < 900) {
                pulse_width = 900;
            } else if (pulse_width > 2100) {
                pulse_width = 2100;
            }

            lcd_locate(15, 4);
            lcd_printf("%4d", pulse_width);
            
            servo_counter++;
            if (servo_counter % 25 == 0) {
                motor_set_duty(1, pulse_width);
                servo_counter = 0;
            }

            
            
            if (counter > 0) {
                counter++;
                if (counter % 10 == 0) {
                    if (PORTEbits.RE8 == 0) {
                        break;
                    } else {
                        counter = 0;
                    }
                }
            }
            else if (PORTEbits.RE8 == 0) {
                counter++;
            }
	}

        while(PORTEbits.RE8 == 0);


        lcd_locate(0,5);
        lcd_printf("pulse width y: ");
        counter = 0;
        pulse_width = 0;
        servo_counter = 0;
        while(1){
            pulse_width = ((2100-900)/(float)(ymax-ymin)) * (sample_adc_y()-ymin) + 900;
            if (pulse_width < 900) {
                pulse_width = 900;
            } else if (pulse_width > 2100) {
                pulse_width = 2100;
            }

            lcd_locate(15, 5);
            lcd_printf("%4d", pulse_width);

            servo_counter++;
            if (servo_counter % 25 == 0) {
                motor_set_duty(0, pulse_width);
                servo_counter = 0;
            }



            if (counter > 0) {
                counter++;
                if (counter % 10 == 0) {
                    if (PORTEbits.RE8 == 0) {
                        break;
                    } else {
                        counter = 0;
                    }
                }
            }
            else if (PORTEbits.RE8 == 0) {
                counter++;
            }
	}

        while(PORTEbits.RE8 == 0);

        

        while(1);
}

