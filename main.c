/*
 * Name: LCD_CAN_Node
 * Author: Marquez Jones
 * Desc: The goal of this project is to write messages received through CAN
 *       to the LCD display
 *
 * Notes:
 *       this will be non interrupt driven
 *       received messages must be C String
 *
 * Hardware Notes:
 *                 LCD:
 *                 Project uses parallel LCD communications
 *                 Due to TIVA launcphad constraints LCD data
 *                 bus is divided between D0-D3 and C4-C7(more info in myLCD.h)
 *
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
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

//my includes
#include "LCD.h"
#include "myLCD.h"

//MIL Includes
#include "MIL_CAN.h"

/********************************************FXN PROTO******************************/

/*
 * Desc: copies the instring into the out string and adds a null character
 * Inputs: pointer to in and out strings and length of the in string
 */
void AddNull(uint8_t *instr, uint8_t *outstr, uint16_t inlen);

/*********************************************MAIN**********************************/

int main(void){

    //set system clock to 16MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //arbitrary delay to wait for LCD to power up
    for(uint16_t idx = 0; idx < 2000; idx++);

    char string[] = "hello world";

    /************LCD INIT START***************/
    InitLCD();

    //configured for 5x11 font and 1 line
    ConfigLCD_8Bit(0,1);

    LCDDisplayOFF();

    LCDDisplayON();

    LCDResetCursor();

    LCDWriteCMD(CLEAR_DISPLAY);

    LCDWriteCString(string);

    //next message will overwrite initial string
    LCDResetCursor();

    /************LCD INIT END***************/

    /************CAN INIT START***************/

    //enable gpio clocks for CAN
    /*
     * Check MIL_CAN.h for when this function
     * should be used
     */
    MIL_CANPortClkEnable(MIL_PORT_B);

    MIL_InitCAN0(MIL_PORT_B);

    //initalize reception object
    tCANMsgObject CANMsgObj;
    uint8_t pCANMsgData[8];

    /*
     * in order to receive any ID
     * set both ID and Mask to 0 then enable Filter
     */
    CANMsgObj.ui32MsgID = 0;
    CANMsgObj.ui32MsgIDMask = 0;
    CANMsgObj.ui32Flags = MSG_OBJ_USE_ID_FILTER;
    CANMsgObj.ui32MsgLen = 8;

    //set to message object 1 and configure as RX
    CANMessageSet(CAN0_BASE, 1, &CANMsgObj, MSG_OBJ_TYPE_RX);

    /************CAN INIT END***************/
    while(1){

        CANMsgObj.pui8MsgData = pCANMsgData;

        //read NEWDAT register and bitmask for object 1
        if(CANStatusGet(CAN0_BASE,CAN_STS_NEWDAT) & 0x01){

            /*
             *  Read message object 1 on CAN0
             *  With the above message parameters
             *  Clear the interrupt(Ints currently disabled)
             */
            CANMessageGet(CAN0_BASE, 1,&CANMsgObj, 1);

            LCDWriteCMD(CLEAR_DISPLAY);
            LCDResetCursor();

            LCDWriteCString(pCANMsgData);


        }

    }

}


/**************************************FUNCTION DEFINITIONS********************************************/

/*
 * Desc: copies the instring into the out string and adds a null character
 * Inputs: pointer to in and out strings and length of the in string
 */
void AddNull(uint8_t *instr, uint8_t *outstr, uint16_t inlen){

    //copy string
    for(uint16_t idx = 0; idx < inlen;idx++){

        outstr[idx] = instr[idx];

    }

    //add 0x00 to the end
    outstr[inlen] = 0x00;

}






