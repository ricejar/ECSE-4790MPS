/*
 * task3.c
 *
 *  Created on: Oct 7, 2021
 *      Author: wangy62
 */

#include "init.h"

// If needed:
//#include <stdio.h>
//#include <stdlib.h>

SPI_HandleTypeDef SPI_handle_type;
char input[1];
char output[1];
/*
 * For convenience, configure the SPI handler here
 */
// See 769 Description of HAL drivers.pdf, Ch. 58.1 or stm32f7xx_hal_spi.c
void configureSPI()
{
	__SPI2_CLK_ENABLE();
	SPI_handle_type.Instance = SPI2; // Please use SPI2!
	SPI_handle_type.Init.Mode = SPI_MODE_MASTER; // Set master mode
	SPI_handle_type.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
	SPI_handle_type.Init.Direction =  SPI_DIRECTION_2LINES;
	SPI_handle_type.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_handle_type.Init.CLKPolarity = SPI_POLARITY_HIGH;
	SPI_handle_type.Init.CLKPhase = SPI_POLARITY_HIGH;
	SPI_handle_type.Init.NSS = SPI_NSS_HARD_OUTPUT;
	SPI_handle_type.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SPI_handle_type.Init.FirstBit = SPI_FIRSTBIT_LSB;
	SPI_handle_type.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI_handle_type.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;


	HAL_SPI_Init(&SPI_handle_type);
	//
	// Note: HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
	//
	// HAL_SPI_Init() will call HAL_SPI_MspInit() after reading the properties of
	// the passed SPI_HandleTypeDef. After finishing the MspInit call, it will set
	// the SPI property bits. This is how all HAL_[peripheral]_Init() functions work.
}

/*
 * This is called upon SPI initialization. It handles the configuration
 * of the GPIO pins for SPI.
 */
 // Do NOT change the name of an MspInit function; it needs to override a
 // __weak function of the same name. It does not need a prototype in the header.
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	// SPI GPIO initialization structure here
	GPIO_InitTypeDef  GPIO_InitStruct;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin       = GPIO_PIN_14;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin       = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin       = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	if (hspi->Instance == SPI2)
	{
		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
	}
}

int main(void)
{
	Sys_Init();
	// For convenience
	configureSPI();
	printf("\033[2J\033[;H");

	while(1){
		input[0] = getchar();
		// print at top
		printf("\033[H");
		printf("\033[2k");
		putchar(input[0]);
		fflush(stdout);
        // Send data and receive data by SPI
		HAL_SPI_TransmitReceive(&SPI_handle_type, (uint8_t*) input, (uint8_t*) output,1,10);
        // print at bottom
		printf("\033[12H");
		printf("\033[2k");
		putchar(output[0]);
		fflush(stdout);
		printf("\033[13H");
		fflush(stdout);
	}


// See 769 Description of HAL drivers.pdf, Ch. 58.2.3 or stm32f7xx_hal_spi.c
//
//	HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
//
}
