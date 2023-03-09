// InputOutput.c
// Runs on MSP432
// Test the GPIO initialization functions by setting the LED
// color according to the status of the switches.
// Only SW1 makes color LED blue, and red LED on
// Only SW2 makes color LED red, and red LED on
// Both SW1 and SW2 makes color LED purple, and red LED on
// Neither SW1 or SW2 turns LEDs off

// Daniel and Jonathan Valvano
// September 23, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// built-in LED1 connected to P1.0
// negative logic built-in Button 1 connected to P1.1
// negative logic built-in Button 2 connected to P1.4
// built-in red LED connected to P2.0
// built-in green LED connected to P2.1
// built-in blue LED connected to P2.2
// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05
#include <stdint.h>
#include "msp.h"

#define SW1       0x02                  // on the left side of the LaunchPad board
#define SW2       0x10                  // on the right side of the LaunchPad board
#define RED       0x01
#define GREEN     0x02
#define BLUE      0x04
#define LED_OFF   0x00

/* Port1_Init: This function initializes P1.0, P1.1, and P1.4 (and ONLY those
 *              pins) as follows:
 *              1)  configure P1.4  P1.1 P1.0 as GPIO
 *              2a) make P1.4 and P1.1 inputs
 *              2b) make P1.0 out
 *              3a) enable pull resistors on P1.4 and P1.1
 *              3b) make P1.4 and P1.1 pull-ups (vs pull-downs)
 *
 *              You will need to use bitmasking:
 *              register &= mask to clear bits (bits to clear are 0's in mask)
 *              register |= mask to set bits (bits to set are 1's in the mask)
 */
void Port1_Init(void){
    //             bbbbbbbb
    //             76543210
    // P1->SEL0 =  xxx0xx00  - and bitmask: 0xEC  (replace x's with ones)
    // P1->SEL1 =  xxx0xx00  - and bitmask: 0xEC  (replace x's with ones)
    // P1->DIR  =  xxx0xx0x  - and bitmask: 0xED  (replace x's with ones)
    // P1->DIR  =  xxxxxxx1  - or  bitmask: 0x01  (replace x's with zeros)
    // P1->REN  =  xxx1xx1x  - or  bitmask: 0x12  (replace x's with zeros)
    // P1->OUT  =  xxx1xx1x  - or  bitmask: 0x12  (replace x's with zeros)
    P1->SEL0 &= 0xEC;
    P1->SEL1 &= 0xEC;
    P1->DIR &= 0xED;
    P1->DIR |= 0x01;
    P1->REN |= 0x12;
    P1->OUT |= 0x12;
}

/* Port1_Input: This function reads P1->IN, inverts the data so that
 *              P1.4 and P1.1 are positive logic, and clears the remaining
 *              bits.
 *
 */
uint8_t Port1_Input(void){
    // Invert P1->IN and then mask to preserve bits 4 and 1; all others are 0
    //             bbbbbbbb
    //             76543210
    // P1->IN   =  000x00x0  - and bitmask: 0x12  (replace x's with ones)
    return ((~P1->IN)&0x12);
}

/* Port1_Output: This function writes P1->OUT, set P1.0 to match data, and
 *               preserving all other bits. Note that data will ONLY be 0 or 1.
 */
void Port1_Output(uint8_t data){  // write output to P1.0
    //  Clear bit 0 *before* or'ing with data.
    //             bbbbbbbb
    //             76543210
    // P1->OUT  =  xxxxxxx0  - and bitmask: 0xFE  (replace x's with ones)
    P1->OUT = (P1->OUT&0xFE)|data;
}

/* Port2_Init: This function initializes P2.0, P2.1, and P2.3 (and ONLY those
 *              pins) as follows:
 *              1)  configure as GPIO
 *              2)  turn off all LED's initially (OUT register)
 *              3)  configure as outputs
 *              4)  set drive strength to High Drive
 *
 */
void Port2_Init(void){
    //             bbbbbbbb
    //             76543210
    // P2->SEL0 =  xxxxx000  - and bitmask: 0xF8  (replace x's with ones)
    // P2->SEL1 =  xxxxx000  - and bitmask: 0xF8  (replace x's with ones)
    // P2->DIR  =  xxxxx111  - or  bitmask: 0x07  (replace x's with zeros)
    // P2->DS   =  xxxxx111  - or  bitmask: 0x07  (replace x's with zeros)
    // P2->OUT  =  xxxxx000  - and bitmask: 0xF8  (replace x's with ones)
    P2->SEL0 &= 0xF8;
    P2->SEL1 &= 0xF8;
    P2->DIR |= 0x07;
    P2->DS |= 0x07;
    P2->OUT &= 0xF8;
}


/* Port1_Output: This function writes P2->OUT, sets P2.0-P2.2 to match data, and
 *               preserving all other bits. Note the P2.1 is a don't care, since
 *               the code will always set bit 1 to 0 (LED OFF).
 */
void Port2_Output(uint8_t data){  // write three outputs bits of P2
    // Clear bits 2, 1 and 0 before or'ing with data
    //             bbbbbbbb
    //             76543210
    // P2->OUT  =  xxxxx000  - and bitmask: 0xF8  (replace x's with ones)
    P2->OUT = (P2->OUT&0xF8)|data;
}


int main(void){ uint8_t status;
  Port1_Init();                    // initialize P1.1 and P1.4 and make them inputs (P1.1 and P1.4 built-in buttons)
                                   // initialize P1.0 as output to red LED
  Port2_Init();                    // initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)
  while(1){
    status = Port1_Input();
    switch(status){                 // data will represent P1.4 and P1.1 in positive logic
      case SW1:                    // SW1 pressed
        Port2_Output(BLUE);
        Port1_Output(RED);
        break;
      case SW2:                    // SW2 pressed
        Port2_Output(RED);
        Port1_Output(RED);
        break;
      case (SW1+SW2):                    // both switches pressed
        Port2_Output(BLUE+RED);
        Port1_Output(RED);
        break;
      case LED_OFF:                    // neither switch pressed
        Port2_Output(LED_OFF);
        Port1_Output(LED_OFF);
        break;
    }
  }
}

