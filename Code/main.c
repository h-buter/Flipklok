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

char value;

uint32_t setTime;
uint32_t test;
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
    setupTimer0();
    setupTimer1();
    resetTimeKeeping();

    //For testing, setting time of mechanical clock
    uint32_t setMinute = 30;
    uint32_t setHour = 16;
    setTime = setHour * 3600 + setMinute * 60;
    storeTime(setTime);

    //wait time otherwise to quick between writing and reading from flash
    __delay_cycles(5000);

    mechanicalTimeInt = loadTime(); // Get the last stored time of the display
    timeOfLastDcfMessage = mechanicalTimeInt; // Set the time of DCF before any message received the same as mechanical time so the clock does not stand still at startup
    mechanicalTimeFloat = mechanicalTimeInt;
    UART_SendString("\r\n------Starting------\r\n");
    UART_SendString("mechanicalTime: ");
    UART_SendTime(mechanicalTimeFloat);

    __no_operation();

    __bis_SR_register(LPM3_bits | GIE);

}
