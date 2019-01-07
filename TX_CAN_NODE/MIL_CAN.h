/*
 * Name: MIL_CAN.h
 * Author: Marquez Jones
 * Desc: A set of wrapper functions I designed
 *       to standardized CAN use in the Lab
 *       this primarily is to reduce the number
 *       of bugs caused by the CAN bus
 *
 * Notes: CAN devices should be run at 100k bps
 *        and PCBs on the network should have on
 *        board termination resistors
 */

#ifndef MIL_CAN_H_
#define MIL_CAN_H_

/*
 *Desc: Port selection will come from this enum
 */
typedef enum {
    MIL_PORT_A,
    MIL_PORT_B,
    MIL_PORT_E,
    MIL_PORT_F
}mil_port;

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
void MIL_InitCAN0(mil_port port);

/*
 * Desc: enables CAN1
 *       CAN 1 is restricted
 *       to Port A so there's
 *       no input parameter
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
void MIL_InitCAN1(void);

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
void MIL_CAN0IntEnable(void (*func_ptr)(void));

/*
 * Desc: Enables interrupts on CAN1
 *
 * Notes: see CAN0
 */
void MIL_CAN1IntEnable(void (*func_ptr)(void));

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
void MIL_CANPortClkEnable(mil_port port);


#endif /* MIL_CAN_H_ */
