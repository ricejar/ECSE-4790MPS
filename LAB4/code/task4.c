//---------------------
// Lab 4 - Sample - Lab04_sample.c
//--------------------------------
//
//

#include "init.h"

void configureADC();
void InitGPIO();
void configureDAC();

ADC_HandleTypeDef hadc1;
HAL_StatusTypeDef adcSta;
GPIO_InitTypeDef GPIO_InitStruct;
DAC_HandleTypeDef DACHandler;
float result[16];
float aveV;
float maxV;
float minV;
unsigned char model = 1;


// Main Execution Loop
int main(void)
{

	//Initialize the system
	//int i = 0;
	//float tmp = 0;
	int convResult = 0;
	//char user_in;
	int start = 0;
	//int output = 0;
	int Xpre2 = 0;
	int Xpre1 = 0;
	int Ypre1 = 0;
	float temp = 0;
	float read = 0;
	double Y = 0;
	float Y1 = 0;
	char user_in;
	Sys_Init();
	InitGPIO();
	configureADC();
	configureDAC();
	HAL_DAC_Start(&DACHandler, DAC_CHANNEL_1);

	// Code goes here
	printf("\033[2J\033[;H");
	printf("start\r\n");
	fflush(stdout);
	while(1){
		if (model == 1){
					printf("\033[1;1H");
					fflush(stdout);
					printf("choose model: 1 for c equation, 2 for assembly \r\n");
					user_in = getchar();
					putchar(user_in);
					start = 0;
					model = 0;
					if(user_in == 0x31){
						printf("\033[4;1H\033[k");
						printf("C equation mode\r\n");
					}else if(user_in == 0x32){
						printf("\033[4;1H\033[K");
						printf("MAC assembly mode");
					}
				}

		adcSta = HAL_ERROR;
		HAL_ADC_Start(&hadc1);
		while(adcSta != HAL_OK){
			adcSta = HAL_ADC_PollForConversion(&hadc1, 10);
		}
		convResult = HAL_ADC_GetValue(&hadc1);
		if(user_in == 0x31){
			if (start == 0){
				Xpre2 = convResult;
				start +=1;
				continue;
			}else if (start == 1){
				Xpre1 = convResult;
				start +=2;
				continue;
			}
			Y = 0.3125*convResult + 0.240385*Xpre1 + 0.3125*Xpre2+0.296875*Ypre1;
			Ypre1 = (int)Y;
			Xpre2 = Xpre1;
			Xpre1 = convResult;
		}
		if(user_in == 0x32){
			if (start == 0){
				Xpre2 = convResult;
				start +=1;
				continue;
			}else if (start == 1){
				Xpre1 = convResult;
				start +=2;
				continue;
			}
			Y1 = 0;
			temp = 0.0003125;
			read = convResult/1.0;
//			printf("\033[7;1H");
//			printf("temp: %f, read: %f\r\n",temp,read);
			asm volatile("VMLA.F32 %[dest], %[tmp1], %[tmp2]" : [dest] "+w" (Y1) : [tmp1] "w" (temp), [tmp2] "w" (read));
			temp = 0.000240385;
			read = Xpre1/1.0;
			asm volatile("VMLA.F32 %[dest], %[tmp1], %[tmp2]" : [dest] "+w" (Y1) : [tmp1] "w" (temp), [tmp2] "w" (read));
			temp = 0.0003125;
			read = Xpre2/1.0;
			asm volatile("VMLA.F32 %[dest], %[tmp1], %[tmp2]" : [dest] "+w" (Y1) : [tmp1] "w" (temp), [tmp2] "w" (read));
			temp = 0.000296875;
			read = Ypre1/1.0;
			asm volatile("VMLA.F32 %[dest], %[tmp1], %[tmp2]" : [dest] "+w" (Y1) : [tmp1] "w" (temp), [tmp2] "w" (read));
//			printf("\033[8;1H");
//			printf("x(k) value: %f\r\n",Y1);
			Y1 = Y1*1000;
			Ypre1 = (int)Y1;
			Xpre2 = Xpre1;
			Xpre1 = convResult;
//			printf("\033[9;1H");
//			printf("output value: %d\r\n",Ypre1);
		}

		HAL_DAC_SetValue(&DACHandler, DAC_CHANNEL_1, DAC_ALIGN_12B_R, Ypre1);


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

	GPIO_InitTypeDef GPIOinit;
	GPIOinit.Pin = GPIO_PIN_4;			//Pin4 of GPIOA
	GPIOinit.Mode = GPIO_MODE_ANALOG;	//Set Analog mode
	GPIOinit.Pull = GPIO_NOPULL;		//No pull up

	HAL_GPIO_Init(GPIOA,&GPIOinit);//Initialize GPIOA Pin4 as configured above

}

void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI->PR |= 0x00000001U;
	asm ("nop");
	asm ("nop");
	model = 1;
//	printf("Start a new sampling\r\n");
//	printf("The average voltage is %2.6f\r\n",aveV);
//	printf("The maximum voltage is %2.6f\r\n",maxV);
//	printf("The minimum voltage is %2.6f\r\n",minV);
	for (int i = 0; i<10;i++){}
}

void configureDAC()
{

	// Enable the DAC Clock.
		__HAL_RCC_DAC_CLK_ENABLE();

	// Declare DAC_CahnnelConfTypeDef
	DAC_ChannelConfTypeDef ChannelConfig;

	//DAC channel OUT1 config
	ChannelConfig.DAC_Trigger = DAC_TRIGGER_NONE;
	ChannelConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

	//Set up DACHandler
	DACHandler.Instance = DAC;
	HAL_DAC_Init(&DACHandler); // Initialize the DAC

	// Configure the ADC channel
	HAL_DAC_ConfigChannel(&DACHandler, &ChannelConfig, DAC_CHANNEL_1);
}

//void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
//{
//
//// GPIO init
//	GPIO_InitTypeDef GPIOinit;
//	GPIOinit.Pin = GPIO_PIN_4;			//Pin4 of GPIOA
//	GPIOinit.Mode = GPIO_MODE_ANALOG;	//Set Analog mode
//	GPIOinit.Pull = GPIO_NOPULL;		//No pull up
//
//	HAL_GPIO_Init(GPIOA,&GPIOinit);//Initialize GPIOA Pin4 as configured above
//}
