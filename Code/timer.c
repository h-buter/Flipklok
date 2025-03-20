/*
 * timer.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "timer.h"
#include "stepperAdvance.h"
void setupTimer0()
{
    //Timer setup
    TA0CTL = TASSEL__ACLK | MC__CONTINUOUS| TACLR | TAIE | ID__1;    // Set to counter to ACLK, Continuous clock, clear counter of clock, Enable TAIFG interrupt, Divider set to 1
    TA0CCTL2 = CCIE;              // TACCR0 interrupt enabled
    TA0CCR2 = stepperTimeOffset;             // compare at
}
