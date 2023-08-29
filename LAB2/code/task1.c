/*
 * task1.c
 *
 *  Created on: Sep 20, 2021
 *      Author: wangy62
 */

#include "init.h"

void Init_GPIO();
void Init_Timer();

GPIO_InitTypeDef GPIO_InitStruct;

int main() {
	Sys_Init();
	HAL_Init();
	Init_GPIO();
	// Clear screan
	printf("\033[2J\033[;H");
	printf("start\r\n");
	// Infinite loop, wait for interrupt
	while(1);
}


void Init_GPIO(){
	//Enable RCC clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	asm ("nop");
	asm ("nop");
	//Enable GPIO J0 Pin D4 for input
	GPIOJ -> MODER &= 0xFFFFFFFCU;	//Mode input
	GPIOJ -> PUPDR |= 0x00000002U;	//Pull down
	GPIOJ -> PUPDR &= 0xFFFFFFFEU;	//Pull down

	// GPIO Interrupt
	// Enable GPIO J for interrupt
	SYSCFG->EXTICR[0] |= 0x00000009;
	SYSCFG->EXTICR[0] &= 0xFFFFFFF9;

	// Set interrupt enable for EXTI0.
	NVIC->ISER[0] = (uint32_t) 1 << (6);

	// Enable Rinsing and Falling edge for interrupt
	EXTI->IMR |= 0x00000001U;
	EXTI->RTSR |= 0x00000001U;
	EXTI->FTSR |= 0x00000001U;

	//GPIO C8 Pin D5 for input
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;	//Rising and falling edge
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	// Pulldown resister
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);	//

}

// IRQHandler for EXTI0 GPIO J0 Pin D4
void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI->PR |= 0x00000001U;
	asm ("nop");
	asm ("nop");
	// Other code here:
	printf("EXTI0 interrupt occured\r\n");
	for (int i = 0; i<15;i++){}
}

// IRQHandler for EXTI9_5 GPIO C8 Pin D5
void EXTI9_5_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	asm ("nop");
	asm ("nop");
	// Other code here:
	printf("EXTI9_5 interrupt occured\r\n");
	for (int i = 0; i<15;i++){}
}

