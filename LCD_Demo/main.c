/*
 * Name: LCD_Demo
 * Author: Marquez Jones
 * Desc: Me figuring out how to use an LCD display with the TIVA
 */

//includes
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

//my includes
#include "LCD.h"
#include "myLCD.h"


/*********************************************MAIN**********************************/

int main(void){


    //set system clock to 16MHZ
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    for(uint16_t idx = 0; idx < 2000; idx++);

    char string[] = "hello world";

    InitLCD();

    ConfigLCD_8Bit(1,1);

    LCDDisplayOFF();

    LCDDisplayON();

    LCDResetCursor();

    LCDWriteCMD(CLEAR_DISPLAY);

    LCDWriteCString(string);

    while(1){

    }

}





