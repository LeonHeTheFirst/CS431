#include <p33Fxxxx.h>
//do not change the order of the following 3 definitions
#define FCY 12800000UL 
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>

#include "lcd.h"
#include "led.h"
#include "crc16.h"
#include "flexserial.h"


/* Initial configuration by EE */
// Primary (XT, HS, EC) Oscillator with PLL
_FOSCSEL(FNOSC_PRIPLL);

// OSC2 Pin Function: OSC2 is Clock Output - Primary Oscillator Mode: XT Crystal
_FOSC(OSCIOFNC_OFF & POSCMD_XT); 

// Watchdog Timer Enabled/disabled by user software
_FWDT(FWDTEN_OFF);

// Disable Code Protection

_FGS(GCP_OFF);

int failed = 0;

void main(){
	//Init LCD
	__C30_UART=1;
	lcd_initialize();
	lcd_clear();
	lcd_locate(0,0);
        led_initialize();
        
        uart2_init(9600);

     
        while(1){
            uint16_t c = uart2_getc();
            if (c == 0xFF00) {
                continue;
            } else {
                uint16_t crc1 = uart2_getc();
                uint16_t crc2 = uart2_getc();
                uint16_t sent_crc = ((crc1 << 8) & 0xFF00) | crc2;

                if (crc1 == 0xFF00 || crc2==0xFF00) {
                    failed++;
                    uart2_putc(0);
                    continue;
                }

                uint16_t length = uart2_getc();
                if (length == 0xFF00) {
                    failed++;
                    uart2_putc(0);
                    continue;
                }

                int i;
                char *buffer = malloc(length+1);
                uint16_t calc_crc = 0;
                // read message
                int asdf = 0;
                for (i=0; i < length; i++) {
                    uint16_t b = uart2_getc();
                    if (b == 0xFF00) {
                        asdf = 1;
                        break;
                    }
                    buffer[i] = b;
                    calc_crc = crc_update(calc_crc, buffer[i]);
                }

                T1CONbits.TON = 0;
                buffer[i] = '\0';

               
                // check crc
                if (calc_crc != sent_crc) {
                    free(buffer);
                    failed++;
                    uart2_putc(0);
                } else {
                    lcd_clear();
                    lcd_locate(0,0);
                    lcd_printf("Failed: %d\r", failed);
                    lcd_printf("%s\r", buffer);
                    free(buffer);
                    failed = 0;
                    uart2_putc(1);
                }
                
            }
	}
}

