/*
 * clock.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "clock.h"

/// Setup clocks of system, all to LFXTAL input
void setupClocks()
{
    // Clock System Setup
    CSCTL0_H = CSKEY >> 8;                                  // Unlock CS to be able to write to CSCTLx registers
    CSCTL1 = DCOFSEL_6;                                     // Set DCO to 8MHz
    CSCTL2 = SELA__LFXTCLK | SELS__LFXTCLK | SELM__DCOCLK;   // set ACLK = XT1; SMCLK = XT1; MCLK = XT1
    CSCTL3 = DIVA__1 | DIVS__1| DIVM__1;                    // Set all dividers to 1
    CSCTL4 &= ~LFXTOFF;                                     // Enable crystal

    do // Wait until crystal becomes stable
    {
        CSCTL5 &= ~LFXTOFFG;                    // Clear flag
        SFRIFG1 &= ~OFIFG;
    } while(SFRIFG1 & OFIFG);

    CSCTL0_H = 0;                             // Lock CS to lock write access to CSCTLx registers
}
