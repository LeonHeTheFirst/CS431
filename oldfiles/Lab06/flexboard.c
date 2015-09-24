#include "flexboard.h"

void touch_init() {
    CLEARBIT(AD1CON1bits.ADON);
    //initialize PIN for x axis
 
    SETBIT(TRISBbits.TRISB15);
    CLEARBIT(AD1PCFGLbits.PCFG15);

    // init pin for y axis
    SETBIT(TRISBbits.TRISB9);
    CLEARBIT(AD1PCFGLbits.PCFG9);


    //Configure AD1CON1
    SETBIT(AD1CON1bits.AD12B); //set 12b Operation Mode
    AD1CON1bits.FORM = 0; //set integer output
    AD1CON1bits.SSRC = 0x7; //set automatic conversion
    //Configure AD1CON2
    AD1CON2 = 0; //not using scanning sampling
    //Configure AD1CON3
    CLEARBIT(AD1CON3bits.ADRC); //internal clock source
    AD1CON3bits.SAMC = 0x1F; //sample-to-conversion clock = 31Tad
    AD1CON3bits.ADCS = 0x2; //Tad = 3Tcy (Time cycles)
    //Leave AD1CON4 at its default value
    //enable ADC
    SETBIT(AD1CON1bits.ADON);

    CLEARBIT(TRISEbits.TRISE1); //I/O pin set to output
    CLEARBIT(TRISEbits.TRISE2); //I/O pin set to output
    CLEARBIT(TRISEbits.TRISE3); //I/O pin set to output

}
void touch_select_dim(uint8_t dim) {
    if (dim == 0) {
        // x dim
        CLEARBIT(LATEbits.LATE1);
        SETBIT(LATEbits.LATE2);
        SETBIT(LATEbits.LATE3);
        AD1CHS0bits.CH0SA = 0xF;
    } else {
        // ydim
        SETBIT(LATEbits.LATE1);
        CLEARBIT(LATEbits.LATE2);
        CLEARBIT(LATEbits.LATE3);
        AD1CHS0bits.CH0SA = 0x9;
    }
}
uint16_t touch_adc() {
    SETBIT(AD1CON1bits.SAMP); //start to sample
    while(!AD1CON1bits.DONE); //wait for conversion to finish
    CLEARBIT(AD1CON1bits.DONE); //MUST HAVE! clear conversion done bit
    return ADC1BUF0; //return sample
}