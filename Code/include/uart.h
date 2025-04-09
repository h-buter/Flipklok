/*
 * uart.h
 *
 *  Created on: 2 apr. 2025
 */

#ifndef UART_H_
#define UART_H_

#include <msp430.h>
#include <stdint.h>

//#define UART_ENABLED //Enables UART

void UART_Init(void);
void UART_SendChar(char c);
void UART_SendString(char *str);

void UART_SendUint32(uint32_t num);
void UART_SendInt(int32_t num);

void UART_SendTime(uint32_t seconds);



#endif /* UART_H_ */
