/**
 * @file main.c
 * @brief Startup of system
 */

#include <msp430.h>
#include "pins.h"
#include "gpio.h"
#include "clock.h"
#include "timer.h"
#include "adc.h"
#include "stepperAdvance.h"
#include "fwdButton.h"
#include "dcfReceive.h"
#include "led.h"
#include "timeKeeping.h"
#include "pinInterrupts.h"
#include "uart.h"

#include "storeLoadMechanicalTime.h"

#warning "project > properties > buid > MSP430 compiler > optimization > optimization level"
#warning "(set to 1 for default, 0 for acces to non global variables while in debug mode)"

// Check if the INFOD section (0x1000 - 0x0040) is NOT within the main FLASH region
#if ((0x1000 >= __INFOA__) || (0x0040 < __FLASH_START__))
    #warning ("INFOD (0x1000 - 0x0040) is safe for user data storage.")
#else
    #error "Warning: INFOD (0x1000 - 0x0040) is part of FLASH! Do NOT use for data storage."
#endif

uint32_t setTime;
uint32_t loaded_value;

/**
 * @brief Main function that initializes system components and enters low-power mode.
 *
 * This function is responsible for setting up various system components, including
 * the GPIO, ADC, clocks, and timekeeping system. It also loads the mechanical time from flash memory (if it is connected to a debugger the flash is cleared and the mechanical time must be set manually).
 * It also initializes the UART (if enabled). Additionally, the function sets the time of the
 * mechanical clock and enters low-power mode with interrupts enabled. The system performs
 * various initialization tasks to ensure the proper operation of the application.
 *
 * It performs the following steps:
 * - Stops the watchdog timer to prevent system reset.
 * - Initializes UART if `UART_ENABLED` is defined.
 * - Sets up GPIO, ADC, clocks, and timekeeping.
 * - Writes the stored mechanical time from flash to UART.
 * - If the value is invalid because the debugger overwritten the flash, enters an infinite loop.
 * - Configures timers and enters low-power mode with interrupts.
 *
 * @note If UART is enabled, a warning will be displayed that the UART will be initialized.
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
    #ifdef UART_ENABLED
        #warning("Warning: UART is enabled! disable define of UART_ENABLED to disable it.")
	    UART_Init();  // Initialize UART
    #endif

	setupGpio();
	setupAdc();
	setupClocks();
    resetTimeKeeping();

    //For testing, setting time of mechanical clock
    uint32_t setMinute = 10;
    uint32_t setHour = 11;
    setTime = setHour * 3600 + setMinute * 60;
    write_SegD(setTime);                    // Write 32-bit value to segment C, increment value

    FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
    __delay_cycles(100000);
    loaded_value = load_from_flash(); // Load the value from flash
    if (loaded_value == 0xFFFFFFFF)
    {
        __delay_cycles(100000);
        __no_operation();
        while(1)
        {
            __no_operation();
        }
    }
    __delay_cycles(100000); //wait time otherwise to quick between writing and reading from flash
    mechanicalTimeFloat = loaded_value;
    timeOfLastDcfMessage = mechanicalTimeFloat; // Set the time of DCF before any message received the same as mechanical time so the clock does not stand still at startup
    #ifdef UART_ENABLED
        UART_SendString("\r\n------Starting------\r\n");
        UART_SendString("mechanicalTime: ");
        UART_SendTime(mechanicalTimeFloat);
    #endif

    setupTimer0();
    setupTimer1();

    __bis_SR_register(LPM3_bits | GIE);
}
