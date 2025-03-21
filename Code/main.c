#include <msp430.h>
#include "pins.h"
#include "gpio.h"
#include "clock.h"
#include "timer.h"
#include "adc.h"
#include "stepperAdvance.h"
#include "fwdButton.h"
#include "led.h"

#warning "project > properties > buid > MSP430 compiler > optimization > optimization level"
#warning "(set to 1 for default, 0 for acces to non global variables while in debug mode)"

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	setupGpio();
	setupAdc();
	setupClocks();
    setupTimer0();
    setupTimer1();

    // Interrupt setup
    __bis_SR_register(LPM3_bits | GIE);
}
