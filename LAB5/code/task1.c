#include "init.h" 
#include<stdint.h>
#include<stdlib.h>


void configDMA();

uint8_t flashData[1000];
uint8_t sramData[1000];
uint16_t flashData2[1000];
uint16_t sramData2[1000];
uint32_t flashData3[1000];
uint32_t sramData3[1000];
DMA_HandleTypeDef hdma_memtomem_dma2_stream0;
HAL_StatusTypeDef statusCheck;


int main(void)
{
	Sys_Init();
	HAL_Init();
	configDMA();
	printf("\033[2J\033[;H");
	printf("start\r\n");
	fflush(stdout);

	// Enable the DWT_CYCCNT register
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->LAR = 0xC5ACCE55;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	for (int i = 0; i<1000;i++){
		flashData[i] = i%10;
		flashData2[i] = i;
		flashData3[i] = i;
	}

    // 1000 data
	DWT->CYCCNT = 0; // Clear the cycle counter
	statusCheck = HAL_DMA_Start(&hdma_memtomem_dma2_stream0, (uint32_t)&flashData, (uint32_t)sramData, 1000);
	//printf("DMA status,%x \r\n",statusCheck);
	statusCheck = HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream0, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	//printf("DMA poll status,%x \r\n",statusCheck);
	uint32_t cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for DMA mode, 1000: %d\r\n", cycles);
	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int j = 0; j<1000; j++){
		sramData[j] = flashData[j];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for c implementation, 1000: %d\r\n", cycles);

	printf("\r\n");

    // 100 data
	DWT->CYCCNT = 0; // Clear the cycle counter
	statusCheck = HAL_DMA_Start(&hdma_memtomem_dma2_stream0, (uint32_t)&flashData, (uint32_t)sramData, 100);
	//printf("DMA status,%x \r\n",statusCheck);
	statusCheck = HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream0, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
		//printf("DMA poll status,%x \r\n",statusCheck);
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for DMA mode, 100: %d\r\n", cycles);
	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int j = 0; j<100; j++){
		sramData[j] = flashData[j];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for c implementation, 100: %d\r\n", cycles);

	printf("\r\n");

    // 10 data
	DWT->CYCCNT = 0; // Clear the cycle counter
	statusCheck = HAL_DMA_Start(&hdma_memtomem_dma2_stream0, (uint32_t)&flashData, (uint32_t)sramData, 10);
	statusCheck = HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream0, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for DMA mode, 10: %d\r\n", cycles);
	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int j = 0; j<10; j++){
		sramData[j] = flashData[j];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for c implementation, 10: %d\r\n", cycles);

	printf("\r\n");

    // 100 data for uint16_t
	DWT->CYCCNT = 0; // Clear the cycle counter
	statusCheck = HAL_DMA_Start(&hdma_memtomem_dma2_stream0, (uint32_t)&flashData2, (uint32_t)sramData2, 100*sizeof(uint16_t));
	statusCheck = HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream0, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for DMA mode, 100, uint16: %d\r\n", cycles);
	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int j = 0; j<100; j++){
		sramData2[j] = flashData2[j];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for c implementation, 100, uint16: %d\r\n", cycles);

	printf("\r\n");

    // 100 data for uint32_t
	DWT->CYCCNT = 0; // Clear the cycle counter
	statusCheck = HAL_DMA_Start(&hdma_memtomem_dma2_stream0, &flashData3, sramData3, 100*sizeof(uint32_t));
	statusCheck = HAL_DMA_PollForTransfer(&hdma_memtomem_dma2_stream0, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for DMA mode, 100, uint32: %d\r\n", cycles);
	DWT->CYCCNT = 0; // Clear the cycle counter
	for (int j = 0; j<100; j++){
		sramData3[j] = flashData3[j];
	}
	cycles = DWT->CYCCNT; // Store the cycle counter
	printf("cycles takes for c implementation, 100, uint32: %d\r\n", cycles);

	printf("\r\n");

	while(1);
}

// DMA setting
void configDMA(){
    // DMA2 channel 0 stream 1
	hdma_memtomem_dma2_stream0.Instance = DMA2_Stream1;
	hdma_memtomem_dma2_stream0.Init.Channel = DMA_CHANNEL_0;
    // No peripheral, memory to memory
	hdma_memtomem_dma2_stream0.Init.Direction = DMA_MEMORY_TO_MEMORY;
    // both increment need to be enable
	hdma_memtomem_dma2_stream0.Init.PeriphInc = DMA_PINC_ENABLE;
	hdma_memtomem_dma2_stream0.Init.MemInc = DMA_MINC_ENABLE;
    // Data alignment is byte
	hdma_memtomem_dma2_stream0.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_memtomem_dma2_stream0.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_memtomem_dma2_stream0.Init.Mode = DMA_NORMAL;
	hdma_memtomem_dma2_stream0.Init.Priority = DMA_PRIORITY_LOW;
    // FIFO need to be disable
	hdma_memtomem_dma2_stream0.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	hdma_memtomem_dma2_stream0.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;

	__HAL_RCC_DMA2_CLK_ENABLE();
	HAL_DMA_Init(&hdma_memtomem_dma2_stream0);
}
