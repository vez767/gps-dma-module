/*
 * usart.h
 *
 *  Created on: 22 Jul 2026
 *      Author: Windows
 */

#ifndef USART_H_
#define USART_H_

#include "stm32f4xx.h"

void USART2_Init(void);
void USART2_Write(int ch);
void USART2_Task_Init(void);


#endif /* USART_H_ */
