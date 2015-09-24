#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <libpic30.h>

#include "lcd.h"
#include "led.h"
#include "flexmotor.h"
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
int trigger_cur;

float set_x = 1700;
float kp=0, kd=0, ki=0;
float x_cur=0,x_prev=0;
float dx=0, ix=0;
float prev_error=0;
float error=0;
float output=0;
int pulse_width;

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    x_cur = touch_adc();
    pid_controller();

    lcd_locate(0,3);
    lcd_printf("P_X=%.2f\r", x_cur);
    lcd_locate(0,4);
    lcd_printf("D_X=%.2f\r", dx);
    lcd_locate(0,5);
    lcd_printf("I_X=%.2f\r", ix);
    lcd_locate(0,6);
    lcd_printf("F_X=%.2f\r", output);
    lcd_locate(0,7);
    lcd_printf("pw=%4d\r", pulse_width);

    IFS0bits.T1IF = 0; // clear the interrupt flag
}

//void __attribute__((__interrupt__)) _T2Interrupt(void)
//{
//    IFS0bits.T2IF = 0; // clear the interrupt flag
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

void pid_controller() {
    error = x_cur - set_x;
    dx = (error - prev_error)/0.05;
    ix += error*.05;
    output = kp*(error) + ki*ix + kd*dx;

    pulse_width = ((2100-900)/(float)(kp*3200)) * (-1*output) + 1500;

    if (pulse_width < 900) {
        pulse_width = 900;
    } else if (pulse_width > 2100) {
        pulse_width = 2100;
    }
    motor_set_duty(1, pulse_width);
    prev_error = error;
    x_prev = x_cur;
}


void main(){
	//Init LCD
	__C30_UART=1;	
	lcd_initialize();
	lcd_clear();

//        // timer 1
        CLEARBIT(T1CONbits.TON);;
        CLEARBIT(T1CONbits.TCS); // Select internal instruction cycle clock
        CLEARBIT(T1CONbits.TGATE); // Disable Gated Timer mode
        TMR1 = 0x00; // Clear timer register
        T1CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
        PR1 = 10000; // Load the period value
        IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
        CLEARBIT(IFS0bits.T1IF); // Clear Timer1 Interrupt Flag
        SETBIT(IEC0bits.T1IE); // Enable Timer1 interrupt
        // 4000= 20*10^-3 * 12.8*10^6 * 1/64


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

        motor_init(1);
        motor_set_duty(0, 900);

        touch_init();
        touch_select_dim(0);

        lcd_clear();
        
        SETBIT(T1CONbits.TON); // Start Timer
        
        kp = .21;
        ki = 0.01;
        kd = .04;

        lcd_locate(0, 0);
        lcd_printf("Kp=%.2f   Kd=%.2f\nKi=%.3f\nSet_x=%.2f", kp, kd, ki,set_x);

        while(1);
}

