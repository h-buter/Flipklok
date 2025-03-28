/*
 * dcfReceive.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "dcfReceive.h"
#include "timeKeeping.h"

unsigned int calculateBitTransmitTime(bool checkSyncStatus, unsigned int start, bool startDir, unsigned int stop, bool stopDir)
{
    if (1 == checkSyncStatus) //currently checking for syncing status
    {

    }
    else
    {
        __no_operation();
        if (1 == startDir && 1 == stopDir) //Both on up direction
        {
            __no_operation();
            return stop - start;
        }
        else if (0 == startDir && 0 == stopDir) //Both on down direction
        {
            __no_operation();
            return start - stop;
        }
        else if (1 == startDir && 0 == stopDir) //start on up and stop on down direction
        {
            __no_operation();
            return (capacityTimer - start) + (capacityTimer - stop);
        }
        else if (0 == startDir && 1 == stopDir) //start on down and stop on up direction
        {
            __no_operation();
            return start + stop;
        }

        __no_operation(); //fallen trough
        return 0;

    }

}

bool bitValue(unsigned int bitTime)
{
    if (bitTime > bitTimeCycles1Lower && bitTime < bitTimeCycles1Upper)
    {
        __no_operation();
        P1OUT |= BIT7; //test
        return 1;
    }
    else if (bitTime > bitTimeCycles0Lower && bitTime < bitTimeCycles0Upper)
    {
        __no_operation();
        P1OUT &= ~BIT7; //test
        return 0;
    }

    __no_operation(); //fall trough
    return 0;

}

void storeBit(bool bit, unsigned int location)
{
    bitArray[location] = bit;
}

void decodeBitStream()
{


    __no_operation();

    unsigned int i;
    for(i = 0; i <= bitArrayLength; i++)
    {

    }
}
