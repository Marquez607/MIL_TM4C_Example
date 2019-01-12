
/*
 * Name: GPIO ISR Demo
 * Author: Marquez Jones
 * Desc: This is a demo on how to configure pins to trigger interrupts
 *       In this demo, we will use one of the push buttons on the launchpad to
 *       toggle the GPIO
 *       These buttons are not debounced in hardware so a live demo may
 *       encounter some unpredictable results due to bouncing
 *       In general hardware designs should have some form of debouncing for push buttons
 *
 *       This program will toggle an LED every time the button is pressed
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
#define BLUE_LED GPIO_PIN_2
#define PUSH_SW_1 GPIO_PIN_4

//function prototypes
void InitGPIO(void);

//ISR prototypes
void LEDToggleISR(void);

//Global variabls
uint8_t STATE = 0x00;


/*************************************** MAIN *****************************************/

int main(void)
{
    //set system clock to 16MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //Run master interrupt enable
    IntMasterEnable();

    //gpio configurations
    InitGPIO();

    while(1){

        GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, STATE);

    }
}

/***************************************FUNCTION DEFINITIONS***************************/

//initializes button and LEDs
void InitGPIO(void){


    //enable peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //wait for peripheral to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    //set these pin directions
    /*
     * Notes: On the launchpad, RGB LEDs are tied to pins 1,2,and 3 of Port F.
     *
     * Switches are on pins 0 and 4
     * I set pin 4 to input
     */
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, PUSH_SW_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_2);

    /*
     * Configure the push input to have an interal pull up resistor
     *
     * Notes:the gpio_strength parameter would set the current output to be
     * 2mA if the pin were configured as an output. In this case it has no effect
     * since the pin is an input pin
     */
    GPIOPadConfigSet(GPIO_PORTF_BASE,
                     PUSH_SW_1,
                     GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);

    /*Configure interrupt*/
    //set ISR function
    GPIOIntRegister(GPIO_PORTF_BASE, &LEDToggleISR);

    //configure pin for falling edge
    GPIOIntTypeSet(GPIO_PORTF_BASE,  PUSH_SW_1 , GPIO_FALLING_EDGE);

    //enable interrupts on for that pin
    GPIOIntEnable(GPIO_PORTF_BASE, PUSH_SW_1);

    /*DO NOT CALL IntEnable function here*/
}

/***************************************ISR DEFINITIONS*******************************/

void LEDToggleISR(void){

    //GPIOPinWrite(GPIO_PORTF_BASE, BLUE_LED, 0xFF);
    GPIOIntClear(GPIO_PORTF_BASE,PUSH_SW_1);

    //Toggle Pin State
    STATE ^= 0xFF;

}










