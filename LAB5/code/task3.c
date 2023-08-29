/*
 * task3.c
 *
 */
#include "init.h"

ADC_HandleTypeDef hadc1;
GPIO_InitTypeDef GPIO_InitStruct;
DAC_HandleTypeDef DACHandler;
DMA_HandleTypeDef ADC_DMA_Handle;//Declare DMA handler struct for ADC module

unsigned int ADCresult1;
int X;
int X1 = 0;
int X2 = 0;
double Y;
int Y1 = 0;
char convComplete=0;//adc conversion complete flag


//Function Prototypes
void configureDAC();
void configureADC();
void configureDMA();
void filter();

int main(void)
{
	// Initialize the system
	Sys_Init();
	configureDAC();
	configureADC();
	configureDMA();
	// Turn on DAC at Channel 1
	HAL_DAC_Start(&DACHandler, DAC_CHANNEL_1);
	// start ADC conversion
	HAL_ADC_Start_DMA(&hadc1, &ADCresult1, 1);

	//infinite loop
	while(1){
		filter();
	}
}

void filter(){
	int DACValue;
	while(!convComplete){} // hold until the ADC reading is complete
	convComplete = 0;//set flag back to 0

	ADCresult1 = HAL_ADC_GetValue(&hadc1);//Get new ADC reading
	X = ADCresult1;
	// Filter
	Y = 0.312500*X + 0.240385*X1+ 0.312500*X2+ 0.296875*Y1;

	DACValue = (int)(Y);
	HAL_DAC_SetValue(&DACHandler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, DACValue);//Set DC valueA

    // Update value
	X2 = X1;
	X1 = X;
	Y1 = DACValue;

}

void configureDAC()
{
    // DAC part are the same as LAB 4
	__HAL_RCC_DAC_CLK_ENABLE();

	DAC_ChannelConfTypeDef ChannelConfig;
	ChannelConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	ChannelConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	DACHandler.Instance = DAC;

	HAL_DAC_Init(&DACHandler);

	// Configure the ADC channel
	HAL_DAC_ConfigChannel(&DACHandler, &ChannelConfig, DAC_CHANNEL_1);
}



void configureADC()
{

	ADC_ChannelConfTypeDef ChannelConfig;

	__HAL_RCC_ADC1_CLK_ENABLE();
	// USE ADC1
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
    // Enable DMA
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	HAL_ADC_Init(&hadc1);

	ChannelConfig.Channel = ADC_CHANNEL_6;
	ChannelConfig.Rank = 1;
    // Choose sample time not too long
	ChannelConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &ChannelConfig);

	/* Available sampling times:
		ADC_SAMPLETIME_3CYCLES
	  	ADC_SAMPLETIME_15CYCLES
		ADC_SAMPLETIME_28CYCLES
		ADC_SAMPLETIME_56CYCLES
		ADC_SAMPLETIME_84CYCLES
		ADC_SAMPLETIME_112CYCLES
		ADC_SAMPLETIME_144CYCLES
		ADC_SAMPLETIME_480CYCLES
	*/

}



void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
    fflush(stdout); 

    // ADC GPIO Init Pin A0
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// DAC GPIO init Pin A1
	GPIO_InitTypeDef GPIOinit;
	GPIOinit.Pin = GPIO_PIN_4;			//Pin4 of GPIOA
	GPIOinit.Mode = GPIO_MODE_ANALOG;	//Set Analog mode
	GPIOinit.Pull = GPIO_NOPULL;		//No pull up

	HAL_GPIO_Init(GPIOA,&GPIOinit);//Initialize GPIOA Pin4 as configured above
}

void configureDMA()
{
	// Enable DMA2 for ADC
	__HAL_RCC_DMA2_CLK_ENABLE();//Enable DMA2 clock

    //ADC uses DMA2 Stream0 channel 0
	ADC_DMA_Handle.Instance = DMA2_Stream0;
	ADC_DMA_Handle.Init.Channel = DMA_CHANNEL_0;
    //ADC peripheral to memory
	ADC_DMA_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    // No need for increement
	ADC_DMA_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
	ADC_DMA_Handle.Init.MemInc = DMA_MINC_DISABLE;
    // Data alignment is 1 byte
	ADC_DMA_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	ADC_DMA_Handle.Init.PeriphDataAlignment = DMA_MDATAALIGN_BYTE;
	ADC_DMA_Handle.Init.Mode = DMA_CIRCULAR;//Circular Mode
	ADC_DMA_Handle.Init.Priority = DMA_PRIORITY_LOW;
    // No FIFO in this lab
	ADC_DMA_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	// Init DMA
	HAL_DMA_Init(&ADC_DMA_Handle);

	// Enable Interrupt
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    // Enable link
	__HAL_LINKDMA(&hadc1, DMA_Handle, ADC_DMA_Handle);
}

void DMA2_Stream0_IRQHandler(){
	HAL_DMA_IRQHandler(&ADC_DMA_Handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	convComplete = 1;   // Set the flag once the ADC reading complete
}
