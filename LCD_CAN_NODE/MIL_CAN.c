/*
 * Name: MIL_CAN.c
 * Author: Marquez Jones
 * Desc: A set of wrapper functions I designed
 *       to standardized CAN use in the Lab
 *
 * Notes: CAN devices should be run at 100k bps
 *        and PCBs on the network should have on
 *        board termination resistors
 */

/* INCLUDES */
//includes
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

//MIL includes
#include"MIL_CAN.h"

/*
 * Desc: enables CAN0 which can be enabled on
 *       Ports B,E, or F
 *
 * Notes: Does not enable interrupts
 *        or port clocks
 *        PORT and Interrupts
 *        must be enabled outside funciton
 *
 * Hardware Notes:
 * PF0 - CANRX  PB4 - CANRX  PE4 - CANRX
 * PF3 - CANTX  PB5 - CANTX  PE5 - CANTX
 *
 * Inputs: port from mil_port enum
 * Assumes: Port clocks are enabled
 */
void MIL_InitCAN0(mil_port port){


    //pin configuration
    /*
     * Depending on the port, different pins
     * need to be configured
     */
    switch(port){
        case MIL_PORT_B:
            GPIOPinConfigure(GPIO_PB4_CAN0RX);
            GPIOPinConfigure(GPIO_PB5_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
            break;
        case MIL_PORT_E:
            GPIOPinConfigure(GPIO_PE4_CAN0RX);
            GPIOPinConfigure(GPIO_PE5_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
            break;
        case MIL_PORT_F:
            GPIOPinConfigure(GPIO_PF0_CAN0RX);
            GPIOPinConfigure(GPIO_PF3_CAN0TX);
            GPIOPinTypeCAN(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_3);
            break;
    }

    //enable CAN peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    //Initialize CAN controller
    CANInit(CAN0_BASE);

    //Set can retry to true
    CANRetrySet(CAN0_BASE,1);

    //Set bit rates
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 100000);

    //enable CAN
    CANEnable(CAN0_BASE);

}

/*
 * Desc: enables CAN1
 *       CAN 1 is restricted
 *       to Port A so there's
 *       no input parameter
 *
 *
 * Notes: Does not enable interrupts
 *        or port clocks
 *        PORT A and Interrupts
 *        must be enabled outside function
 *
 * Hardware Notes:
 * PA0 - CANRX
 * PA1 - CANTX
 *
 * Assumes: Port clock enabled
 */
void MIL_InitCAN1(void){

    //sets CAN as available pin functions
    GPIOPinConfigure(GPIO_PA0_CAN1RX);
    GPIOPinConfigure(GPIO_PA1_CAN1TX);
    GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //enable CAN peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);

    //Initialize CAN controller
    CANInit(CAN1_BASE);

    //Set can retry to true
    CANRetrySet(CAN1_BASE,1);

    //Set bit rates
    CANBitRateSet(CAN1_BASE, SysCtlClockGet(), 100000);

    //enable CAN
    CANEnable(CAN1_BASE);


}

/*
 * Desc: Enables interrupts on CAN0
 *
 * Notes: The Tiva CAN system has two
 *        separate interrupt sources.
 *        Either a controller error has
 *        occured(Tiva side) or there's
 *        a status change which can result
 *        from message transfer or a system
 *        bus error.
 *
 *        Further diagnostics in required in
 *        the external program
 *
 * Inputs: A pointer to your custom ISR
 * Assumes: Nothing
 */
void MIL_CAN0IntEnable(void (*func_ptr)(void)){

    //set a custom ISR
    CANIntRegister(CAN0_BASE, func_ptr);

    //enable status interrupts
    //errors due to controller errors are ignored
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_STATUS);

    IntEnable(INT_CAN0);

}

/*
 * Desc: Enables interrupts on CAN1
 *
 * Notes: see CAN0
 */
void MIL_CAN1IntEnable(void (*func_ptr)(void)){

    //set a custom ISR
    CANIntRegister(CAN1_BASE, func_ptr);

    //enable status interrupts
    //errors due to controller errors are ignored
    CANIntEnable(CAN1_BASE, CAN_INT_MASTER | CAN_INT_STATUS);

    IntEnable(INT_CAN1);



}

/*
 * Desc: Enables the Port associated with the CAN system
 *
 * Note: For sake of your program making sense,
 *       only call this function if your CAN shares the
 *       port with no other GPIOs or peripherals
 *
 *       This just called the GPIO clock enable for
 *       the port
 */
void MIL_CANPortClkEnable(mil_port port){

    switch(port){

        case MIL_PORT_A:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
            break;
        case MIL_PORT_B:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
            break;
        case MIL_PORT_E:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
            break;
        case MIL_PORT_F:
            SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
            break;

    }

}









