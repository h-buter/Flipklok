/*
 * adc.c
 *
 *  Created on: 21 mrt. 2025
 */
/**
 * @file adc.c
 * @brief implementation of the reading and converting the ADC values
 */

#include "adc.h"
#include "led.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "dcfReceive.h"

volatile unsigned int ADCvar;
volatile unsigned char currentChannel = 0;
volatile double voltReading;

/**
 * @brief ADC interrupt routine, gets called when end interrupt of ISR_TA0(void) is reached via startAdcConv().
 *
 */
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
    adcResults[currentChannel] = ADC10MEM; // Read out the result of the current pin
    #if !defined(UART_ENABLED)
        // Update the ADC input channel (INCH_x)
        ADC10CTL0 &= ~ENC; // Disable conversion before changing channel
        switch (currentChannel)
        {
            case 0: // P1.4 (LDR):
                ADC10CTL1 = (ADC10CTL1 & ~INCH_4) | INCH_1;
                changeLedBrightness(adcResults[0]);
                break;
            case 1: // P1.1 (Battery):
                ADC10CTL1 = (ADC10CTL1 & ~INCH_1) | INCH_2;
                break;
            case 2: // P1.2 (Ignition):
                ADC10CTL1 = (ADC10CTL1 & ~INCH_2); // No more ADC conversions until startAdcConv() is called again
                voltReading = convertAdc2Voltage(18, 75, adcResults[currentChannel]);
//                if (voltReading > 12.0) // Switched on
//                {
//                    wakeUp();
//                    volatile int k = 0;
//                    __no_operation();
//                }
//                else
//                {
//                    sleep();
//                    countDcf77Messages = 0;
//                    volatile int k = 0;
//                    __no_operation();
//                }
                break;
        }

        currentChannel++; // Move to next channel
        if (currentChannel >= 3)
        {
            currentChannel = 0; //stop starting ADC conversions until startAdcConv() is called again
        }
        else
        {
            ADC10CTL0 |= ENC; // Enable conversions
            ADC10CTL0 |= ADC10SC; // Start next conversion
        }
    #else
        changeLedBrightness(adcResults[0]);
    #endif
}

/**
 * @brief Setup ADC, get called in startup from main(), Shortest samling rate, with 2.5V reference, no prdivider, setup channels and enable conversion interrupts
 */
void setupAdc()
{
    while (ADC10CTL1 & ADC10BUSY);  // Wait if ADC is busy

#warning ("TO-DO let ADC trigger by clock directly see SHSx on page 601 of manual");
    // Configure ADC10


    ADC10CTL0 &= ~ENC;                          // Disable conversions to be able to setup ADC registers
    ADC10CTL0 = SREF_1 | ADC10SHT_0 | REF2_5V | REFON | ADC10ON;              // Reference: VR+ = VREF+ and VR- = AVSS, set sampling time  4 ADC10CLK cycles, reference-generator voltage 2.5V, reference generator on, turn on ADC10.
    ADC10CTL1 |= INCH_4 | ADC10DF; // First select channel 4
    #ifdef UART_ENABLED
        P1DIR &= ~BIT4;  // Set P1.4 as input: LDR
        ADC10AE0 |= INCH_4; // enable as analog inputs
    #else
        P1DIR &= ~BIT4;  // Set P1.4 as input: LDR
        P1DIR &= ~BIT1;  // Set P1.1 as input: Battery
        P1DIR &= ~BIT2;  // Set P1.2 as input: Ignition switch
        ADC10AE0 |= INCH_4 | INCH_1 | INCH_2; // enable as analog inputs
    #endif
    ADC10CTL1 &= ~ADC10DF;  //Straight binary
    ADC10CTL1 |= ADC10DIV_0;                      // Predivider 00b = Predivide by 1
    ADC10CTL1 |= ADC10SSEL1;              // Use sampling timer and set clock source to ACLK

//    ADC10CTL0 |= ENC;                          // Enable conversions
    ADC10CTL0 |= ADC10IE;                          // Enable ADC conv complete interrupt

    while ((ADC10CTL1 & ADC10BUSY));  // Wait if ADC is busy
}

/**
 * @brief Start the ADC conversion'(s), gets called when end interrupt of ISR_TA0(void) is reached
 */
void startAdcConv()
{
    ADC10CTL0 &= ~ENC; // Disable before editing control registers
    ADC10CTL1 |= INCH_4; // First select channel 4
    ADC10CTL0 |= ENC;
    ADC10CTL0 |= ADC10SC;                   // Start conversion-software trigger
}

/**
 * @brief Convert the adc reading to a voltage before voltage divider
 *
 * @return double voltage
 */
double convertAdc2Voltage(int R1, int R2, int adcReading)
{
    double voltage = referenceVoltage / adcMax * adcReading; //convert ADC reading to voltage on pin, using 2.5V reference
    voltage = (voltage * (R1 + R2)) / R1; // convert voltage to actual voltage as input on the voltage divider
    return voltage;
}
