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

/* INCLUDES */
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
#include "myUART.h"

/*
 * Desc: Initializes UART
 *       Baud Rate:115.2k
 *       Length: 8 bits
 *       Parity: None
 *       Stop bits: 1
 *       RX Int enabled
 *       FIFO enabled
 * Parameters:
 * interrupt - Accepts pointer to ISR function
 *
 *
 * Hardware Notes: UART1 on Port B used
 *       UART RX: PB0
 *       UART TX: PB1
 */
void InitUART1(void (*isr_fxn)(void)){
       /* Peripheral Clock enables
        * Enable Port B
        * Enable UART0
        */
       SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

       while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1));

       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

       while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));

       //Pin configuarations
       GPIOPinConfigure(GPIO_PB0_U1RX);
       GPIOPinConfigure(GPIO_PB1_U1TX);

       //Set A1 and A0 to alternate pin functions(UART)
       GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

       /*
        * Configure UART
        * -115.2kbp = baud rate
        * -8 bit message length
        * -No Parity
        * -One stop bit
        */
       UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
                           (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                               UART_CONFIG_PAR_NONE));

       /*INTERRUPTS*/
       //Peripheral Interrupt configs
       UARTIntEnable(UART1_BASE,UART_INT_RX);
       UARTIntRegister(UART1_BASE, isr_fxn);

       //CPU interrupt enbales
       IntEnable(INT_UART1);

       UARTEnable(UART1_BASE);
       /*
        * Note:
        * if FIFO is enabled, rx and tx interrupts
        * will be based on user defined level as opposed
        * to any data recevied/transmitted
        *
        * in this I disabled FIFO
        */
        UARTFIFODisable(UART1_BASE);


}

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
void UART1InString(uint8_t *string,uint8_t term){

    uint8_t temp = UARTCharGet(UART1_BASE);

    //while received character isn't the termination char
    while(temp != term){

        //echo input to terminal
        UARTCharPut(UART1_BASE,temp);

        //if backspace received decrement string and remove char
        if(temp == BS){

            string = string - 1;
            *string = 0;

        }
        else{
            //set string equal to received character
            *string = temp;

            // increment string
            string++;
        }
        temp = UARTCharGet(UART1_BASE);


    }

    //terminate string with null
    string++;
    *string = 0x00;

}


