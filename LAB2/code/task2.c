/*
 * task2.c
 *
 *  Created on: Sep 22, 2021
 *      Author: wangy62
 */

#include "init.h"

void Init_GPIO();
void Init_Timer();

volatile uint16_t timeUpdated = 0;	// Overflow Counter
uint32_t Time_ms = 0;	//

int main() {
	Sys_Init();
	Init_Timer();

	printf("\033[2J\033[;H");
	printf("start\r\n");

	while(1);

}

void Init_Timer() {
	// Open NVIC interrupt for TIM6 position 54
	NVIC->ISER[1] = (uint32_t) 1 << (22);

	// Enable TIM6 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	asm ( "nop" );
	asm ( "nop" );
	// Set pre-scaler to slow down ticlks
	TIM6->PSC = 0x2A2F;	//PSC = 10799
	// Set the Auto-reload Value for 10Hz overflow
	TIM6->ARR = 0x03E9;	//ARR = 999
	// Generate update events to auto reload.
	TIM6->EGR |= 0x0001;
	// Enable Update Interrupts.
	TIM6->DIER |= 0x0001;
	// Start the timer.
	TIM6->CR1 |= 0x0001;
}

void TIM6_DAC_IRQHandler() {
	// Clear Interrupt Bit
	TIM6->SR &= 0xFFFE;
	asm ( "nop" );
	asm ( "nop" );
	// Update the timer counter for overflows
	timeUpdated ++;
	Time_ms = timeUpdated*100;	// calculate actual time in ms
	printf("\033[2;0H");
	printf("overflow occured, time %dms\r\n", Time_ms);

}
