/*
 * uart.c
 *
 *  Created on: 2 apr. 2025
 */
/**
 * @file uart.c
 * @brief Setup and control of UART communication
 */

#include "uart.h"
#include "dcfReceive.h"

/**
 * @brief Setup UART communication, gets called from main()
 *
 */
void UART_Init(void)
{
    #ifdef UART_ENABLED
        P1SEL |= BIT1 + BIT2;   // Set P1.1 (RX) and P1.2 (TX) for UART function
        P1SEL2 |= BIT1 + BIT2;

        UCA0CTL1 |= UCSSEL_2;   // Use SMCLK
        UCA0BR0 = 13;          // 2400 baud rate
        UCA0BR1 = 0;
        UCA0MCTL = UCBRS0;      // Modulation UCBRSx = 1
        UCA0CTL1 &= ~UCSWRST;   // Initialize USCI state machine
    //    IE2 |= UCA0RXIE;        // Enable RX interrupt
    #endif
}

/**
 * @brief send a single character via UART to host system
 * @param[in] c The character to send
 */
void UART_SendChar(char c)
{
    #ifdef UART_ENABLED
        while (!(IFG2 & UCA0TXIFG));  // Wait until buffer is empty
        UCA0TXBUF = c;  // Send character
    #endif
}

/**
 * @brief send a full string to host system
 * @param[out] *str Pointer to the string
 */
void UART_SendString(char *str)
{
    #ifdef UART_ENABLED
        while (*str)
        {
            UART_SendChar(*str++);
        }
    #endif
}

/**
 * @brief send a 16 bit integer to host system
 * @param[in] num The integer to send
 */
void UART_SendInt(int32_t num)
{
    #ifdef UART_ENABLED
        char buffer[12];  // Enough for "-2147483648\0"
        int i = 0;
        uint8_t isNegative = 0;

        if (num < 0)
        {
            isNegative = 1;
            num = -num;
        }

        // Convert integer to string (reverse order)
        do
        {
            buffer[i++] = (num % 10) + '0';
            num /= 10;
        } while (num > 0);

        if (isNegative)
        {
            buffer[i++] = '-';
        }

        buffer[i] = '\0';

        // Reverse the string and send
        int j;
        for (j = i - 1; j >= 0; j--)
        {
            UART_SendChar(buffer[j]);
        }
    #endif
}

/**
 * @brief send a 32 bit integer to host system
 * @param[in] num The integer to send
 */
void UART_SendUint32(uint32_t num)
{
    #ifdef UART_ENABLED
        char buffer[11];  // Enough for "4294967295\0"
        int i = 0;

        // Convert integer to string (reverse order)
        do
        {
            buffer[i++] = (num % 10) + '0';
            num /= 10;
        } while (num > 0);

        buffer[i] = '\0';

        // Reverse the string and send
        int j;
        for (j = i - 1; j >= 0; j--)
        {
            UART_SendChar(buffer[j]);
        }
    #endif
}

/**
 * @brief Sends a time formated in hours and minutes to host system
 * @param[in] seconds The time in seconds to send
 */
void UART_SendTime(uint32_t seconds)
{
    #ifdef UART_ENABLED
        int hours = (seconds / 3600) % 24;   // Get hours (mod 24 to avoid overflow)
        int minutes = (seconds % 3600) / 60; // Get minutes

    //    UART_SendString("time: ");
        UART_SendInt(hours);
        UART_SendString(":");
        if (minutes <= 9)
        {
            UART_SendString("0"); // send a zero first other wise it prints xx:y instead of xx:yy
        }
        UART_SendInt(minutes);
        UART_SendString("\r\n");
    #endif
}

/**
 * @brief Sends a the received DCF bitstream to host system
 * @param[out] arr Pointer to the bitstream array
 * @param[in] length The length of the array
 */
void UART_Bitstream(const bool* arr, unsigned int length)
{
    #ifdef UART_ENABLED
        unsigned int i;
        for (i = 0; i < length; i++)
        {
            char c = arr[i] ? '1' : '0';  // convert to ASCII
            UART_SendChar(c);
        }
        UART_SendString("\r\n");
    #endif
}

