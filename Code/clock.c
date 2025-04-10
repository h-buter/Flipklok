/*
 * clock.c
 *
 *  Created on: 20 mrt. 2025
 */
/**
 * @file clock.c
 * @brief Setup of system clocks
 */
#include "clock.h"

/**
 * @brief Setup clocks of system, all to LFXTAL input, get called from main()
 *
 * @return void
 */
void setupClocks()
{
    BCSCTL1 |= XT2OFF;              // Disable XT2
    BCSCTL1 &= ~XTS;                //LFXT1
    BCSCTL1 |= DIVA_0;              // ACLK Divider 0: /1

//    BCSCTL2 |= SELM_3;                    // LFXT1CLK,
//    BCSCTL2 |= DIVM_0;                    // MCLK Divider 0: /1, LFXT1CLK,  SMCLK Divider 0: /1
    BCSCTL2 |= SELS;                        // LFXT1CLK,  SMCLK Divider 0: /1
    BCSCTL2 |= DIVS_0;                      // SMCLK Divider 0: /1
    BCSCTL3 |=  LFXT1S_0;                   // Mode 0 for LFXT1
    BCSCTL3 |=  XCAP_2;                     // Approximately 10 pF

    //Enable when running cpu on 1 MHz instead of LFXTAL
    BCSCTL1 |= CALBC1_1MHZ;  // Load calibrated DCO setting for 1 MHz
    DCOCTL |= CALDCO_1MHZ;   // Load fine-tuned DCO setting
    BCSCTL2 |= SELM_0;  // MCLK = DCO
    BCSCTL2 |= DIVM_0;  // No division


    // 3. Wait for the crystal to stabilize
    while (IFG1 & OFIFG) {       // Check oscillator fault flag
        IFG1 &= ~OFIFG;          // Clear oscillator fault flag
        __delay_cycles(1000);   // Wait for the crystal to stabilize
    }
}
