/*
 * task4.c
 *
 *  Created on: Sep 23, 2021
 *      Author: wangy62
 */

#include "init.h"
#include "stdlib.h"

//void blinkScreen();
void Init_GPIO();
void Init_Timer();
void Screen_Init();
void blinkScreen();

GPIO_InitTypeDef GPIO_InitStruct;
// Interrupt flag
uint8_t react_flag = 0;
char c;
// Overflow counter, timer
uint32_t Global_Time = 0;
uint32_t reactTime = 0;
uint32_t player_score[5];
float averageScore = 0;
int temp = 0;
int random_time[5];
float totalTime = 0;

int main() {

	Sys_Init();
	HAL_Init();
	Init_Timer();
	Init_GPIO();
	printf("\033[2J\033[;H");
	printf("start\r\n");
	// Get random seed
	srand(time(NULL));
	// Generate the random value for setting the blinking start time
	for (int i=0;i<5;i++){
		random_time[i] = 50 + 10 * (rand()%40); // 0.5 sec to 4.5 sec to start blinking
		printf("Random number(50,450): %d\r\n", random_time[i]);
	}
	HAL_Delay(1000);
	// Push a key to start
	printf("push a key to start\r\n");
	c = getchar();
	// Game loop
	while(1){
		// Total 5 trails
		for (int k=0;k<5;k++){
			// Clear the screen
			Screen_Init();
			// Calculate delay time and delay
			temp = 10*random_time[k];
			HAL_Delay(temp);
			// Clear the timer
			Global_Time =0;
			// Blink the screen until interrupt
			// react_flag set to 1 only when there is an interrupt
            while(react_flag == 0){
            	// Blink the screen
            	printf("\033c\033[36m\033[2J");
            	printf("Blink!\r\n");
            	HAL_Delay(50);
            	blinkScreen();
            	HAL_Delay(50);
            }
            // record the react time in 10ms
            player_score[k] = reactTime;
		}
		printf("\033[2J\033[;H");
		// Print each score
        for (int j=0; j<5; j++){
            printf("the %d trail reatction time is %d0ms \r\n",j+1, player_score[j]);
            totalTime += player_score[j];
        }
        // Print total score
        averageScore = totalTime / 500;
        printf("Player average react time is %.2fs\r\n", averageScore);
        while(1);
	}
}

void Init_Timer() {
	// Enable the TIM6 interrupt.
	// Enable NVIC interrupt for TIM6_DAC position 54
	NVIC->ISER[1] = (uint32_t) 1 << (22);
	// Enable TIM6 clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	asm ( "nop" );
	asm ( "nop" );
	TIM6->PSC = 0x0437;	// PSC = 1079
	// Set the Auto-reload Value for 100Hz overflow
	TIM6->ARR = 0x03E9;	// ARR = 999
	// Generate update events to auto reload.
	TIM6->EGR |= 0x0001;
	// Enable Update Interrupts.
	TIM6->DIER |= 0x0001;
	// Start the timer.
	TIM6->CR1 |= 0x0001;
}

void Init_GPIO(){
	// Enable RCC clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	asm ("nop");
	asm ("nop");
	//Enable GPIO PA0 USER pushbutton for input
	GPIOA -> MODER &= 0xFFFFFFFCU;
	GPIOA -> PUPDR |= 0x00000002U;
	GPIOA -> PUPDR &= 0xFFFFFFFEU;

	// Enable GPIO A for interrupt
	SYSCFG->EXTICR[0] |= 0x00000000;
	SYSCFG->EXTICR[0] &= 0xFFFFFFFF;

	// Set interrupt enable for EXTI0.
	NVIC->ISER[0] = (uint32_t) 1 << (6);
	// Interrupt on triggers for the rising edge
	EXTI->IMR |= 0x00000001U;
	EXTI->RTSR |= 0x00000001U;

    __HAL_RCC_GPIOC_CLK_ENABLE();
    //GPIO C8 Pin D5 for input
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;	// Rising trigger
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	// Internal resistor pulldown
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	// Enable GPIO C8 Pin D5

	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

// User pushbutton interrupt to record the time
void EXTI0_IRQHandler() {
	// Clear Interrupt Bit by setting it to 1.
	EXTI->PR |= 0x00000001U;
	asm ("nop");
	asm ("nop");
	// Record the time when interrupt trigger
	reactTime = Global_Time;
	// set the flag to quit the loop for blinking
	react_flag = 1;
	for (int i = 0; i<10;i++){}
}

void TIM6_DAC_IRQHandler() {
	// Clear Interrupt Bit
	TIM6->SR &= 0xFFFE;
	// Overflow counter increases
	Global_Time++;
	asm ( "nop" );
	asm ( "nop" );
}

void EXTI9_5_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	// ISR code here.
	asm ("nop");
	asm ("nop");
		// Other code here:
	for (int i = 0; i<5;i++){}
	// Restart the program work same as the reset pushbutton
    NVIC_SystemReset();
}

void Screen_Init(){
	// Clear the screen and begin game
	printf("\033[2J\033[;H");
	printf("Test starts in 3 seconds\r\n");
	printf("3\r\n");
	HAL_Delay(1000);
	printf("2\r\n");
	HAL_Delay(1000);
	printf("1\r\n");
	HAL_Delay(1000);
	printf("0 Counting Begin!!\r\n");
	printf("0 Counting Begin!!\r\n");
	printf("0 Counting Begin!!\r\n");
	// Clear the timer and interrupt flag
	Global_Time = 0;
    react_flag = 0;
}

// blink function in the lab template
void blinkScreen(){
	printf("\033[30;47m");
	// Clear and redraw display (flash it & sound the bell).
	printf("\a\033[s\033[2J\033[u");
	fflush(stdout);
	HAL_Delay(100);
	printf("\033[37;40m");
	// Clear and redraw display (flash it).
	printf("\033[s\033[2J\033[u");
	fflush(stdout);
}
