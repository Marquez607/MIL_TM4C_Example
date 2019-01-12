/*
 * Name: myUART.h
 * Author: Marquez Jones
 * Desc: UART functions to be used in LCD_UART_NODE
 *       includes UART init
 *
 * Hardware Notes: UART1 on Port B used
 *       UART RX: PB0
 *       UART TX: PB1
 *
*/


#ifndef MYUART_H_
#define MYUART_H_

//ascii values
/*
 * CR and LF are sent when enter is sent via a keyboard
 */
#define CR 0x0D //carriage return
#define LF 0x0A //line feed
#define BS 0x08 //backspace

/*
 * Desc: Initializes UART
 *       Baud Rate:115.2k
 *       Length: 8 bits
 *       Parity: None
 *       Stop bits: 1
 *       RX Int enabled
 * Parameters:
 * interrupt - Accepts pointer to ISR function
 *
 *
 * Hardware Notes: UART1 on Port B used
 *       UART RX: PB0
 *       UART TX: PB1
 */
void InitUART1(void (*isr_fxn)(void));

/*
 * Desc: Receives a string from UART,
 *       function will stop once it detects
 *       a user input termination value
 *
 * Paramters:
 *       string: pointer to output string
 *       term: specified termiation character
 *
 * Notes: WILL TERMINATE THE FINAL STRING WITH A
 *        NULL CHARACTER
 *
 *        function will not preserve received terminator
 *
 */
void UART1InString(uint8_t *string,uint8_t term);

#endif /* MYUART_H_ */
