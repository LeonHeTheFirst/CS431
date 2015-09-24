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

// control task frequency (Hz)
#define RT_FREQ 50

#define X_MIN_BOARD   300
#define X_MAX_BOARD   3100
#define Y_MIN_BOARD   400
#define Y_MAX_BOARD   2600

// control setpoint
double Xpos_set = 1650.0, Ypos_set = 1550.0;

volatile uint8_t start = 0;
volatile uint8_t deadline_miss = 0;

float set_x = 1700;
float kp_x=0, kd_x=0, ki_x=0;
float x_cur=0,x_prev=0;
float dx=0, ix=0;
float prev_error_x=0;
float error_x=0;
float output_x=0;
int pulse_width_x;

float set_y = 1550;
float kp_y=0, kd_y=0, ki_y=0;
float y_cur=0,y_prev=0;
float dy=0, iy=0;
float prev_error_y=0;
float error_y=0;
float output_y=0;
int pulse_width_y;

int xy_flag = 0;

/* Motor X-axis number of duty cycle ticks*/
volatile  uint16_t Xduty_ticks;
/* Motor Y-axis number of duty cycle ticks*/
volatile  uint16_t Yduty_ticks;

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

double filterX(double x)
{
  // low pass digital filter for X axis
}

double filterY(double y)
{
  // low pass digital filter for Y axis
}

double pidX_controller() {
  error_x = x_cur - set_x;
    dx = (error_x - prev_error_x)/0.05;
    ix += error_x*.05;
    output_x = kp_x*(error_x) + ki_x*ix + kd_x*dx;

    pulse_width_x = ((2100-900)/(float)(kp_x*4200)) * (-1*output_x) + 1400;

    if (pulse_width_x < 900) {
        pulse_width_x = 900;
    } else if (pulse_width_x > 2100) {
        pulse_width_x = 2100;
    }
    //motor_set_duty(1, pulse_width_x);
    prev_error_x = error_x;
    x_prev = x_cur;

    return pulse_width_x;

}

double pidY_controller() {
    error_y = y_cur - set_y;
    dy = (error_y - prev_error_y)/0.05;
    iy += error_y*.05;
    output_y = kp_y*(error_y) + ki_y*iy + kd_y*dy;

    pulse_width_y = ((2100-900)/(float)(kp_y*5200)) * (-1*output_y) + 1340;

    if (pulse_width_y < 900) {
        pulse_width_y = 900;
    } else if (pulse_width_y > 2100) {
        pulse_width_y = 2100;
    }
    //motor_set_duty(0, pulse_width_y);
    prev_error_y = error_y;
    y_prev = y_cur;
    return pulse_width_y;
}


// This function accepts as input an actuation command between (-100, +100);
// it converts actuation percentage to number of duty cycle ticks
// void motor_set_percentage(uint8_t chan, double x){

//   /* number of duty cycle ticks*/
//   uint16_t duty_ticks;

//   // compute duty cycle ticks

//   //saturate actuation command if necessary!
//   if(duty_ticks > MAX_DUTY_MICROSEC)
//     duty_ticks = MAX_DUTY_MICROSEC;
//   else if(duty_ticks < MIN_DUTY_MICROSEC)
//     duty_ticks = MIN_DUTY_MICROSEC;

//   // more computation...

//   switch(chan) {
//   case MOTOR_X_CHAN:
//     Xduty_ticks = duty_ticks; /* preload Xduty_ticks: next pwm duty cycle */
//     break;
//   case MOTOR_Y_CHAN:
//     Yduty_ticks = duty_ticks; /* Load Yduty_ticks: next pwm duty cycle */
//     break;
//   }
// }

// Configure the real-time task timer and its interrupt.
void timers_initialize() {
  //Set Timer1 to generate an interrupt every 10ms (100Hz) ==> PR1=500
  CLEARBIT(T1CONbits.TON);;
  CLEARBIT(T1CONbits.TCS); // Select internal instruction cycle clock
  CLEARBIT(T1CONbits.TGATE); // Disable Gated Timer mode
  TMR1 = 0x00; // Clear timer register
  T1CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
  PR1 = 2000; // Load the period value
  IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
  CLEARBIT(IFS0bits.T1IF); // Clear Timer1 Interrupt Flag
  SETBIT(IEC0bits.T1IE); // Enable Timer1 interrupt
  // 4000= 20*10^-3 * 12.8*10^6 * 1/64
}

void main(){

  __C30_UART=1;
  lcd_initialize();
  lcd_clear();

  uint8_t start_r, old_IPL;
  uint8_t hz50_scaler, hz5_scaler;
  uint32_t tick = 0;

  // raw, unfiltered X and Y position of the ball
  uint16_t Xpos, Ypos;

  // filtered X and Y position of the ball
  double Xposf, Yposf;

  double pidX, pidY;

  hz50_scaler = hz5_scaler = 0;


  touch_init();
  touch_select_dim(0);               // selext X-axis for touchscreen sampling

  // Initialization code
  // init motor
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




  // BUTTON STUFF
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


        lcd_clear();
        lcd_printf("max x: ");

        int counter = 0;
        int xmax = 0;
        while(1){
            xmax = sample_adc_x();
            lcd_locate(8, 0);
            lcd_printf("%4d", xmax);
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
            lcd_printf("%4d", xmin);
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
            lcd_printf("%4d", ymax);
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
            lcd_printf("%4d", ymin);
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

  timers_initialize();

  // init joystick
  //disable ADC
//        CLEARBIT(AD2CON1bits.ADON);
//        //initialize PIN for x axis
//        SETBIT(TRISBbits.TRISB4); //set TRISE RE8 to input
//        CLEARBIT(AD2PCFGLbits.PCFG4); //set AD1 AN20 input pin as analog
//        // init pin for y axis
//        SETBIT(TRISBbits.TRISB5); //set TRISE RE8 to input
//        CLEARBIT(AD2PCFGLbits.PCFG5); //set AD1 AN20 input pin as analog



        kp_x = .21;
        //ki_x = 0.01;
        kd_x = .06;

        kp_y = .21;
        //ki_y = 0.01;
        kd_y = .02;


        
        SETBIT(T1CONbits.TON); // Start Timer

counter = 0;
int trigger_pressed = 0;
  while(1) {

    start_r = 0;
    while(!start_r) {
      // disable all maskable interrupts
      SET_AND_SAVE_CPU_IPL(old_IPL, 7);
      start_r = start;

      // enable all maskable interrupts
      RESTORE_CPU_IPL(old_IPL);
    }

    // Periodic real-time task code starts here!!!

    // Sample ADC channel for touch screen
    uint16_t sample = touch_adc();
    int joystick_sample_x = (X_MAX_BOARD - X_MIN_BOARD) * (sample_adc_x()-(float)xmin) / (float)((float)xmax-(float)xmin) + X_MIN_BOARD;
    int joystick_sample_y = (Y_MAX_BOARD - Y_MIN_BOARD) * (sample_adc_y()-(float)ymin) / (float)((float)ymax-(float)ymin) + Y_MIN_BOARD;

    if(xy_flag==0){
      x_cur = sample;

      // digital filtering X coordinate at 50Hz
      //Xposf = filterX(Xpos);

      // switch channel
      touch_select_dim(1);
      xy_flag = 1;
    }
    else {
      y_cur = sample;

      // digital filtering Y coordinate at 50Hz
      //Yposf = filterY(Ypos);

      // switch channel
      touch_select_dim(0);
      xy_flag = 0;
    }


    // 50Hz control task
    if(hz50_scaler == 0)
      {
	// generate set point trajectory
        // Xpos_set = set_x + RADIUS * cos(tick * SPEED);
        // Ypos_set = set_y + RADIUS * sin(tick * SPEED);
        tick++;

        // PID controllers
        Xduty_ticks = pidX_controller();
        Yduty_ticks = pidY_controller();
//
        lcd_locate(0, 0);
        lcd_printf("pos: %4.1f, %4.1f\n", x_cur, y_cur);
        lcd_printf("set: %4.1f, %4.1f\n", set_x, set_y);
        lcd_printf("stick: %4d, %4d\n", joystick_sample_x, joystick_sample_y);
        lcd_printf("x:%.3f,%.3f,%.3f\n", kp_x, kd_x, ki_x);
        lcd_printf("y:%.3f,%.3f,%.3f\n", kp_y, kd_y, ki_y);
        lcd_printf("kp, kd, ki\n");

        if (trigger_pressed == 1) {
            if (PORTEbits.RE8 == 0) {
                set_x = joystick_sample_x;
                set_y =  joystick_sample_y;
            }
            trigger_pressed = 2;
        }
        if (PORTEbits.RE8 == 0 && trigger_pressed == 0) {
            trigger_pressed = 1;
        } else if (PORTEbits.RE8 == 1 && trigger_pressed == 2) {
            trigger_pressed = 0;
        }
      }


    // 5Hz display task
 //    if(hz5_scaler == 0) {
 //      // 5Hz activities

 //      //You need to make sure that the control code has completed within 10ms deadline
 //      // if deadline is missed, print the number of times deadline is missed here:
 //      if(deadline_miss >= 1) {
	// lcd_locate(0,6);
	// lcd_printf("%4d d_misses!!!", deadline_miss);
 //      }
 //    }

    hz50_scaler = (hz50_scaler + 1)%2;
    hz5_scaler = (hz5_scaler + 1)%20;
    start = 0;
  }
}


// This ISR will execute whenever Timer1 has a compare match.
// it kicks off the periodic execution of user code and performs Output
void __attribute__((interrupt)) _T1Interrupt(void) {



  // Output to the servos
  //OC8RS = (uint16_t)Xduty_ticks; /* Load OCRS: next pwm duty cycle */
    OC8RS = ((20000-Xduty_ticks)/5.0);
    OC7RS = ((20000-Yduty_ticks)/5.0);

  //You need to make sure that the control code has completed within 10ms.
  // add the code neccessary for checking missed deadline
  // Deadline misses are *BAD* for this code! figure out why.
  start = 1;
  IFS0bits.T1IF = 0; // clear interrupt flag
}
