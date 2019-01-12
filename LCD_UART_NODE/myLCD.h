/*
 * Name: myLCD.h
 * Author: Marquez Jones
 * Desc: header file for LCD_Demo project
 *       function prototypes for interface TIVA C launchpad with a parallel LCD
 *
 * Hardware Notes:
 * D0 - 3 on LCD : PD0 - PD3
 * D4 - 7 on LCD : PC4 - PC7
 * ENABLE : PF3
 * RW : PF1
 * RS : PF2
 */

#ifndef MYLCD_H_
#define MYLCD_H_

#include<stdint.h>

//LCD data pins are split between two ports
#define LCD_DATAL_PINS GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3
#define LCD_DATAH_PINS GPIO_PIN_4| GPIO_PIN_5| GPIO_PIN_6| GPIO_PIN_7

//special pins
#define LCD_EN_PIN GPIO_PIN_3
#define LCD_RW_PIN GPIO_PIN_1
#define LCD_RS_PIN GPIO_PIN_2

//ascii values
/*
 * CR and LF are sent when enter is sent via a keyboard
 */
#define CR 0x0D //carriage return
#define LF 0x0A //line feed
#define DEL 0x7F //backspace

//LCD line start addresses
#define LINE1_ADDR 0x00
#define LINE2_ADDR 0x40

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
void InitLCD(void);

/*
 * Desc: Sets3 LCD enable
 *
 * Notes: Tied to PF0
 */
void LCDEnable(void);
/*
 * Desc: Clears LCD  enable
 *
 * Notes: Tied to PF0
 *
 */
void LCDDisable(void);

/*
 * Desc: Writes Command to LCD
 *       sets R/W low for write (PF1)
 *       sets RS low for command data (PF2)
 */
void LCDWriteCMD(uint8_t cmd);

/*
 * Desc: Writes Data to LCD
 *       sets R/W low for write (PF1)
 *       sets RS high for data (PF2)
 */
void LCDWriteData(uint8_t data);

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
void LCDWriteASCII(uint8_t ascii,uint8_t *pcounter);

/*
 * Desc: reads from cursor position
 *       sets R/W high for write (PF1)
 *       sets RS high for data (PF2)
 */
uint8_t LCDReadData(void);

/*
 * Desc: reads from cursor position
 *       sets R/W high for write (PF1)
 *       sets RS low for cmd (PF2)
 *
 *       will also output busy flag
 */
uint8_t LCDReadAddr(void);

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
uint8_t LCDReadBusy(void);

/*
 * Desc: sets LCD address and consequency where
 *       next set of data will go
 */
void LCDSetAddr(uint8_t addr);

/*
 * Desc: sets address to first writable
 *       location in LCD line 1
 */
void LCDStartLine1();

/*
 * Desc: sets address to first writable
 *       location in LCD line 2
 *
 * Assumes: that the LCD is configured for
 *          2 lines
 */
void LCDStartLine2();


/*
 * Desc: Configures LCD to 8 bit mode
 * Parameters: lines, determines either 1 or 2 line display
 *             font , determines either 5x8 or 5x11 font
 * Notes:
 * lines - 0 for 1 line and 1 for 2 line
 * font - 0 for 5x8 and 1 for 5x11
 */
void ConfigLCD_8Bit(uint8_t lines,uint8_t font);

/*
 * Desc: Turns LCD display ON
 */
void LCDDisplayON(void);

/*
 * Desc: Turns LCD display OFF
 */
void LCDDisplayOFF(void);

/*
 * Desc: resets cursor to first writeable location
 */
void LCDResetCursor(void);

/*
 * Desc: writes C-String(string terminated by null character) to LCD
 */
void LCDWriteCString(uint8_t *string);

//lol
void LCDWriteMyName(void);
void LCDWriteYEET(void);


#endif /* MYLCD_H_ */
