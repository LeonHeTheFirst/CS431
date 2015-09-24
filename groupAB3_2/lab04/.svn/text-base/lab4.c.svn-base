#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include "linuxanalog.h"

float v1;
float v2;

int alternate = 0;

void handler_function(int signum)
{
	if (alternate = !alternate) {
		dac(((v1+5)/10) * 4095);
	} else {
		dac(((v2+5)/10) * 4095);
	}
}

int setup_signal_handler() {
	struct sigaction action;
	// Ensure that the entire structure is zeroed out.
	memset(&action, 0, sizeof(action));
	// Set the sa_handler member to point to the desired handler function.
	action.sa_handler = handler_function;
	// Call sigaction to change the action taken upon receipt of the SIGINT signal.
	if (sigaction(SIGALRM, &action, NULL) != 0)
	{
		// If there is an error, print out a message and exit.
		perror("sigaction");
		return 1;
	}
	return 0;
}

int main() {
	

	printf("Enter first voltage (-5V to 5V): \n");
	scanf("%f", &v1);
	printf("%f\n", v1);

	printf("Enter second voltage (-5V to 5V): \n");
	scanf("%f", &v2);
	printf("%f\n", v2);

	struct timespec res;
	clock_getres(CLOCK_REALTIME, &res);
	double max_freq = 1/(2*(res.tv_nsec/1000000000.0));
	printf("Max frequency %f\n", max_freq);

	float freq;
	printf("Enter sqaure wave frequency (0 to %f): \n", max_freq);
	scanf("%f", &freq);

	if ((v1 > 5 || v1 < -5) || (v2 > 5 || v2 < -5) || freq < 0 || freq > max_freq) {
		printf("invalid input\n");
		return 1;
	}

	das1602_initialize();

	if (setup_signal_handler()) {
		return 1;
	}

	timer_t timer1;
	// Create a new timer that will send the default SIGALRM signal.
	if (timer_create(CLOCK_REALTIME, NULL, &timer1) != 0)
	{
		// If there is an error, print out a message and exit.
		perror("timer_create");
		return 1;
	}

	struct itimerspec timer1_time;
	// The it_value member sets the time until the timer first goes off (2.5 seconds).
	// The it_interval member sets the period of the timer after it first goes off (100 ms).
	timer1_time.it_value.tv_sec = 2; // 2 seconds
	timer1_time.it_value.tv_nsec = 500000000; // 0.5 seconds (5e8 nanoseconds)

	float period = 1/(freq);

	timer1_time.it_interval.tv_sec = (int)period; // 0 seconds
	timer1_time.it_interval.tv_nsec = (period-((int)period)) * 1000000000; // 100 milliseconds (1e8 nanoseconds)
	// Schedule the timer.
	if (timer_settime(timer1, 0, &timer1_time, NULL) != 0)
	{
		// If there is an error, print out a message and exit.
		perror("timer_settime");
		return 1;
	}

	while(1);


}