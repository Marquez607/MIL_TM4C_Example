/*
 * Name: TX_CAN_Node
 * Author: Marquez Jones
 * Desc: Node transmits strings via CAN
 *
 * Hardware Notes:
 *                 CAN:
 *                 Demo uses CAN0 on Port B
 *                 PB4 - CANRX
 *                 PB5 - CANTX
 *                 CAN must have termination resistors(120 Ohms) on each node
 */

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
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

//MIL Includes
#include "MIL_CAN.h"

/********************************************FUNC PROTOTYPES******************************/

/*
 * Desc: Timer configured to trigger a periodic interrupt
 *       every 1 second
 */
void InitTimer0(void);

/********************************************ISR PROTOTYPES******************************/

/*
 * Desc: ISR to be triggerd by timer0 overflow
 *       will toggle a flag to be used in main
 */
void Timer0ISR(void);

/********************************************GLOBAL DATA******************************/

//if 1, send message
uint8_t timer0_txflag = 0x00;

//selects message to be sent
uint8_t timer0_msgsel = 0x00;

/*********************************************MAIN**********************************/

int main(void){

    //set system clock to 16MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    /************CAN INIT START***************/

    //enable gpio clocks for CAN
    /*
     * Check MIL_CAN.h for when this function
     * should be used
     */
    //MIL_CANPortClkEnable(MIL_PORT_B);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    MIL_InitCAN0(MIL_PORT_B);

    //initalize reception object
    tCANMsgObject CANMsgObj;
    char blue_msg[] = "blue";
    char red_msg[] = "red";

    /*
     * Object used to tranmit messagesr
     */
    CANMsgObj.ui32MsgID = 1;
    CANMsgObj.ui32MsgIDMask = 0;

    /************CAN INIT END***************/

    InitTimer0();

    IntMasterEnable();

    while(1){

        if(timer0_txflag){

            if(timer0_msgsel){

                CANMsgObj.ui32MsgLen = 5;
                CANMsgObj.pui8MsgData = blue_msg;

            }

            else{

                CANMsgObj.ui32MsgLen = 4;
                CANMsgObj.pui8MsgData = red_msg;

            }

            //transmit using CAN object 1
            CANMessageSet(CAN0_BASE, 1, &CANMsgObj, MSG_OBJ_TYPE_TX);

            timer0_txflag = 0;

       }

    }

}


/**************************************FUNCTION DEFINITIONS********************************************/

/*
 * Desc: Timer configured to trigger a periodic interrupt
 *       every 1 second
 */
void InitTimer0(void){

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
       TimerIntRegister(TIMER0_BASE, TIMER_B, &Timer0ISR);

       //enable timer interrupts from CPU perspective
       IntEnable(INT_TIMER0B);

       //enable timer
       TimerEnable(TIMER0_BASE, TIMER_B);


}

/********************************************ISR DEFINITIONS******************************/

/*
 * Desc: ISR to be triggerd by timer0 overflow
 *       a toggle a flag to toggle which message is sent
 *       a tx flag will transmit a CAN message
 */
void Timer0ISR(void){

    //Clear the interrupt flag
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);

    timer0_txflag = 0xFF;
    timer0_msgsel ^= 0xFF;

}






