/*
 * usart.c
 *
 *  Created on: 22 Jul 2026
 *      Author: vez767
 */

#include "stm32f4xx.h"

void USART2_Init(void){

RCC->APB1ENR |= (1U << 17); // USART 2
RCC->AHB1ENR |= (1U << 0);	// GPIOAEN

GPIOA->MODER &= ~(0xFU << 4);//  CLEAR PA2 & 3
GPIOA->MODER |= (0xAU << 4); // PA2 & PA3 - AF (1010)

GPIOA->AFR[0] &= ~(0xFFU << 8);
GPIOA->AFR[0] |= (0x77U << 8);

GPIOA->OSPEEDR &= ~(3U << 4);
GPIOA->OSPEEDR |= (2U << 4); // HIGH - 0b10

USART2->BRR = (138U << 4) | (14U << 0);

USART2->CR1 |= (1U << 13) | (1U << 3) | (1U << 2); // USARTEN [13] : TE [3] : RE [2]
}




