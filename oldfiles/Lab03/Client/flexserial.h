/* 
 * File:   flexserial.h
 * Author: npathur2
 *
 * Created on February 19, 2015, 5:06 PM
 */

#ifndef FLEXSERIAL_H
#define	FLEXSERIAL_H

#include <p33Fxxxx.h>
#include "types.h"
#include <stdio.h>
#include <libpic30.h>



void uart2_init(uint16_t baud);
int uart2_putc(uint8_t c);
uint16_t uart2_getc();


#endif	/* FLEXSERIAL_H */

