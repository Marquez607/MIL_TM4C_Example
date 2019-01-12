Project Name: Mini LCD CAN Network
Author: Marquez Jones
Desc: This project involves two Tiva launchpads whose main purpose is to demonstrate 
       CAN operation on the TIVA processor. The overall function is one processor termed
       the TX_CAN_NODE will routinely strings to the LCD_CAN_NODE who will display the message
       on an 16x2 LCD screen. 
       
  TX_CAN_NODE: This node transmits the messages "red" or "blue" which are C strings
                terminated by a null character. This process involves the CAN bus, which
                is the medium of communincation, and the timer0 peripheral. The timer is
                used to trigger a transmission. This transmit a message every 1 second which 
                is accomplished with a timer interrupt. This timer ISR will also toggle the message
                sent.

  LCD_CAN_NODE: This nodes receives CAN messages from the bus. This node assumes the messages are 
                 C-strings as described above and will fail if the message is not terminated by a 
                 NULL. This also assumes that the string will only be the result of one CAN transmission
                 as opposed to multiple. An LCD driver was written to write strings to the LCD display.

Note: I highly recommend all EEs in MIL read up on the CAN communication protocol.
      Resources for this include the TIVA CAN section which provides a brief description
      of CAN and the TIVA. 
      Other Resources are listed here:
      http://www.ni.com/white-paper/2732/en/
      https://en.wikipedia.org/wiki/CAN_bus
      https://sewelldirect.com/learning-center/canbus-technology
      
      Videos On CAN:
      https://www.youtube.com/watch?v=YvsGuK9Up0E
      
Hardware Notes: In order to provide noise tolerance, CAN messages are transmitted via a differential pair. 
                This pair is denoted as CAN High(CANH) and CAN Low(CANL). In order to transmission, the bus must have at least
                two termination resistors of 120 Ohms at each end of the bus. Alternatively, you can have the termination resistors
                between the CANH and CANL of each ECU in your system. The latter is what we will enforce in MIL.
                
       CANH--+--------------------------+--CANH
             |                          |
            .-.                        .-.
            | |120ohm                  | |120ohm
            | |                        | |
            '-'                        '-'
             |                          |
       CANL--+--------------------------+--CANL

Header Files Created by Me: 
       MIL_CAN:
       In order to make life easier, I made a MIL_CAN header file for anyone to use which will allow very 
       primitive deployment of CAN on boards. For more robust situaions, a more involved solution than what's 
       currently provided may need to be created.
       
       myLCD: 
       I created this header for parallel communications with a 1602 LCD panel. This driver has a set of hardcoded pins that
       it uses for LCD operations which makes it not as portable but still useful template if someone was to use a
       parallel LCD system.
