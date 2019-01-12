/*
 * Name: Timer Demo(Police Lights)
 * Author: Marquez Jones
 * Desc: Basic Blink example using timer interrupts
 *       This program will toggle between the blue and red lights on
 *       the launchpad
 *
 */

//includes
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

//defines

//function prototypes
void InitTimer(void);
void InitGPIO(void);

//Interrupt Sub Routine Prototypes
//Note: a pointer to your custom ISR, must be passed into an
//      IntRegister function to set it as an ISR
void Timer0IntBlink(void);


//flags
uint8_t blink_flag = 0x00;


int main(void)
{
    //set system clock to 16MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //Run master interrupt enable
    IntMasterEnable();

    //timer configurations
    InitTimer();

    //gpio configurations
    InitGPIO();

    while(1){

        /*
         * In this program, I set each pin to toggle every 1 second
         * Meaning that each light will hold for 1 second and have a period of
         * 2 seconds.
         * Both lights will alternate with each other
         *
         * Recommendation:
         * Use the oscope feature on the DAD board to verify this
         */
        if(blink_flag){

            //toggle blue light
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2|GPIO_PIN_1,GPIO_PIN_2);

        }
        else{

           //toggle red light
           GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0,GPIO_PIN_1);

        }

    }

}


/***********************************function definitions***********************************************/
void InitTimer(void){

    //We will be dividing the system clock by 255
    uint8_t prescalar = 0xFF;

    //enable peripheral clock
    /*
     * A feature of ARM processors in general is that in order to use any
     * given peripheral(UART,Timers,etc.) you must enable their clocks
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //wait for peripheral clock to stabalize
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

    //configure timer for periodic count
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);

    //Set the prescalar to divide system clock
    TimerPrescaleSet(TIMER0_BASE, TIMER_B, prescalar);

    //set timer to interrupt every 1 second
    /*
     * The equation to determine timer period is
     * (Time in seconds) * (System Clock)/(Clock Prescalar)
     */
    TimerLoadSet(TIMER0_BASE, TIMER_B, 1 * SysCtlClockGet()/prescalar);

    //enable interrupt triggered on timeout
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    //set ISR function
    /*
     * One of the features of the TI library is that the use can
     * pass a function pointer into this function to set their
     * own interrupt handler
     *
     * In this case I passed Timer0IntBlink into this function to
     * set it as my interrupt service routine
     */
    TimerIntRegister(TIMER0_BASE, TIMER_B, &Timer0IntBlink);

    //enable timer interrupts from CPU perspective
    IntEnable(INT_TIMER0B);

    //enable timer
    TimerEnable(TIMER0_BASE, TIMER_B);

}

void InitGPIO(void){

    //enable peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //wait for peripheral to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    //set these pins to outputs
    /*
     * Notes: On the launchpad, RGB LEDs are tied to pins 1,2,and 3 of Port F.
     * I set pin 0 as an output for additional probing points on the pad
     */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2| GPIO_PIN_3);


}

/***********************************Interrupt Service Routines*******************************************/
void Timer0IntBlink(void){

    //Clear the interrupt flag
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    blink_flag ^= 0xFF;

}


