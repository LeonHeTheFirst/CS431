#include "flexserial.h"

int timer_flag = 1;

void __attribute__((__interrupt__)) _T1Interrupt(void)
{
    timer_flag = 0;
    IFS0bits.T1IF = 0; // clear the interrupt flag
}

void uart2_init(uint16_t baud) {
    /* Stop UART port */
    CLEARBIT(U2MODEbits.UARTEN); //Disable UART for configuration
    /* Disable Interrupts */
    IEC1bits.U2RXIE = 0;
    IEC1bits.U2TXIE = 0;
    /* Clear Interrupt flag bits */
    IFS1bits.U2RXIF = 0;
    IFS1bits.U2TXIF = 0;
    /* Set IO pins */
    //TRISFbits.TRISF2 = 1; //set as input UART1 RX pin
    //TRISFbits.TRISF3 = 0; //set as output UART1 TX pin
    /* baud rate */
    // use the following equation to compute the proper
    // setting for a specific baud rate
    U2MODEbits.BRGH = 0; //Set low speed baud rate
    U2BRG = (uint32_t)800000 / baud -1; //Set the baudrate to be at baud
    /* Operation settings and start port */
    U2MODE = 0; // 8-bit, no parity and, 1 stop bit
    U2MODEbits.RTSMD = 0; //select simplex mode
    U2MODEbits.UEN = 0; //select simplex mode
    U2MODE |= 0x00;
    U2MODEbits.UARTEN = 1; //enable UART
    U2STA = 0;
    U2STAbits.UTXEN = 1; //enable UART TX

    // enable timer
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
}

int uart2_putc(uint8_t c) {
    while (U2STAbits.UTXBF);
    U2TXREG = c;
    while(!U2STAbits.TRMT);
}

uint16_t uart2_getc() {
    TMR1 = 0x00;
    timer_flag = 1;
    T1CONbits.TON = 1;// Start Timer
    while(timer_flag) {
        if (U2STAbits.OERR) {
            U2STAbits.OERR = 0;
        }
        if (U2STAbits.URXDA) {
            T1CONbits.TON = 0;
            return U2RXREG & 0x00FF;
        }
    }
    return 0xFF00;
}