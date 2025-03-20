/*
 * fwdButton.c
 *
 *  Created on: 20 mrt. 2025

 */

#include "fwdButton.h"

/// Interrupt vector of port1
#pragma vector=PORT1_VECTOR
__interrupt void fwdButtonISR(void)
{
    switch(__even_in_range( P1IV, 0x10 ))
    {
        case 0x00: break; // None
        case 0x02: break; // Pin 0
        case 0x04:
            P4OUT ^= BIT6;
            if (timerOffsetToggle == 0) // Toggle edge interrupt If rising edge was set, switch to falling & vice versa
            {
                P1IES &= ~BIT1;
                timerOffsetToggle = 1;
            }
            else
            {
                P1IES |= BIT1;
                timerOffsetToggle = 0;
            }
            P1IFG &= ~BIT0;  // Clear interrupt flag
            break; // Pin 1
        case 0x06: break; // Pin 2
        case 0x08: break; // Pin 3
        case 0x0A: break; // Pin 4
        case 0x0C: break; // Pin 5
        case 0x0E: break; // Pin 6
        case 0x10: break; // Pin 7
        default: _never_executed();
    }
}


