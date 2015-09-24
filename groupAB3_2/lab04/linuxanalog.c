#include "linuxanalog.h"
#include <stdio.h>
#include <sys/io.h>

#define BADR0 0xCCC0
#define BADR1 0xCC40
#define BADR2 0xCC60
#define BADR3 0xCC80
#define BADR4 0xCCA0

void das1602_initialize() {
	outw(0x0027, BADR1+8);
	outw(0, BADR4+2);
}

void dac(int value) {
    outw(value, BADR4);
}
