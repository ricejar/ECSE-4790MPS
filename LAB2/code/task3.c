/*
 * task3.c
 *
 *  Created on: Sep 23, 2021
 *      Author: wangy62
 */


#include "init.h"

//void blinkScreen();
void Init_GPIO();
void Init_Timer();

TIM_HandleTypeDef htim6;

volatile uint16_t timeUpdated = 0;	// Overflow counter
uint32_t Time_ms = 0;

int main() {
	Sys_Init();
	Init_Timer();

	printf("\033[2J\033[;H");
	printf("start\r\n");

	// Infinite loop
	while(1);
}


void Init_Timer() {
	// initialize the pointer to set timer 6
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 5399;
	htim6.Init.Period = 1999;

	__HAL_RCC_TIM6_CLK_ENABLE();
	// Enable NVIC interrupt for timer 6 TIM6_DAC
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	HAL_TIM_Base_Init(&htim6); //Configure the timer
	HAL_TIM_Base_Start_IT(&htim6);	// Start the timer
}


void TIM6_DAC_IRQHandler() {
// Pass the control to HAL, which processes the IRQ
   HAL_TIM_IRQHandler(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	// Interrupt triggers only with TIM6
	if(htim->Instance == TIM6){
		// Overflow counter increase
	    timeUpdated ++;
	    Time_ms = timeUpdated*100;	// Calculate time is ms
	    printf("\033[2;0H");
	    printf("overflow occured, time %dms\r\n", Time_ms);
   }
}
