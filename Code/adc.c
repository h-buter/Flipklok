/*
 * adc.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "adc.h"

volatile unsigned int ADCvar;

/// Interrupt routine for ADC
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (ADC10CTL0 & ADC10IFG)  // Check if ADC10 interrupt flag is set
    {
        ADC10CTL0 &= ~ADC10IFG; // Clear the interrupt flag
        ADCvar = ADC10MEM & 0xFF; // Read conversion result (masking to 8 bits)
        changeLedBrightness(ADCvar);
    }
}

/// Setup ADC
/// Shortest samling rate, with 2.5V reference, no prdivider, setup channels and enable conversion interrupts
void setupAdc()
{
    while (ADC10CTL1 & ADC10BUSY);  // Wait if ADC is busy

    // Configure ADC10
    ADC10CTL0 = ADC10ON | ADC10SHT_0 | REFON | REF2_5V;              // Turn on ADC10, set sampling time  4 ADC10CLK cycles, Reference generator on, Reference-generator voltage 2.5V
    ADC10CTL1 |= ADC10SSEL1;              // Use sampling timer and set clock source to ACLK
    ADC10CTL1 |= ADC10DIV_0;                      // Predivider. This bit predivides the selected ADC10_B clock source. 00b = Predivide by 1
    ADC10CTL1 |= INCH_4 | SREF_1;                   // Channel 4, reference: VR+ = VREF+ and VR- = AVSS

    ADC10CTL0 |= ENC;                          // Enable conversions
    ADC10CTL0 |= ADC10IE;                          // Enable ADC conv complete interrupt

    while ((ADC10CTL1 & ADC10BUSY));  // Wait if ADC is busy
}

/// Start a ADC conversion
void startAdcConv()
{
    ADC10CTL0 |= ADC10SC;                   // Start conversion-software trigger
}
