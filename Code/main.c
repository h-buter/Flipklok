#include <msp430.h>
#include "pins.h"
#include "gpio.h"
#include "clock.h"
#include "timer.h"
#include "adc.h"
#include "stepperAdvance.h"
#include "fwdButton.h"
#include "led.h"

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	setupGpio();
	setupAdc();
	setupClocks();
    setupTimer0();

    // Interrupt setup
    __bis_SR_register(LPM3_bits | GIE);
}
