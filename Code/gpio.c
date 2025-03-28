/*
 * gpio.c
 *
 *  Created on: 20 mrt. 2025
 */

#include "gpio.h"

/// Setup of all GPIO's on startup
void setupGpio()
{
    //Enable pull-up on PCB
    P1DIR |= BIT3;
    P1OUT |= BIT3;

    P2OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4); // Clear stepper pins output latch for a defined power-on state
    P2DIR |= STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4; // Set stepper pins to output direction

    P2SEL  |= BIT6 | BIT7;   // Select XIN and XOUT functionality
    P2SEL2 &= ~(BIT6 | BIT7); // Ensure secondary function is not selected

    //fwdButton
    P2DIR &= ~BIT1;                             // Input
//    P1REN |= BIT1;                              // Enable pull
//    P1OUT |= BIT1;                              // Enable pull up
    P2IES &= ~BIT1;                              // low to high
    P2IFG &= ~BIT1;                             // Clear flag
    P2IE |= BIT1;                               // Enable interrupt

    //led for lightning the clock
    P1OUT &= ~BIT6;                             //Turn off LED0
    P1DIR |= BIT6;                              //LED0 as output

    //Indicator L
    P1DIR |= BIT7;
    P1OUT &= ~BIT7;                             //Turn off


    //EX-btn, dcf button for testing
//    P2DIR &= ~BIT2;                             // Input
////    P2REN |= BIT2;                              // Enable pull
////    P2OUT |= BIT2;                              // Enable pull up
//    P2IES &= ~BIT2;                              // low to high
//    P2IFG &= ~BIT2;                             // Clear interrupt flag
//    __delay_cycles(1000);                       // Small delay for stabilization otherwise a trigger is registered on boot up of dev board
//    P2IFG &= ~BIT2;                             // Clear it again
//    P2IE |= BIT2;                               // Enable interrupt


    //DCF77 input
    P1DIR &= ~BIT5;                             // Input
//    P2REN |= BIT2;                              // Enable pull
//    P2OUT |= BIT2;                              // Enable pull up
    P1IES &= ~BIT5;                              // low to high
    P1IFG &= ~BIT5;                             // Clear interrupt flag
    __delay_cycles(1000);                       // Small delay for stabilization otherwise a trigger is registered on boot up of dev board
    P1IFG &= ~BIT5;                             // Clear it again
    P1IE |= BIT5;                               // Enable interrupt


    //Test pin LDR as output test pin
    P1DIR |= BIT4;
    P1OUT &= ~BIT4;                             //Turn off




    __enable_interrupt();  // Enable global interrupts
}
