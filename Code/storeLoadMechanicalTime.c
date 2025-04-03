/*
 * storeLoadMechanicalTime.c
 *
 *  Created on: 1 apr. 2025
 */

#include "storeLoadMechanicalTime.h"

#include <msp430.h>


void storeTime(uint32_t  data)
{
    __no_operation();
    uint16_t *flashPtr = (uint16_t *)INFO_SEGMENT_ADDRESS; // Pointer to flash memory (word-aligned)

    // Step 1: Unlock Flash & Erase Segment (Flash requires an erase before write)
    FCTL3 = FWKEY ;         // Clear lock bit
    FCTL1 = FWKEY  + ERASE; // Set erase mode
    *flashPtr = 0;             // Dummy write to erase segment

    // Wait for erase to complete
    while (FCTL3 & BUSY); // Wait until flash is no longer busy

    // Step 2: Enable Write Mode
    FCTL1 = FWKEY + WRT;

    // Step 3: Write 32-bit data (two 16-bit writes)
    flashPtr[0] = (uint16_t)(data & 0xFFFF);       // Write lower 16 bits
    flashPtr[1] = (uint16_t)((data >> 16) & 0xFFFF); // Write upper 16 bits

    // Wait for the write to complete
    while (FCTL3 & BUSY); // Wait until flash is no longer busy

    // Step 4: Lock Flash
    FCTL1 = FWKEY;          // Disable write mode
    FCTL3 = FWKEY + LOCK;   // Lock flash
}

uint32_t  loadTime()
{
    uint16_t *flashPtr = (uint16_t *)INFO_SEGMENT_ADDRESS;

    // Read back 32-bit value from flash (two 16-bit reads that are concatenated)
    volatile uint32_t data = (uint32_t)flashPtr[0] | ((uint32_t)flashPtr[1] << 16);
    __no_operation();
    return data;
}



