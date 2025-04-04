/*
 * adc.c
 *
 *  Created on: 21 mrt. 2025
 */

#include "adc.h"
#include "led.h"

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
    ADC10CTL0 &= ~ADC10IFG; // Clear the interrupt flag
    ADCvar = ADC10MEM; // Read conversion result
    changeLedBrightness(ADCvar);
}

/// Setup ADC
/// Shortest samling rate, with 2.5V reference, no prdivider, setup channels and enable conversion interrupts
void setupAdc()
{
    while (ADC10CTL1 & ADC10BUSY);  // Wait if ADC is busy

#warning ("TO-DO let ADC trigger by clock directly see SHSx on page 601 of manual");
    // Configure ADC10
    P1DIR &= ~BIT4;  // Set P1.4 as input
    ADC10CTL0 &= ~ENC;                          // Disable conversions to be able to setup ADC registers
    ADC10CTL0 = SREF_1 | ADC10SHT_0 | REF2_5V | REFON | ADC10ON;              // Reference: VR+ = VREF+ and VR- = AVSS, set sampling time  4 ADC10CLK cycles, reference-generator voltage 2.5V, reference generator on, turn on ADC10.
    ADC10CTL1 |= INCH_4 | ADC10DF;                    // Channel 4,
    ADC10AE0 |= INCH_4;
    ADC10CTL1 &= ~ADC10DF;  //Straight binary
    ADC10CTL1 |= ADC10DIV_0;                      // Predivider 00b = Predivide by 1
    ADC10CTL1 |= ADC10SSEL1;              // Use sampling timer and set clock source to ACLK

//    ADC10CTL0 |= ENC;                          // Enable conversions
    ADC10CTL0 |= ADC10IE;                          // Enable ADC conv complete interrupt

    while ((ADC10CTL1 & ADC10BUSY));  // Wait if ADC is busy
}

/// Start a ADC conversion
void startAdcConv()
{
    ADC10CTL0 |= ENC;
    ADC10CTL0 |= ADC10SC;                   // Start conversion-software trigger
}
