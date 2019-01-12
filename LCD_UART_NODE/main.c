/*
 * Name: LCD_UART_NODE
 * Author: Marquez Jones
 * Desc: The goal of this project is to write messages received through CAN
 *       to the LCD display
 *
 * Hardware Notes: system employs UART1 on port B
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
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

//my includes
#include "LCD.h"
#include "myLCD.h"
#include "myUART.h"

/************************FLAGS******************************/

uint8_t rx_flag = 0x00;

//0 if on line 1, not 0 if on line 2
uint8_t line_flag = 0;

/*********************************************ISR PROTOTYPES**********************************/

void SwapLines(void);

/*
 * some uncalculated software delay I used to ensure LCD was fully booted
 * before the TIVA attempted to write to it
 */

void ArbSoftDelay(void);

/*********************************************ISR PROTOTYPES**********************************/

//interrupt configured for RX interrupt
void UART_RX_Handler(void);

/*********************************************MAIN**********************************/

int main(void){

    //set system clock to 16MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //wait for LCD boot up before proceeding
    ArbSoftDelay();

    char string[] = "hello world";

    InitLCD();

    //configure to be 5x11 font and 2 lines
    ConfigLCD_8Bit(1,1);

    LCDDisplayON();

    LCDResetCursor();

    LCDWriteCMD(CLEAR_DISPLAY);

    LCDWriteCString(string);

    InitUART1(&UART_RX_Handler);

    IntMasterEnable();

    //reset cursor in prep for user input
    LCDResetCursor();

    uint8_t data;

    //keeps track of characters input to
    uint8_t counter = 0;

    //LCDWriteASCII function can track counter for you
    //backspaces retract from counter and CR/LF does not affect it
    uint8_t *pcounter;
    pcounter = &counter;

    while(1){

      if(rx_flag){

          data = UARTCharGet(UART1_BASE);

          LCDWriteASCII(data,pcounter);

          rx_flag = 0;

      }

      //LCD is a 2x16
      if(counter > 15){

          SwapLines();

      }


    }

}

/************************FXN DEFINITIONS******************************/

/*
 * just swaps which line the LCD is outputting
 * characters to
 *
 * Based on a flag
 */
void SwapLines(void){

    /*
     * if you are on line 2
     *  swap to line 1
     *
     * if you are on line 1
     *  swap to line 2
     *
     */
    if(line_flag){

        LCDStartLine1();

    }
    else{

        LCDStartLine2();

    }

    line_flag ^= 0xFF;

}

void ArbSoftDelay(void){

    for(uint16_t idx = 0; idx < 5000; idx++);

}

/************************ISR******************************/
void UART_RX_Handler(void){

   UARTIntClear(UART1_BASE, UART_INT_RX); //clear the asserted interrupts

   rx_flag = 1;

}







