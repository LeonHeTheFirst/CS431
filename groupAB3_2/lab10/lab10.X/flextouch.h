/* 
 * File:   flexboard.h
 * Author: npathur2
 *
 * Created on March 12, 2015, 4:55 PM
 */

#ifndef FLEXBOARD_H
#define	FLEXBOARD_H

#include "types.h"
#include <libpic30.h>
#include <p33Fxxxx.h>

void touch_init();
void touch_select_dim(uint8_t dim);
uint16_t touch_adc();

#endif	/* FLEXBOARD_H */

