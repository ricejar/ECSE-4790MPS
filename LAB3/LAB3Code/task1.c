/*
 * task1.c
 *
 *  Created on: Sep 30, 2021
 *      Author: wangy62
 */


#include "init.h"

void Uart_Init();
char task1_getchar(UART_HandleTypeDef *huart, uint8_t echo);


UART_HandleTypeDef huart6;
HAL_StatusTypeDef usb;  // UART1
HAL_StatusTypeDef uart6;
uint16_t Size;
uint8_t * pData;
uint32_t outTime = 50;

// Main Execution Loop
int main(void) {
	//Initialize the system
	Sys_Init();
	Uart_Init();
	printf("\033[2J\033[;H");
	printf("Your code here!\r\n");
	char input[1];
	char output[1];


	// Lost? Check out uart.c!
	while(1){
		usb = HAL_UART_Receive(&USB_UART, (uint8_t*) input, 1, 10);
		uart6 = HAL_UART_Receive(&huart6, (uint8_t*) output, 1, 20);
		// UART1 receive
		if(usb == HAL_OK){
			HAL_UART_Transmit(&huart6, (uint8_t*) input, 1, 10);
            // escape stop program
			if (input[0] == 0x1b){
				HAL_UART_Transmit(&USB_UART, (uint8_t*) " PROGRAM EXTI!!!", 16, 10);
				return 1;
			}
			HAL_UART_Transmit(&USB_UART, (uint8_t*) input, 1, 10);
		}
        // UART6 receive
		if (uart6 == HAL_OK){
            // Escape stop program
			if (output[0] == 0x1b){
				HAL_UART_Transmit(&USB_UART, (uint8_t*) " PROGRAM EXTI!!!", 16, 10);
				return 1;
			}
			HAL_UART_Transmit(&USB_UART, (uint8_t*) output, 1, 10);
		}
	}
}
// UART6 initialization
void Uart_Init(){
	GPIO_InitTypeDef  GPIO_InitStruct;

	__GPIOC_CLK_ENABLE();

	// Initialize TX Pin
	GPIO_InitStruct.Pin       = GPIO_PIN_6;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //TX Config

	// Initialize RX Pin
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); //RX Config

	// Enable UART Clocking
	__USART6_CLK_ENABLE();
	initUart(&huart6, 38400, USART6);
}




