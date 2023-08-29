/*
 * Task3.c
 *
 *  Created on: Sep 13, 2021
 *      Author: 18238
 */
#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>
int main(void)
{
	Sys_Init();
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;// Set clock for GPIOD
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;// Set clock for GPIOC
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN;// Set clock for GPIOJ
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;// Set clock for GPIOA
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;// Set clock for GPIOF
	printf("\033[0;33;44m");
	printf("\033[2J\033[;H"); // Erase screen & move cursor to home position

	fflush(stdout);

	GPIOD -> MODER |= 0x00000100U;// Set to output mode
	GPIOJ->MODER |= 1024U;
	GPIOJ->MODER |= 67108864U;
	GPIOA->MODER |= 16777216U;
	//GPIOC 6 and 7
    GPIOC -> MODER &= 0xFFFF0FFFU;// Set to input mode
	GPIOJ->MODER &= 0xfffffffc;
	GPIOF->MODER &= 0xffffcfff;
	GPIOC -> PUPDR |= 0X00004000U;// Set to pull up
	GPIOC -> PUPDR |= 0X00005000U;
	GPIOJ -> PUPDR |= 0x00000004U;
	GPIOF -> PUPDR |= 0x00001000U;
	while(1)
	{
		//printf("Pin state %d\r\n",GPIOC -> IDR &= 0x00000080);
		if(GPIOC -> IDR &= 0x00000080)//led1 control
			GPIOJ -> BSRR = 0x00002000U;//led1 on
		else
			GPIOJ -> BSRR = 0x20000000U;//led1 off
		if(GPIOC -> IDR &= 0x00000040)//led2 control
			GPIOJ -> BSRR = 0x00000020U;//led2 on
		else
			GPIOJ -> BSRR = 0x00200000U;//led2 off
		if(GPIOJ -> IDR &= 0x00000020)//led3 control
			GPIOA -> BSRR = 0x00001000U;//led3 on
		else
			GPIOA -> BSRR = 0x10000000U;//led3 off
		if(GPIOF -> IDR &= 0x00000040)//led4 control
			GPIOD -> BSRR = 0x00100000;//led4 on
		else
			GPIOD -> BSRR = 0x00000010;//led4 off




	}

}

