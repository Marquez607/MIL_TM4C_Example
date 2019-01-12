/*
 * Name: myLCD.c
 * Author: Marquez Jones
 * Desc: c file for LCD_Demo projecta
 *       function definitions for interface TIVA C launchpad with LCD
 */

//includes
#include "myLCD.h"
#include "LCD.h"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

char yeet[] = "YEET";

/*
 * Desc: Inits pins used in LCD writing
 *
 * Notes: Due to limited pins availability on TIVA Launchpad
 *        Data bus will be split between two different ports
 *        Port D 0-3 and Port C 4-7
 *        PF0 for enable signal
 *        PF1 for R/W signal
 *        PF2 for RS signal
 *
 * Assumes: Nothing
 */
void InitLCD(void){

    //Peripheral clock enables
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    /* Port D Inits */
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_0|
                           GPIO_PIN_1|
                          GPIO_PIN_2|
                          GPIO_PIN_3);

    /* Port C Inits*/
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_4|
                          GPIO_PIN_5|
                          GPIO_PIN_6|
                          GPIO_PIN_7);

    /*Port F Inits*/
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_3|
                                          GPIO_PIN_1|
                                          GPIO_PIN_2);

    //reset LCD
   LCDWriteCMD(LCD_RESET);

}

/*********************************NOTE******************************/
/* ALL FUNCTIONS BELOW ASSUMING LCD INIT HAS BEEN CALLED*/
/*******************************************************************/

/*
 * Desc: Sets LCD enable
 *
 * Notes: Tied to PF0
 */
void LCDEnable(void){

    GPIOPinWrite(GPIO_PORTF_BASE, LCD_EN_PIN, LCD_EN_PIN);

}
/*
 * Desc: Clears LCD  enable
 *
 * Notes: Tied to PF0
 *
 */
void LCDDisable(void){

    GPIOPinWrite(GPIO_PORTF_BASE, LCD_EN_PIN, 0x00);

}

/*
 * Desc: Writes Command to LCD
 *       sets R/W low for write (PF1)
 *       sets RS low for command data (PF2)
 */
void LCDWriteCMD(uint8_t cmd){

    //stall until bus no longer busy
    while(LCDReadBusy());

    //port D
    uint8_t cmd_L = cmd & 0x0F;
    //port C
    uint8_t cmd_H = cmd & 0xF0;

    //clear RS and R/W pin
    GPIOPinWrite(GPIO_PORTF_BASE, LCD_RS_PIN | LCD_RW_PIN, 0x00);

    LCDEnable();

    //write low nibble
    GPIOPinWrite(GPIO_PORTD_BASE, LCD_DATAL_PINS, cmd_L);

    //write high nibble
    GPIOPinWrite(GPIO_PORTC_BASE, LCD_DATAH_PINS, cmd_H);

    LCDDisable();

}

/*
 * Desc: Writes Command to LCD
 *       sets R/W low for write (PF1)
 *       sets RS high for data (PF2)
 */
void LCDWriteData(uint8_t data){

    //stall until bus no longer busy
     while(LCDReadBusy());

    //port D
    uint8_t data_L = data & 0x0F;
    //port C
    uint8_t data_H = data & 0xF0;

    //set RS and clear RW pin
    GPIOPinWrite(GPIO_PORTF_BASE, LCD_RS_PIN | LCD_RW_PIN, LCD_RS_PIN);

    LCDEnable();

    //write low nibble
    GPIOPinWrite(GPIO_PORTD_BASE, LCD_DATAL_PINS, data_L);

    //write high nibble
    GPIOPinWrite(GPIO_PORTC_BASE, LCD_DATAH_PINS, data_H);

    LCDDisable();

}

/*
 * Desc: Writes ascii characters to
 *       display. This will take into
 *       backspace input
 *
 * NOTES: if the system receives "enter" (CR and LF)
 *       it will alternate the line in a two line system
 *       also due to lazy programming, if the system
 *       is not configured for two lines, the function will
 *       reconfigure to two lines
 */
void LCDWriteASCII(uint8_t ascii, uint8_t *pcounter){


    if(ascii == DEL){


        LCDWriteCMD(CURSOR_MOVE_L);

        //set to entry mode
        LCDWriteCMD(ENTRY_MODE | CURSOR_INC);

        //write 0 to that location to clear
        LCDWriteData(' ');

        LCDWriteCMD(CURSOR_MOVE_L);

        *pcounter = *pcounter - 1;

    }
    else if(ascii == CR){

        ConfigLCD_8Bit(1,1);

        /*
         * will alternate lines
         * so if the cursor is on line 1, it will jump to line 2
         * if the cursor is on line 2, it will jump to line 1
         */
        if(LCDReadAddr() > LINE2_ADDR){

            LCDStartLine1();

        }
        else{

            LCDStartLine2();

        }

    }
    else if(ascii == LF){

        //if Line feed, do nothing since it's paired with CR

    }
    else{

        //set to entry mode
        LCDWriteCMD(ENTRY_MODE | CURSOR_INC);
        LCDWriteData(ascii);

        *pcounter++;

    }

}

/*
 * Desc: reads from cursor position
 *       sets R/W high for write (PF1)
 *       sets RS high for data (PF2)
 */
uint8_t LCDReadData(void){

    uint8_t data_L = 0x00;
    uint8_t data_H = 0x00;

    //set RS and set RW pin
    GPIOPinWrite(GPIO_PORTF_BASE, LCD_RS_PIN | LCD_RW_PIN, LCD_RS_PIN | LCD_RW_PIN);

    /* reconfigure for input */
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE,GPIO_PIN_0|
                                          GPIO_PIN_1|
                                          GPIO_PIN_2|
                                          GPIO_PIN_3);

    /* reconfigure for input */
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_4|
                                          GPIO_PIN_5|
                                          GPIO_PIN_6|
                                          GPIO_PIN_7);

    LCDEnable();

    //write low nibble
    data_L = GPIOPinRead(GPIO_PORTD_BASE, LCD_DATAL_PINS);

    //write high nibble
    data_H = GPIOPinRead(GPIO_PORTC_BASE, LCD_DATAH_PINS);

    LCDDisable();

    /* reconfigure for output */
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_0|
                                              GPIO_PIN_1|
                                              GPIO_PIN_2|
                                              GPIO_PIN_3);

    /* reconfigure for output */
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_4|
                                              GPIO_PIN_5|
                                              GPIO_PIN_6|
                                              GPIO_PIN_7);

    return data_H & data_L;

}

/*
 * Desc: reads from cursor position
 *       sets R/W high for write (PF1)
 *       sets RS low for cmd (PF2)
 *
 *       will also output busy flag
 */
uint8_t LCDReadAddr(void){

    uint8_t data_L = 0x00;
    uint8_t data_H = 0x00;

    //clear RS and set RW pin
    GPIOPinWrite(GPIO_PORTF_BASE, LCD_RS_PIN | LCD_RW_PIN, LCD_RW_PIN);

    /* reconfigure for input */
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE,GPIO_PIN_0|
                                          GPIO_PIN_1|
                                          GPIO_PIN_2|
                                          GPIO_PIN_3);

    /* reconfigure for input */
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_4|
                                          GPIO_PIN_5|
                                          GPIO_PIN_6|
                                          GPIO_PIN_7);

    LCDEnable();

    //read low nibble
    data_L = GPIOPinRead(GPIO_PORTC_BASE, LCD_DATAL_PINS);

    //read high nibble
    data_H = GPIOPinRead(GPIO_PORTD_BASE, LCD_DATAH_PINS);

    LCDDisable();

    /* reconfigure for output */
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_0|
                                                 GPIO_PIN_1|
                                                 GPIO_PIN_2|
                                                 GPIO_PIN_3);

    /* reconfigure for output */
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_4|
                                                 GPIO_PIN_5|
                                                 GPIO_PIN_6|
                                                 GPIO_PIN_7);

    return data_H & data_L;

}

/*
 * Desc: sets LCD address and consequency where
 *       next set of data will go
 */
void LCDSetAddr(uint8_t addr){

    //sets set address bit high
    uint8_t cmd = CURSOR_ADDR & addr;

    LCDWriteCMD(cmd);

}

/*
 * Desc: sets address to first writable
 *       location in LCD line 1
 */
void LCDStartLine1(){

    LCDSetAddr(LINE1_ADDR);

}

/*
 * Desc: sets address to first writable
 *       location in LCD line 2
 *
 * Assumes: that the LCD is configured for
 *          2 lines
 */
void LCDStartLine2(){

    LCDSetAddr(LINE2_ADDR);

}

/*
 * Desc: checks the busy flag
 *       sets R/W high for write (PF1)
 *       sets RS low for cmd (PF2)
 *
 *       Will output 1 if bus is busy
 *       0 otherwise
 *
 * Note: Should be used in all functions that
 *       are reading or writing to LCD
 */
uint8_t LCDReadBusy(void){

    uint8_t busy_flag = LCDReadAddr();

    return busy_flag >> 7;

}

/*
 * Desc: Configures LCD to 8 bit mode
 * Parameters: lines, determines either 1 or 2 line display
 *             font , determines either 5x8 or 5x11 font
 * Notes:
 * lines - 0 for 1 line and 1 for 2 line
 * font - 0 for 5x8 and 1 for 5x11
 */
void ConfigLCD_8Bit(uint8_t lines,uint8_t font){

    //default configuration is one line and 5x8 font
    uint8_t line_cmd = EIGHT_BIT_ONE_LINE;
    uint8_t cmd_font = 0x00;

    if(font){

        //set F bit in function set to 1 for 5x11 font
        cmd_font = 0x04;

    }
    if(lines){

        //change to two line mode if lines = 1
        line_cmd = EIGHT_BIT_TWO_LINE;

    }

    LCDWriteCMD(line_cmd | cmd_font);

}

/*
 * Desc: Turns LCD display ON
 */
void LCDDisplayON(void){

    LCDWriteCMD(DISPLAY_ON | DISPLAY_CTRL);

}

/*
 * Desc: Turns LCD display OFF
 */
void LCDDisplayOFF(void){

    LCDWriteCMD(DISPLAY_OFF | DISPLAY_CTRL);

}

/*
 * Desc: resets cursor to first writeable location
 */
void LCDResetCursor(void){

    LCDWriteCMD(RETURN_HOME);

    LCDWriteCMD(CURSOR_MOVE_R);

}

/*
 * Desc: writes C-String(string terminated by null character) to LCD
 */
void LCDWriteCString(uint8_t *string){

    //set entry mode
    LCDWriteCMD(ENTRY_MODE | CURSOR_INC);


    while(*string != 0x00){

         LCDWriteData(*string);

         string++;

    }


}

void LCDWriteMyName(void){

    char name[] = "Marquez";

    //set entry mode
    LCDWriteCMD(ENTRY_MODE | CURSOR_INC);

    uint8_t idx = 0;
    while(name[idx] != 0x00){

        LCDWriteData(name[idx]);

        idx ++;

    }

}

void LCDWriteYEET(void){

    //set entry mode
    LCDWriteCMD(ENTRY_MODE | CURSOR_INC);

    uint8_t idx = 0;
    while(yeet[idx] != 0x00){

        LCDWriteData(yeet[idx]);

        idx ++;

    }

}



