/*
 * storeLoadMechanicalTime.c
 *
 *  Created on: 1 apr. 2025
 */
/**
 * @file storeLoadMechanicalTime.c
 * @brief Setup and control of flash memory to store the mechanical time
 */

#include "storeLoadMechanicalTime.h"

#include <msp430.h>

#define INFO_SEGMENT_ADDRESS 0x1000  // Info Memory segment D, see page 319 of SLAU144K

//void storeTime(uint32_t  data)
//{
//    __no_operation();
//    uint16_t *flashPtr = (uint16_t *)INFO_SEGMENT_ADDRESS; // Pointer to flash memory (word-aligned)
//
//    // Step 1: Unlock Flash & Erase Segment (Flash requires an erase before write)
//    FCTL3 = FWKEY;         // Clear lock bit
//    FCTL1 = FWKEY  + ERASE; // Set erase mode
//    *flashPtr = 0;             // Dummy write to erase segment
//
//    // Wait for erase to complete
//    while (FCTL3 & BUSY); // Wait until flash is no longer busy
//
//    // Step 2: Enable Write Mode
//    FCTL1 = FWKEY + WRT;
//
//    // Step 3: Write 32-bit data (two 16-bit writes)
//    flashPtr[0] = (uint16_t)(data & 0xFFFF);       // Write lower 16 bits
//    flashPtr[1] = (uint16_t)((data >> 16) & 0xFFFF); // Write upper 16 bits
//
//    // Wait for the write to complete
//    while (FCTL3 & BUSY); // Wait until flash is no longer busy
//
//    // Step 4: Lock Flash
//    FCTL1 = FWKEY;          // Disable write mode
//    FCTL3 = FWKEY + LOCK;   // Lock flash
//}
//
//uint32_t  loadTime()
//{
//    uint16_t *flashPtr = (uint16_t *)INFO_SEGMENT_ADDRESS;
//
//    // Read back 32-bit value from flash (two 16-bit reads that are concatenated)
//    volatile uint32_t data = (uint32_t)flashPtr[0] | ((uint32_t)flashPtr[1] << 16);
//    __no_operation();
//    return data;
//}



/**
 * @brief Function to write a 32-bit value to Flash segment D for storing the mechanical time, gets called from main() and at the end of stepperAdvance()
 *
 * @param value the value to be stored in flash
 * @return void
 */
void write_SegD(uint32_t value)
{
  uint32_t *Flash_ptr;                          // Flash pointer
  unsigned int i;

  Flash_ptr = (uint32_t *) 0x1000;             // Initialize Flash pointer (for 32-bit writes)
  FCTL1 = FWKEY + ERASE;                       // Set Erase bit
  FCTL3 = FWKEY;                               // Clear Lock bit
  *Flash_ptr = 0;                              // Dummy write to erase Flash segment

  FCTL1 = FWKEY + WRT;                         // Set WRT bit for write operation

  *Flash_ptr--;
  for (i = 0; i < 8; i++)                     // Write 32-bit value to flash (16 writes per 64 bytes)
  {
    *Flash_ptr = value;                      // Write 32-bit value to flash
    *Flash_ptr++;
  }

//  *Flash_ptr = value;                      // Write 32-bit value to flash

  FCTL1 = FWKEY;                               // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                        // Set LOCK bit
}

/**
 * @brief Function to load a 32-bit value from Flash memory segment D used for restoring the mechanical time on startup, gets called from main().
 *
 * @return uint32_t the value that is stored in flash
 */
uint32_t load_from_flash(void)
{
  uint32_t *Flash_ptr;                         // Pointer to read from Flash memory
  uint32_t loaded_value;

  Flash_ptr = (uint32_t *) 0x1000;             // Point to the start of Flash segment D
  loaded_value = *Flash_ptr;                   // Read the 32-bit value from Flash

  return loaded_value;
}

