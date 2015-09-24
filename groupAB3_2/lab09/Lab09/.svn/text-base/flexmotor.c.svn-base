#include "flexmotor.h"

void motor_init() {
    //setup Timer 2
    CLEARBIT(TRISDbits.TRISD7); /* Set OC8 as output */
    CLEARBIT(TRISDbits.TRISD6); /* Set OC7 as output */
    CLEARBIT(T2CONbits.TON); // Disable Timer
    CLEARBIT(T2CONbits.TCS); // Select internal instruction cycle clock
    CLEARBIT(T2CONbits.TGATE); // Disable Gated Timer mode
    TMR2 = 0x00; // Clear timer register
    T2CONbits.TCKPS = 0b10; // Select 1:64 Prescaler
    CLEARBIT(IFS0bits.T2IF); // Clear Timer2 interrupt status flag
    CLEARBIT(IEC0bits.T2IE); // Disable Timer2 interrupt enable control bit
    PR2 = 4000; // Set timer period 20ms:
    // 4000= 20*10^-3 * 12.8*10^6 * 1/64

    OC8R = ((20000-1500)/5.0); /* Set the initial duty cycle to 5ms*/
    OC7R = ((20000-1500)/5.0); /* Set the initial duty cycle*/

    OC8CON = 0x0006; /* Set OC8: PWM, no fault check, Timer2 */
    OC7CON = 0x0006; /* Set OC7: PWM, no fault check, Timer2 */
        SETBIT(T2CONbits.TON); /* Turn Timer 2 on */
}

void motor_set_duty(uint8_t chan, uint16_t duty_us) {
    if (chan == 1) {
        OC8RS = ((20000-duty_us)/5.0); /* Load OCRS: next pwm duty cycle */
    } else { 
        OC7RS = ((20000-duty_us)/5.0); /* Load OCRS: next pwm duty cycle */
    }
}
