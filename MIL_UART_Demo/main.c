/*
 * Name: UART Demo
 * Author: Marquez Jones
 * Desc: This demonstrates UART
 *       This example continually outputs 'U'
 *       until you send data. At this point,
 *       it will echo your inputs
 *
 * Note: To test this, you can use a DAD board to see the output.
 *       DAD boards also have a logic analyzer that allow you to read the UART
 *       protocol,
 *       but it's recommended that you use a serial terminal such as Putty.
 *       The launchpad has no usb access to the UART pins so you'll need a
 *       FTDI breakout board to use the putty terminal
 *
 * Hardware Notes:
 * PB0 - UART RX
 * PB1 - UART TX
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
/************************FLAGS******************************/
//test flags for UART demo
//Is used for tiva to continuously output 'U'
uint8_t tx_test_flag = 1;
//Will be used to echo back received data
uint8_t rx_test_flag = 0;

/************************FUNCTION PROTOTYPES******************************/
//initialize UART
void InitUART(void);

//These GPIOs will be used to indicate that the program is flashed
void InitGPIO(void);

 /************************ISR PROTOTYPES******************************/
//interrupt configured for RX interrupt
void UART_RX_Handler(void);

/************************MAIN******************************/
int main(void)
{


    /*CONFIGURE SYSTEM CLOCK TO 16MHZ*/
    SysCtlClockSet(SYSCTL_SYSDIV_1 |
                   SYSCTL_USE_OSC  |
                   SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);


    /*
     * Note:
     * With in interrupt enable hierarchy,
     * You have 3 separate enables to deploy
     * One is the specific enable for any particular flag in the peripheral
     * One is the total interrupt enable for the peripheral
     * and one is the master enable which enables ISR on the MCU
     *
     */
    IntMasterEnable();

    //initialize UART
    InitUART();

    //initialize GPIO
    InitGPIO();

    //confirm that the program is running by turning on these pins
    GPIOPinWrite(GPIO_PORTF_BASE,
                 GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3,
                 GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3);

    while(1){

        if(tx_test_flag){
            /*Uart test by outputting 'U'*/
            UARTCharPut(UART1_BASE,0x55);
        }

    }

	//return 0;
}

/************************FUNCTIONS******************************/
void InitUART(void){

        /*
        * Peripheral Clock enables
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
       UARTIntRegister(UART1_BASE, &UART_RX_Handler);

       //CPU interrupt enbales
       IntEnable(INT_UART1);

       UARTEnable(UART1_BASE);

       /*
        *Note:
        * if FIFO is enabled, rx and tx interrupts
        * will be based on user defined level as opposed
        * to any data recevied/transmitted
        *
        * in this I disabled FIFO
        */
        UARTFIFODisable(UART1_BASE);



}

void InitGPIO(void){

    //enable peripheral clock
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //wait for peripheral to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    //set port F pins as outputs
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3);


}

/************************ISR******************************/
void UART_RX_Handler(void){
    tx_test_flag = 0;  // Stop transmitting
   // rx_test_flag = 1;  // now echo
    uint8_t rx_data = UARTCharGet(UART1_BASE);

     //transmit data
    UARTCharPut(UART1_BASE,rx_data);
}

