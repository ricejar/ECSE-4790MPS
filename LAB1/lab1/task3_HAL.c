/*
 * task.c
 *
 * Task 3 HAL function part
 *
 *  Created on: Sep 9, 2021
 *      Author: wangy62
 */

#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>

GPIO_InitTypeDef GPIO_InitStruct;

void LED_GPIO_Init(void);
void INPUT_GPIO_Init(void);



int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)
    LED_GPIO_Init();
    INPUT_GPIO_Init();
    //char choice;

    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n
    printf("press ESC to quit.\r\n\n");

    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);// LED4 logical inverted reset means set
    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);



    //HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET);         // Turn green LED on (GPIO_PIN_SET == 1)
    //HAL_Delay(2000);

    while(1){


    	printf("Pin state %d\r\n",HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7));
    	//HAL_Delay(1000);
    	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7))	//D0 and LED1
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
    	else
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);

    	//HAL_Delay(1000);
    	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6))	//D1 and LED2
    	   	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET);
    	else
    	    HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET);

    	if (HAL_GPIO_ReadPin(GPIOJ, GPIO_PIN_1))	//D2 and LED3
    	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    	else
    	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

    	if (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_6))	//D3 and LED4
    	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
    	else
    	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

    	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);// LED4 logical inverted reset means set
    	//HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
    	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);

    	//HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_RESET);
    	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
    	//HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);
    	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    	//HAL_Delay(1000);



    }


}


void LED_GPIO_Init(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOJ_CLK_ENABLE();

	// --------------LED1 initialization----------------
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	// --------------LED2 initialization----------------
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
	// --------------LED2 initialization----------------
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// ------LED4 initialization------LED4 logical inverted reset means set
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void INPUT_GPIO_Init(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	//__HAL_RCC_GPIOJ_CLK_ENABLE();
	// --------------D0 initialization----------------
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// --------------D1 initialization----------------
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// --------------D0 initialization----------------
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

	// --------------D0 initialization----------------
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);



}

