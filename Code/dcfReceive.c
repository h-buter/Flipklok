/*
 * dcfReceive.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "dcfReceive.h"
#include "timeKeeping.h"

// Temporary used for simulating DCF messages
// Interrupt vector of port4
#pragma vector=PORT4_VECTOR
__interrupt void dcFButtonISR(void)
{
    switch(__even_in_range( P4IV, 0x10 ))
    {
        case 0x00: break; // None
        case 0x02: break; // Pin 0
        case 0x04: break; // Pin 1
        case 0x06: break; // Pin 2
        case 0x08: break; // Pin 3
        case 0x0A: break; // Pin 4
        case 0x0C:
            P4IE &= ~BIT5;                              // Disable interrupt
            P4IFG &= ~BIT5;                             // Clear flag
//            timeOfLastDcfMessage = timeOfLastDcfMessage + 60;

            if (interruptEdgeToggleDcf == 0) // Toggle edge interrupt If rising edge was set, switch to falling & vice versa
            {
                startTime = TA1R;
                P4IES &= ~BIT5;
                interruptEdgeToggleDcf = 1;
            }
            else
            {
                endTime = TA1R;
                currentBit = bitValue(calculateBitTransmitTime(startTime, endTime));
                P4IES |= BIT5;
                interruptEdgeToggleDcf = 0;
            }
            __no_operation();
            P4IFG &= ~BIT5;                             // Clear flag
            P4IE |= BIT5;                               // Enable interrupt
            break; // Pin 5
        case 0x0E: break; // Pin 6
        case 0x10: break; // Pin 7
        default: _never_executed();
    }
}

unsigned int calculateBitTransmitTime(unsigned int start, unsigned int stop)
{
    return stop - start;
}

bool bitValue(unsigned int bitTime)
{
    if (bitTime > bitTimeCycles1Lower && bitTime < bitTimeCycles1Upper)
    {
        return 1;
    }
    else if (bitTime > bitTimeCycles0Lower && bitTime < bitTimeCycles0Upper)
    {
        return 0;
    }
    return 0;

}
