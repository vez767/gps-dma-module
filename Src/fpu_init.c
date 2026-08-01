/*
 * fpu_init.c
 *
 *  Created on: 12 May 2026
 *      Author: vez767
 */
#include "stm32f4xx.h"
#include <stdint.h>

#include "fpu_init.h"

void FPU_Init(void){
	SCB->CPACR |= (0xF << 20); // FPU Calculator
}

