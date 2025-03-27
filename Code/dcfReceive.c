/*
 * dcfReceive.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "dcfReceive.h"
#include "timeKeeping.h"

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
