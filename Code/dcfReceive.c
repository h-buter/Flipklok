/*
 * dcfReceive.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "dcfReceive.h"
#include "timeKeeping.h"
#include "uart.h"

unsigned int testbitTimeCyclesStart = bitTimeCyclesSync;
unsigned int testbitTimeCyclesStartUpper = bitTimeCyclesSyncUpper;
unsigned int testbitTimeCyclesStartLower = bitTimeCyclesSyncLower;

/// Time the DCF77 pulses.
/// Wait first for sync pulse than read out each of 59 bits
/// Determine the value of the bit with: bool bitValue() based on the calculated transmit time with: unsigned int calculateBitTransmitTime()
/// Store the result via: storeBit(), when all 59 bits are received check with: checkBitStream(), and decode to time: decodeBitStream2Seconds()
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
       }
       else // Triggered on falling edge, change edge interrupt and time can be calculated
       {
           endTime = TA1R; // Store current time
           countingDirectionEnd = toggleTimer1Direction; // store counting direction when end time is taken

           P1IES |= BIT5;                  //Toggle edge interrupt high to low
           interruptEdgeToggleDcf = 0;
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
//          P1OUT |= BIT7; //test pin
       }
       else // Triggered on falling edge, change edge interrupt and time can be calculated
       {
          endTime = TA1R; // Store current time
          countingDirectionEnd = toggleTimer1Direction; // store counting direction when end time is taken

          P1IES &= ~BIT5;                  //Toggle edge interrupt low to high
          interruptEdgeToggleDcf = 0;
//          P1OUT &= ~BIT7; //test pin
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
                  decodeBitStream2Seconds();
              }
              __no_operation();
              P1IES |= BIT5;                 //Sync timing starts form high to low edge so change edge interrupt high to low
              syncingStatus = 1; // Start looking for sync again, count will be reseted there
          }
       }
    }
}

/// Calculate the time it took between the rising and falling edge of the DCF77 pulse
/// Take in to account the up and down behavior of the timer
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

/// Determine the bit value of the transmitted DCF77 bit and return it
bool bitValue(unsigned int bitTime)
{
    if (bitTime > bitTimeCycles1Lower && bitTime < bitTimeCycles1Upper)
    {
        __no_operation();
        return 1;
    }
    else if (bitTime > bitTimeCycles0Lower && bitTime < bitTimeCycles0Upper)
    {
        __no_operation();
        return 0;
    }

    __no_operation(); //fall trough
    return 0;

}

/// Store the received DCF77 bit in the array
void storeBit(bool bit, unsigned int location)
{
    bitArray[location] = bit;
}

/// Checks if certain predetermined bits are 1 or 0 and if parity of minute and hour is correct
bool checkBitStream()
{
    __no_operation();
    // First bit needs to be 0, bit 20 needs to be 1, bit 59 needs to be 0
    if (1 == bitArray[0] || 0 == bitArray[20] || 1 == bitArray[59])
    {
        __no_operation();
        UART_SendString("RX bit fault\r\n");
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
        UART_SendString("RX fault no parity minute\r\n");
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
        UART_SendString("RX fault no parity hour\r\n");
        return 1; //transmission fault no even parity hour
    }

    __no_operation();
    return 0; // No errors found
    //other parity for date needs no checking because it is not used

}

/// Decode the received DCF77 bitstream to a time
/// Also ensure that the received time is plausible
void decodeBitStream2Seconds()
{
    static const unsigned int bcdValues[] = {1, 2, 4, 8, 10, 20, 40, 80};
    __no_operation();

    volatile unsigned int minuteTemp = 0;
    volatile unsigned int hourTemp = 0;

    bool error = 0;

    unsigned int i;

    //minute
    for(i = 21; i <= 27; i++) //convert to if else instead of multiply probaly faster
    {
        minuteTemp = minuteTemp + (bcdValues[i-21] * bitArray[i]); // Convert from 7 bit BCD to a integer
    }

    __no_operation();
    if (minuteTemp > 60) // there can't be more than 60 minutes in a hour
    {
        __no_operation();
        UART_SendString("More than 60 minutes fault\r\n");
        error = 1;
    }

    //hour
    for(i = 29; i <= 34; i++)
    {
        hourTemp = hourTemp + (bcdValues[i-29] * bitArray[i]); // Convert from 6 bit BCD to a integer
    }

    __no_operation();
    if (hourTemp > 24) // there can't be more than 24 hours in a day
    {
        __no_operation();
        UART_SendString("More than 24 hours fault\r\n");
        error = 1;
    }

    if (0 == error) // No errors
    {
        minuteDcfLast = minuteTemp;
        hourDcfLast = hourTemp;
        // Convert to seconds passed midnight
        timeOfLastDcfMessage = minuteDcfLast * 60 + hourDcfLast * 3600;


        countDcf77Messages++; // Increment counter, used for checking if there was ever a message received, also for statistics
        timeSinceLastCompleteDcfMessage = 0; // Reset time since last message received, current DCF message syncs the clock

        UART_SendString("DCF: ");
        UART_SendInt(hourDcfLast);
        UART_SendString(":");
        UART_SendInt(minuteDcfLast);
        UART_SendString("\r\n");
    }
    __no_operation();
}
