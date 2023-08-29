//--------------------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"

void configureADC();
void InitGPIO();

ADC_HandleTypeDef hadc1;
HAL_StatusTypeDef adcSta;
GPIO_InitTypeDef GPIO_InitStruct;
float result[16];
float aveV;
float maxV;
float minV;


// Main Execution Loop
int main(void)
{

	//Initialize the system
	int i = 0;
	float tmp = 0;
	int convResult = 0;
	Sys_Init();
	InitGPIO();
	configureADC();

	// Code goes here
	printf("\033[2J\033[;H");
	printf("start\r\n");
	fflush(stdout);
	while(1){
		if (i < 17){i = i + 1;}
		adcSta = HAL_ERROR;
		HAL_ADC_Start(&hadc1);
		while(adcSta != HAL_OK){
			adcSta = HAL_ADC_PollForConversion(&hadc1, 10);
		}
		convResult = HAL_ADC_GetValue(&hadc1);
		//printf("conversion: %d\r\n",convResult);
		if (convResult >= 4094){
			tmp = 3.3;
		}else{
			tmp = 3.3*convResult/4095;
		}
		//printf("volatge: %f\r\n",tmp);
		if (i<16){
			result[i] = tmp;
		}else{
			for (int j = 0; j < 15 ;j++){
				result[j] = result[j+1];
			}
			result[15] = tmp;
		}

	}
}

void configureADC()
{
	ADC_ChannelConfTypeDef sConfig;
	// Enable the ADC Clock.
	__HAL_RCC_ADC1_CLK_ENABLE();

	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;

	HAL_ADC_Init(&hadc1); // Initialize the ADC

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

	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;

	// Configure the ADC channel

	HAL_ADC_ConfigChannel(&hadc1,&sConfig);
}

void InitGPIO(){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    //GPIO C8 Pin D5 for input
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;	// Rising trigger
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	// Internal resistor pulldown
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	// Enable GPIO A0 User pushbutton

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;	// Rising trigger
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	// Internal resistor pulldown
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI->PR |= 0x00000001U;
	asm ("nop");
	asm ("nop");
	maxV = result[0];
	minV = result[0];
	aveV = 0;

	for (int k = 0; k<16;k++){
		aveV += result[k];
		if (result[k] > maxV){
			maxV = result[k];
		}
		if(result[k] < minV){
			minV = result[k];
		}
	}
	aveV = aveV/16;
	printf("Start a new sampling\r\n");
	printf("The average voltage is %2.6f\r\n",aveV);
	printf("The maximum voltage is %2.6f\r\n",maxV);
	printf("The minimum voltage is %2.6f\r\n",minV);
	for (int i = 0; i<10;i++){}
}
