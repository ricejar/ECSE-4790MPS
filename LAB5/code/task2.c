/*
 * task2.c
 *
 */
#include "init.h"
#include <stdio.h>
#include <stdlib.h>

//Declase Variables&Structs
SPI_HandleTypeDef SPIHandle;//Declare SPI handler struct
DMA_HandleTypeDef SPI_DMA_rxHandle;
DMA_HandleTypeDef SPI_DMA_txHandle;
unsigned char input;
unsigned char Data;
char inputs[20];
char outputs[20];
unsigned char c;
unsigned char r;
int i;
unsigned char flag;
void configureDMA()
{
	/*
	 * The channels for rx and tx in SPI2 is DMA1 channel 0, stream 3 and 4 
	 */
	SPI_DMA_rxHandle.Instance = DMA1_Stream3;
	SPI_DMA_rxHandle.Init.Channel = DMA_CHANNEL_0;
    // Receive memory so peripheral to memory
	SPI_DMA_rxHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    // Receive memory, only peripheral needs increment
	SPI_DMA_rxHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	SPI_DMA_rxHandle.Init.MemInc = DMA_MINC_ENABLE;
	SPI_DMA_rxHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	SPI_DMA_rxHandle.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    // Circular
	SPI_DMA_rxHandle.Init.Mode = DMA_CIRCULAR;
	SPI_DMA_rxHandle.Init.Priority = DMA_PRIORITY_LOW;
    // FIFO disable
	SPI_DMA_rxHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	__HAL_RCC_DMA1_CLK_ENABLE();

	SPI_DMA_txHandle.Instance = DMA1_Stream4;
	SPI_DMA_txHandle.Init.Channel = DMA_CHANNEL_0;
    // Sending data, memory to peripheral
	SPI_DMA_txHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    // Send data, only memory needs increment
	SPI_DMA_txHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	SPI_DMA_txHandle.Init.MemInc = DMA_MINC_ENABLE;
	SPI_DMA_txHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	SPI_DMA_txHandle.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
    // Circular
	SPI_DMA_txHandle.Init.Mode = DMA_CIRCULAR;
	SPI_DMA_txHandle.Init.Priority = DMA_PRIORITY_LOW;
    // FIFO disable
	SPI_DMA_txHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    // Initialization
	HAL_DMA_Init(&SPI_DMA_txHandle);
	HAL_DMA_Init(&SPI_DMA_rxHandle);

    // Enable interrupt
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

	// Link DMA
	__HAL_LINKDMA(&SPIHandle, hdmarx, SPI_DMA_rxHandle);
	__HAL_LINKDMA(&SPIHandle, hdmatx, SPI_DMA_txHandle);



}

void configureSPI()
{
	/*In this function, only construct a SPI_HandleTypeDef Struct and pass it
	 * in to HAL_SPI_Init function, GPIO Setups should be in HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
	 * function
	 */
	__SPI2_CLK_ENABLE();
	SPIHandle.Instance = SPI2;
	SPIHandle.Init.Mode = SPI_MODE_MASTER;
	SPIHandle.Init.NSS = SPI_NSS_SOFT;
	SPIHandle.Init.TIMode = SPI_TIMODE_DISABLE;
	SPIHandle.Init.DataSize = SPI_DATASIZE_8BIT;
	SPIHandle.Init.Direction = SPI_DIRECTION_2LINES;
	SPIHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	SPIHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPIHandle.Init.CLKPhase = SPI_PHASE_2EDGE;

	HAL_SPI_Init(&SPIHandle);

	//Enable Interrupt

	HAL_NVIC_EnableIRQ(SPI2_IRQn);
}


// SPI GPIO initialization
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	// Clock enable
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIOInit;
	GPIOInit.Mode = GPIO_MODE_AF_PP;    // Mode for SPI
	GPIOInit.Pin = (GPIO_PIN_15 | GPIO_PIN_14);// Pin 14 and 15
	GPIOInit.Pull = GPIO_PULLUP;
	GPIOInit.Alternate =  GPIO_AF5_SPI2;    // SPI2 enable

	HAL_GPIO_Init(GPIOB,&GPIOInit);//Initialize GPIO
	GPIOInit.Pin = (GPIO_PIN_11 | GPIO_PIN_12);//Pin 11 and 12 Enable SCLK
	HAL_GPIO_Init(GPIOA,&GPIOInit);//Initialize GPIO


	if (hspi->Instance == SPI2)
	{
		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
		// No GPIO is needed for Task 3 therefore no GPIO setup here
	}
}

void DMA1_Stream4_IRQHandler(){
	HAL_DMA_IRQHandler(&SPI_DMA_txHandle);
}

void DMA1_Stream3_IRQHandler(){
	HAL_DMA_IRQHandler(&SPI_DMA_rxHandle);
}

void SPI2_IRQHandler(){
	HAL_SPI_IRQHandler(&SPIHandle);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi){
	// Set the flag once the interrupt triggered
	if(hspi -> Instance == SPI2){
		flag = 1;
	}
	for( i = 0; i < 10; i++){asm("nop");}
}

int main(void)
{
	Sys_Init();
	configureSPI();
	configureDMA();
	printf("\033[2J\033[;H");
	printf("start\r\n");
	fflush(stdout);

	int length = 0;
	length = uart_getline(&USB_UART,inputs,19);
	flag = 0;
    // Start the DMA
	HAL_SPI_TransmitReceive_DMA(&SPIHandle,&inputs,outputs,sizeof(inputs));
	// Wait untill the transmit and receive is complete
	while(flag == 0){}
    // reset the flag
	flag = 0;
    // print the received data
	printf("\033[12;1H\033[K");
	fflush(stdout);
	for (int j = 0; j< length; j++){
		printf("%c",outputs[j]);
	}
	printf("\r\n");
    // clear the input data buffer
	memset(inputs, 0, sizeof(inputs));

	while(1){
		flag = 0;
		printf("\033[2;1H\033[K");
		fflush(stdout);
        // get input
		length = uart_getline(&USB_UART,inputs,19);
		// wait for interrupt
		while(flag == 0){}
		printf("\033[12;1H\033[K");
		fflush(stdout);
        // print output
		for (int j = 0; j< length; j++){
			printf("%c",outputs[j]);
		}
		printf("\r\n");
        // clear data buffer
		memset(inputs, 0, sizeof(inputs));
	}
}


