#include "lcd.h"
#include "types.h"
#include "led.h"
#include "flexmotor.h"
#include "flextouch.h"
#include "performance.h"
#include "math.h"

#include <p33Fxxxx.h>

#define FCY 12800000UL

// control task frequency (Hz)
#define RT_FREQ 50

//setpoint parameters
#define SPEED 0.08  // tested up to .12!
#define RADIUS 350
#define CENTER_X 1650
#define CENTER_Y 1350

// Servo defines
#define MAX_DUTY_MICROSEC 2100
#define MIN_DUTY_MICROSEC 900
#define SERVO_PWM_PERIOD_MICROSEC 20000
#define INIT_DUTYX_MICROSEC 1410    // leveled plate on X axis: 1410
#define INIT_DUTYY_MICROSEC 1400     // leveled plate on Y axis: 1400

// Touch screen definitions
#define X_DIM 0
#define Y_DIM 1

uint16_t TOUCH_MIN_X = 300;
uint16_t TOUCH_MAX_X = 3100;
uint16_t TOUCH_MIN_Y = 400;
uint16_t TOUCH_MAX_Y = 2600;

// do not change position of this include
#include <libpic30.h>

/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT);

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection
_FGS(GCP_OFF);

// control setpoint
double Xpos_set = 1650.0, Ypos_set = 1550.0;

// raw, unfiltered X and Y position of the ball
volatile uint16_t Xpos, Ypos;
volatile uint8_t start = 0;
volatile uint8_t select = X_DIM;
volatile uint8_t deadline_miss = 0;


uint16_t x_prev=0;
float dx=0, ix=0;
float prev_error_x=0;
float error_x=0;
float output_x=0;
uint16_t pulse_width_x;

uint16_t y_prev=0;
float dy=0, iy=0;
float prev_error_y=0;
float error_y=0;
float output_y=0;
uint16_t pulse_width_y;

float kp_x=.21, kd_x=.15, ki_x=0;
float kp_y=.21, kd_y=.15, ki_y=0;
int x_scale = .21*5200;
int y_scale = .21*5800;

void init() {
    kp_x=.31;
    kd_x=.22;
    ki_x=.005;

    kp_y=.31;
    kd_y=.22;
    ki_y=.005;

    x_scale = kp_x*6200;
    y_scale = kp_y*6800;
}

uint16_t pidX_controller() {
  error_x = Xpos - Xpos_set;
    dx = (error_x - prev_error_x)/0.05;
    ix += error_x*.05;
    output_x = kp_x*(error_x) + ki_x*ix + kd_x*dx;

    pulse_width_x = ((2100-900)/(float)(x_scale)) * (-1*output_x) + 1400;

    if (pulse_width_x < 900) {
        pulse_width_x = 900;
    } else if (pulse_width_x > 2100) {
        pulse_width_x = 2100;
    }

    prev_error_x = error_x;
    x_prev = Xpos;

    return pulse_width_x;
}


uint16_t pidY_controller() {
    error_y = Ypos - Ypos_set;
    dy = (error_y - prev_error_y)/0.05;
    iy += error_y*.05;
    output_y = kp_y*(error_y) + ki_y*iy + kd_y*dy;

    pulse_width_y = ((2100-900)/(float)(y_scale)) * (-1*output_y) + 1420;

    if (pulse_width_y < 900) {
        pulse_width_y = 900;
    } else if (pulse_width_y > 2100) {
        pulse_width_y = 2100;
    }

    prev_error_y = error_y;
    y_prev = Ypos;
    return pulse_width_y;
}


// Configure the real-time task timer and its interrupt.
void timers_initialize() {

  //Set Timer1 to generate an interrupt every 10ms (100Hz) ==> PR1=500
  CLEARBIT(T1CONbits.TON); //Disable Timer
  CLEARBIT(T1CONbits.TCS); //Select internal instruction cycle clock
  CLEARBIT(T1CONbits.TGATE); //Disable Gated Timer mode
  T1CONbits.TCKPS = 0b11; //Select 1:256 Prescaler
  PR1 = 500; //Load the period value ==> running at 100Hz now!
  TMR1 = 0x00; //Clear timer register
  IPC0bits.T1IP = 0x01; // Set Timer1 Interrupt Priority Level
  CLEARBIT(IFS0bits.T1IF); // Clear Timer1 Interrupt Flag
  SETBIT(IEC0bits.T1IE); // Enable Timer1 interrupt
  SETBIT(T1CONbits.TON); // Start Timer
}

int main(){
  uint8_t start_r, old_IPL;
  uint8_t hz50_scaler, hz5_scaler, hz1_scaler, sec;
  uint32_t tick = 0;

  hz50_scaler = hz5_scaler = hz1_scaler = sec = 0;

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

  init();
  touch_init();

  //__delay_ms(200);
  lcd_initialize();             // Initialize the LCD 

  motor_init();

  lcd_clear();
  lcd_locate(0,0);
  lcd_printf("-- Ball position: --");

  timers_initialize();          // Initialize timers

  while (1) {
    start_r = 0;
    while(!start_r) {      
      // disable all maskable interrupts
      SET_AND_SAVE_CPU_IPL(old_IPL, 7);
      start_r = start;

      // enable all maskable interrupts
      RESTORE_CPU_IPL(old_IPL);
    }

    // Periodic real-time task code starts here!!!
    double pidX, pidY;
    uint16_t duty_us_x, duty_us_y;

    // 50Hz control task
    if(hz50_scaler == 0) {
      calcQEI(Xpos_set, Xpos, Ypos_set, Ypos);

      Xpos_set = CENTER_X + RADIUS * cos(tick * SPEED);
      Ypos_set = CENTER_Y + RADIUS * sin(tick * SPEED);
      tick++;

      pidX_controller();
      pidY_controller();

         

      // setMotorDuty is a wrapper function that calls your motor_set_duty
      // implementation in flexmotor.c. The 2nd parameter expects a value
      // between 900-2100 us
      setMotorDuty(MOTOR_X_CHAN, pulse_width_x);
      setMotorDuty(MOTOR_Y_CHAN, pulse_width_y);
      //OC8RS = ((20000-duty_us_x)/5.0);
      //OC7RS = ((20000-duty_us_y)/5.0);
    }

    // 5Hz display task
    if(hz5_scaler == 0) {
//      lcd_locate(0,1);
//      lcd_printf("Xf(t)=%4u", Xpos);
//      lcd_locate(0,2);
//      lcd_printf("Yf(t)=%4u", Ypos);
//      lcd_locate(0,3);
//      lcd_printf("Yout=%4u", pulse_width_y);

      if(deadline_miss >= 1) {
        lcd_locate(0,6);
        lcd_printf("%4d d_misses!!!", deadline_miss);
      }

     // lcd_locate(0,3);
      //   lcd_printf("Yout=%4f", output_y);
    }

    // 1Hz seconds display task
    if(hz1_scaler == 0) {
      lcd_locate(0,7);
      lcd_printf("QEI: %5u", getQEI());
      sec++;
    }
    
    hz50_scaler = (hz50_scaler + 1) % 2;
    hz5_scaler = (hz5_scaler + 1) % 20;
    hz1_scaler = (hz1_scaler + 1) % 100;

    start = 0;
  }

  return 0;
}

uint16_t median(uint16_t a, uint16_t b, uint16_t c, uint16_t d, uint16_t e)
{
    return b < a ? d < c ? b < d ? a < e ? a < d ? e < d ? e : d
                                                 : c < a ? c : a
                                         : e < d ? a < d ? a : d
                                                 : c < e ? c : e
                                 : c < e ? b < c ? a < c ? a : c
                                                 : e < b ? e : b
                                         : b < e ? a < e ? a : e
                                                 : c < b ? c : b
                         : b < c ? a < e ? a < c ? e < c ? e : c
                                                 : d < a ? d : a
                                         : e < c ? a < c ? a : c
                                                 : d < e ? d : e
                                 : d < e ? b < d ? a < d ? a : d
                                                 : e < b ? e : b
                                         : b < e ? a < e ? a : e
                                                 : d < b ? d : b
                 : d < c ? a < d ? b < e ? b < d ? e < d ? e : d
                                                 : c < b ? c : b
                                         : e < d ? b < d ? b : d
                                                 : c < e ? c : e
                                 : c < e ? a < c ? b < c ? b : c
                                                 : e < a ? e : a
                                         : a < e ? b < e ? b : e
                                                 : c < a ? c : a
                         : a < c ? b < e ? b < c ? e < c ? e : c
                                                 : d < b ? d : b
                                         : e < c ? b < c ? b : c
                                                 : d < e ? d : e
                                 : d < e ? a < d ? b < d ? b : d
                                                 : e < a ? e : a
                                         : a < e ? b < e ? b : e
                                                 : d < a ? d : a;
}


// This ISR will execute whenever Timer1 has a compare match.
// it kicks off the periodic execution of user code and performs I/O
// Min period: 10msec due to X,Y switch time for touchscreen
void __attribute__((interrupt, auto_psv)) _T1Interrupt(void) {
  IFS0bits.T1IF = 0; // clear interrupt flag

  if(start == 1)
    deadline_miss++;

  if (select == X_DIM) {
    Xpos = median(touch_adc(), touch_adc(), touch_adc(), touch_adc(), touch_adc());
//    if (Xpos > 3100) {
//        Xpos = 3100;
//    } else if (Xpos < 300) {
//        Xpos = 300;
//    }
    touch_select_dim(Y_DIM);
    select = Y_DIM;
  }
  else {
    Ypos = median(touch_adc(), touch_adc(), touch_adc(), touch_adc(), touch_adc());
//    if (Ypos > 2700) {
//        Ypos = 2700;
//    } else if (Ypos < 400) {
//        Ypos = 400;
//    }
    touch_select_dim(X_DIM);
    select = X_DIM;
  }

  start = 1;
}

