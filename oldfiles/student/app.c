#include <includes.h>


/*
*********************************************************************************************************
*                                                CONSTANTS
*********************************************************************************************************
*/

// control task frequency (Hz)
#define RT_FREQ 50

//setpoint parameters
#define SPEED 0.08  // tested up to .12!
#define RADIUS 350
#define CENTER_X 1650
#define CENTER_Y 1350

#define X_DIM 0
#define Y_DIM 1

#define APP_TASK_LCD_PRIO       5
#define APP_TASK_PID_PRIO       4
#define APP_TASK_TOUCH_PRIO       3
#define APP_TASK_LCD_STK_SIZE   512
#define APP_TASK_PID_STK_SIZE   512
#define APP_TASK_TOUCH_STK_SIZE   512

/*
*********************************************************************************************************
*                                                VARIABLES
*********************************************************************************************************
*/

OS_STK  AppStartTaskStk[APP_TASK_START_STK_SIZE];
OS_STK  AppLCDTaskStk[APP_TASK_LCD_STK_SIZE];
OS_STK  AppPIDTaskStk[APP_TASK_PID_STK_SIZE];
OS_STK  AppTouchTaskStk[APP_TASK_TOUCH_STK_SIZE];

// control setpoint
// control setpoint
double Xpos_set = 1650.0, Ypos_set = 1550.0;

// raw, unfiltered X and Y position of the ball
volatile double Xpos, Ypos;
volatile int start = 0;
volatile int select = X_DIM;
volatile int deadline_miss = 0;


float x_prev=0;
float dx=0, ix=0;
float prev_error_x=0;
float error_x=0;
float output_x=0;
int pulse_width_x;

float y_prev=0;
float dy=0, iy=0;
float prev_error_y=0;
float error_y=0;
float output_y=0;
int pulse_width_y;

float kp_x=.21, kd_x=.12, ki_x=0;
float kp_y=.21, kd_y=.12, ki_y=0;
int x_scale = .21*5200;
int y_scale = .21*5800;

// filtered X and Y position of the ball
CPU_INT16U Xposf, Yposf;

/*
*********************************************************************************************************
*                                            FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppStartTask(void *p_arg);
static  void  AppLCDTask(void *p_arg);
static  void  AppPIDTask(void *p_arg);
static  void  AppTouchTask(void *p_arg);
static  void  AppTaskCreate(void);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.
* Arguments   : none
*********************************************************************************************************
*/

CPU_INT16S  main (void)
{
    CPU_INT08U  err;

    BSP_IntDisAll();                                                    /* Disable all interrupts until we are ready to accept them */
    OSInit();                                                           /* Initialize "uC/OS-II, The Real-Time Kernel"              */

    OSTaskCreateExt(AppStartTask,                                       /* Create the start-up task for system initialization       */
                    (void *)0,
                    (OS_STK *)&AppStartTaskStk[0],
                    APP_TASK_START_PRIO,
                    APP_TASK_START_PRIO,
                    (OS_STK *)&AppStartTaskStk[APP_TASK_START_STK_SIZE-1],
                    APP_TASK_START_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &err);

    OSStart();                                                          /* Start multitasking (i.e. give control to uC/OS-II)       */
	return (-1);                                                        /* Return an error - This line of code is unreachable       */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppStartTask()' by 'OSTaskCreate()'.
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*               2) Interrupts are enabled once the task start because the I-bit of the CCR register was
*                  set to 0 by 'OSTaskCreate()'.
*********************************************************************************************************
*/

static  void  AppStartTask (void *p_arg)
{
	(void)p_arg;
	
    BSP_Init();                                                         /* Initialize BSP functions                                 */
    OSStatInit();                                                       /* Determine CPU capacity                                   */
    DispInit();

    kp_x=.31;
    kd_x=.22;
    ki_x=.005;

    kp_y=.31;
    kd_y=.22;
    ki_y=.005;

    x_scale = kp_x*6200;
    y_scale = kp_y*6800;
    
    
    // TODO initialize touchscreen and motors
    touch_init();
    motor_init();

    AppTaskCreate();                                                    /* Create additional user tasks                             */

    while (DEF_TRUE) {
	    OSTimeDlyHMSM(0, 0, 5, 0);
    }
}


/*
*********************************************************************************************************
*                              CREATE ADDITIONAL APPLICATION TASKS
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
    // Create LCD task
    OSTaskCreateExt(AppLCDTask,
            (void*)0,
            (OS_STK*) & AppLCDTaskStk[0],
            APP_TASK_LCD_PRIO,
            APP_TASK_LCD_PRIO,
            (OS_STK*) & AppLCDTaskStk[APP_TASK_LCD_STK_SIZE-1],
            APP_TASK_LCD_STK_SIZE,
            (void*)0,
            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(AppPIDTask,
            (void*)0,
            (OS_STK*) & AppPIDTaskStk[0],
            APP_TASK_PID_PRIO,
            APP_TASK_PID_PRIO,
            (OS_STK*) & AppPIDTaskStk[APP_TASK_PID_STK_SIZE-1],
            APP_TASK_PID_STK_SIZE,
            (void*)0,
            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskCreateExt(AppTouchTask,
            (void*)0,
            (OS_STK*) & AppTouchTaskStk[0],
            APP_TASK_TOUCH_PRIO,
            APP_TASK_TOUCH_PRIO,
            (OS_STK*) & AppTouchTaskStk[APP_TASK_TOUCH_STK_SIZE-1],
            APP_TASK_TOUCH_STK_SIZE,
            (void*)0,
            OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	// TODO create tasks
}

static void AppLCDTask (void *p_arg)
{
    CPU_INT08U reset_string[50];
    CPU_INT08U xpos_string[50];
    CPU_INT08U ypos_string[50];
    (void)p_arg;
    unsigned int i = 0;
    int led = 1;
    DispClrScr();
    
    while (1) {
        OSTimeDlyHMSM(0, 0, 1, 0);
        DispClrScr();
        DispStr(0, 0, "Group 3\n");
        sprintf(reset_string, "Uptime: %d seconds\n", i++);
        sprintf(xpos_string, "X-pos: %d", Xposf);
        sprintf(ypos_string, "Y-pos: %d", Yposf);

        DispStr(1, 0, reset_string);
        DispStr(2, 0, xpos_string);
        DispStr(3, 0, ypos_string);

        LED_Off(led);
        led = (led==5 ? 1 : led+1);
        LED_On(led);
    }
}

static  void  AppPIDTask(void *p_arg) {
    (void)p_arg;

    int tick = 0;

    while (1) {
        OSTimeDlyHMSM(0, 0, 0, 50);

        Xpos_set = CENTER_X + RADIUS * cos(tick * SPEED);
        Ypos_set = CENTER_Y + RADIUS * sin(tick * SPEED);
        tick++;

        // x PID
        error_x = Xposf - Xpos_set;
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
        x_prev = Xposf;

        // y PID
        error_y = Yposf - Ypos_set;
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
        y_prev = Yposf;

        motor_set_duty(MOTOR_X_CHAN, pulse_width_x);
        motor_set_duty(MOTOR_Y_CHAN, pulse_width_y);
    } 
}

#define NZEROS 1
#define NPOLES 1
#define GAIN   6.242183581e+00

static  void  AppTouchTask(void *p_arg) {
    (void)p_arg;

    float xx[NZEROS+1], yx[NPOLES+1];
    float xy[NZEROS+1], yy[NPOLES+1];

//    xx[0] = 1500;
//    xx[1] = 1500;
//    xx[2] = 1500;
//    xx[3] = 1500;
//    xy[0] = 1500;
//    xy[1] = 1500;
//    xy[2] = 1500;
//    xy[3] = 1500;
//    yx[0] = 1500;
//    yx[1] = 1500;
//    yx[2] = 1500;
//    yx[3] = 1500;
//    yy[0] = 1500;
//    yy[1] = 1500;
//    yy[2] = 1500;
//    yy[3] = 1500;

    while (1) {
        OSTimeDlyHMSM(0, 0, 0, 10);

        if (select == X_DIM) {
            Xpos = touch_adc();

          xx[0] = xx[1];
        xx[1] = Xpos / GAIN;
        yx[0] = yx[1];
        yx[1] =   (xx[0] + xx[1])
                     + (  0.6795992982 * yx[0]);
        Xposf = yx[1];
            //Xposf = Xpos;

            touch_select_dim(Y_DIM);
            select = Y_DIM;
        } else {
            Ypos = touch_adc();
            xy[0] = xy[1];
        xy[1] = Ypos / GAIN;
        yy[0] = yy[1];
        yy[1] =   (xy[0] + xy[1])
                     + (  0.6795992982 * yy[0]);
        Yposf = yy[1];
            //Yposf = Ypos;

            touch_select_dim(X_DIM);
            select = X_DIM;
        }
    }
}