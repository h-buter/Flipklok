/*
 * gpio.c
 *
 *  Created on: 20 mrt. 2025
 */
/**
 * @file gpio.c
 * @brief Setup of gpio pins on: startup, sleep and wakeup
 */

#include "gpio.h"
#include "pinInterrupts.h"
#include "timer.h"

/**
 * @brief Setup of all GPIO's on startup, gets called from main()
 *
 * @return void
 */
void setupGpio()
{
    //Enable pull-up on PCB
    P1DIR |= BIT3;
    P1OUT |= BIT3;

    //Stepper pins
    P2OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4); // Clear stepper pins output latch for a defined power-on state
    P2DIR |= STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4; // Set stepper pins to output direction

    setupClockIO();

    //fwdButton
    P2DIR &= ~BIT1;                             // Input
//    P1REN |= BIT1;                              // Enable pull
//    P1OUT |= BIT1;                              // Enable pull up
    P2IES &= ~BIT1;                              // low to high
    P2IFG &= ~BIT1;                             // Clear flag
    P2IE |= BIT1;                               // Enable interrupt
    toggleFwdInterruptISR = 1;                     // Enable the ISR

    //LED for lightning the clock
    P1DIR |= BIT6;                              //LED0 as output
    P1OUT &= ~BIT6;                             //Turn off LED0

    //Indicator L
    P1DIR |= BIT7;                              // Output
    P1OUT &= ~BIT7;                             //Turn off

    //EX-btn
    P2DIR &= ~BIT2;                             // Input
//    P2REN |= BIT2;                              // Enable pull
//    P2OUT |= BIT2;                              // Enable pull up
    P2IES &= ~BIT2;                              // low to high
    P2IFG &= ~BIT2;                             // Clear interrupt flag
//    __delay_cycles(1000);                       // Small delay for stabilization otherwise a trigger is registered on boot up of dev board
//    P2IFG &= ~BIT2;                             // Clear it again
    P2IE |= BIT2;                               // Enable interrupt

    //DCF77 input
    P1DIR &= ~BIT5;                             // Input
    P1IES |= BIT5;                              // high to low, first trigger on a low going pulse to detect 1800 ms sync pulse after bit 58
    P1IFG &= ~BIT5;                             // Clear interrupt flag
    __delay_cycles(1000);                       // Small delay for stabilization otherwise a trigger is registered on boot up of dev board
    P1IFG &= ~BIT5;                             // Clear it again
    P1IE |= BIT5;                               // Enable interrupt

    //5V converter
    P1DIR |= BIT0;      // Output
    P1OUT &= ~BIT0;      //Turn off

    //See for ADC pin setup adc.c

    __enable_interrupt();  // Enable global interrupts
}

/**
 * @brief Setup of all GPIO's after wakeup, gets called from ADC10_ISR(void)
 *
 * @return void
 */
void wakeUp()
{
    setupGpio();
    P1OUT |= BIT0;      //Turn on 5v supply so stepper can spin
    wakeUpTimer0();
}

/**
 * @brief Setup of all GPIO's after sleep, gets called from ADC10_ISR(void)
 *
 * @return void
 */
void sleep()
{
    // Configure GPIOs to it's lowest power state
    sleepTimer0();
    P2IE &= ~BIT1;                               // Disable interrupt fwdButton
    P2IE &= ~BIT2;                               // Disable interrupt exButton
    P1IE &= ~BIT5;                               // Disable interrupt DCF77

    P1OUT = 0;                                // All P1.x reset
    P1DIR = 0xFF;                             // All P1.x outputs
    P2OUT = 0;                                // All P2.x reset
    P2DIR = 0xFF;                             // All P2.x outputs
    P3OUT = 0;                                // All P3.x reset
    P3DIR = 0xFF;                             // All P3.x outputs

    P2DIR &= ~BIT1;                             // Input fwdButton
    P2DIR &= ~BIT2;                             // Input exButton
    P1DIR &= ~BIT5;                             // Input DCF77

    //Keep ADC pins as input
    P1DIR &= ~BIT1;  // Set P1.1 as input: Battery
    P1DIR &= ~BIT2;  // Set P1.2 as input: Ignition switch

    setupClockIO();
}

/**
 * @brief Setup of the external clock pins, gets called from setupGpio(), wakeup(), sleep()
 *
 * @return void
 */
void setupClockIO()
{
    //Clock
    P2SEL  |= BIT6 | BIT7;   // Select XIN and XOUT functionality
    P2SEL2 &= ~(BIT6 | BIT7); // Ensure secondary function is not selected
}
