/**
 * @file stepperAdvance.c
 * @brief Setup and control of stepper motor
 */

#include "stepperAdvance.h"
#include "timeKeeping.h"
#include "uart.h"
#include "storeLoadMechanicalTime.h"
#include "pinInterrupts.h"

/**
 * @brief Advances the stepper each time it gets called by one full step gets triggered by ISR_TA0().
 *
 * Step sequence:
 *  - 0001
 *  - 0010
 *  - 0100
 *  - 1000
 * @return void
 */
int stepperArray[] = {BIT0, BIT3, BIT4, BIT5};
void stepperAdvance()
{
    if (P1IN & BIT0) // if 5V converter is on turn stepper
    {
        toggleCalculateTimeDifference = 0;          // disable triggering time difference function in timer interrupt.
        static unsigned int coil;
        if (stepsRemaining > 0 || toggleFwdInterrupt == 1)
        {
            P2OUT &= ~(STEPPER1 + STEPPER2 + STEPPER3 + STEPPER4);
            P2OUT |= stepperArray[coil];
            if (coil == 3)
            {
                P2OUT |= stepperArray[0];
            }
            else
            {
                P2OUT |= stepperArray[coil + 1];
            }

            coil++;
            if (coil == 4)
            {
                P2OUT |= stepperArray[0];
                coil = 0;
            }


            if (toggleFwdInterrupt == 0)
            {
                stepsRemaining--;
                mechanicalTimeFloat = mechanicalTimeFloat + timePerStep; //Increment mechanical time keeping counter with 1 step time

                if(mechanicalTimeFloat > 86400)
                {
                    mechanicalTimeFloat = 0; //reset after 24:00
                    #ifdef UART_ENABLED
                        UART_SendString("mechTime reset\r\n");
                    #endif
                }

                if((stepsRemaining > 999) && (0 == (stepsRemaining % 1000))) //every 1000 steps print steps remaining
                {
                    #ifdef UART_ENABLED
                        UART_SendString("s: ");
                        UART_SendUint32(stepsRemaining);
                        UART_SendString(", MTC: ");
                        UART_SendTime(mechanicalTimeFloat);
                        UART_SendString("\r\n");
                    #endif
                }
            }
        }
        else
        {
            TA0CCTL2 &= ~CCIE;                          // TACCR2 interrupt stepperAdvance disable
//            storeTime((uint32_t)mechanicalTimeFloat); // Store the current state of the clock in non volatile flash
            write_SegD((uint32_t)mechanicalTimeFloat);
            toggleCalculateTimeDifference = 1;          // enable triggering time difference function in timer interrupt.
            toggleFwdInterruptISR = 1;                     // Enable fwdButton interrupt again (disabled in timekeeping function when it is syncing)
        }
    }
}
