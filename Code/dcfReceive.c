/*
 * dcfReceive.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "dcfReceive.h"
#include "timeKeeping.h"

unsigned int testbitTimeCyclesStart = bitTimeCyclesSync;
unsigned int testbitTimeCyclesStartUpper = bitTimeCyclesSyncUpper;
unsigned int testbitTimeCyclesStartLower = bitTimeCyclesSyncLower;

void interruptDcf()
{
    static unsigned int count;
    static bool syncingStatus = 1;
    unsigned int timeDiv;
    if (1 == syncingStatus)
    {
       if (interruptEdgeToggleDcf == 0) // Triggered on falling edge, change edge interrupt
       {
           startTime = TA1R;
           countingDirectionStart = toggleTimer1Direction; // store counting direction when start time is taken

           P1IES &= ~BIT5;                 //Toggle edge interrupt low to high
           interruptEdgeToggleDcf = 1;
           P1OUT |= BIT7; //test pin
       }
       else // Triggered on falling edge, change edge interrupt and time can be calculated
       {
           endTime = TA1R; // Store current time
           countingDirectionEnd = toggleTimer1Direction; // store counting direction when end time is taken

           P1IES |= BIT5;                  //Toggle edge interrupt high to low
           interruptEdgeToggleDcf = 0;
           P1OUT &= ~BIT7; //test pin
           timeDiv = calculateBitTransmitTime(1, startTime, countingDirectionStart, endTime, countingDirectionEnd);


           __no_operation();
           if (timeDiv >= testbitTimeCyclesStartLower && timeDiv <= testbitTimeCyclesStartUpper)
           {
               __no_operation();
               syncingStatus = 0; //Sync bit is detected start receiving data bits
               storeBit(0, 0); // Currently on top of bit 0, this bit is always 0, so store it directly
               count = 1; // First bit is already stored so start at the second bit, bit 1.
               P1IES &= ~BIT5;                  //Toggle edge interrupt low to high
           }
       }
    }
    else //Sync is detected so readout data bits
    {
       if (interruptEdgeToggleDcf == 0) // Triggered on rising edge, change edge interrupt
       {
          startTime = TA1R;
          countingDirectionStart = toggleTimer1Direction; // store counting direction when start time is taken

          P1IES |= BIT5;                 //Toggle edge interrupt high to low
          interruptEdgeToggleDcf = 1;
          P1OUT |= BIT7; //test pin
       }
       else // Triggered on falling edge, change edge interrupt and time can be calculated
       {
          endTime = TA1R; // Store current time
          countingDirectionEnd = toggleTimer1Direction; // store counting direction when end time is taken

          P1IES &= ~BIT5;                  //Toggle edge interrupt low to high
          interruptEdgeToggleDcf = 0;
          P1OUT &= ~BIT7; //test pin
          timeDiv = calculateBitTransmitTime(0, startTime, countingDirectionStart, endTime, countingDirectionEnd);
          currentBit = bitValue(timeDiv); // Determine current bit value
          storeBit(currentBit, count);
          __no_operation();
          count++;
          if (bitArrayLength <= count) // Reset count if all bits of a message are received
          {
              __no_operation();
              bool errors = checkBitStream();
              if (0 == errors)
              {
                  __no_operation();
                  decodeBitStream();
              }
              __no_operation();
              P1IES |= BIT5;                 //Sync timing starts form high to low edge so change edge interrupt high to low
              syncingStatus = 1; // Start looking for sync again, count will be reseted there
          }
       }
    }
}

unsigned int calculateBitTransmitTime(bool checkSyncStatus, unsigned int start, bool startDir, unsigned int stop, bool stopDir)
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

//Checks if certain bits are 1 or 0 and if parity of minute and hour is correct
bool checkBitStream()
{
    __no_operation();
    // First bit needs to be 0, bit 20 needs to be 1, bit 59 needs to be 0
    if (1 == bitArray[0] || 0 == bitArray[20] || 1 == bitArray[59])
    {
        __no_operation();
        return 1; //transmission fault,
    }

    __no_operation();
    //Check parity minutes
    unsigned int i;
    unsigned int onesMinute = 0;
    for(i = 21; i <= 27; i++) //minutes
    {
        if(1 == bitArray[i])
        {
            onesMinute++;
        }
    }

    onesMinute = onesMinute + (int)bitArray[28]; // Add parity bit to even count, if the count is uneven the pariybit should make it even.

    __no_operation();
    if (onesMinute % 2) // if odd then transmission fault
    {
        __no_operation();
        return 1; //transmission fault no even parity minute
    }

    __no_operation();
    //Check parity hour
    unsigned int j;
    unsigned int onesHour = 0;
    for(j = 29; j <= 34; j++) //hour
    {
        if(1 == bitArray[j])
        {
            onesHour++;
        }
    }

    onesHour = onesHour + (int)bitArray[35]; // Add parity bit to even count, if the count is uneven the pariybit should make it even.

    __no_operation();
    if (onesHour % 2) // if odd then transmission fault
    {
        __no_operation();
        return 1; //transmission fault no even parity hour
    }

    __no_operation();
    return 0; // No errors found
    //other parity for date needs no checking because it is not used

}

void decodeBitStream()
{











//    for(i = 0; i <= bitArrayLength; i++)
//    {
//
//    }
}
