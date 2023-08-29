/*
 * task3.c
 *
 */

#include "init.h"

UART_HandleTypeDef WIRE_UART;
char input[1];
int Rev_Flag = 0;
int Rev_Flag_2 = 0;
void interrupt_Init();
void USART1_IRQHandler();
void USART6_IRQHandler();
// Main Execution Loop
int main(void) {
	//Initialize the system

	Sys_Init();
	printf("\033[2J\033[;H");
	printf("Your code here!\r\n");
	initUart(&WIRE_UART,38400,USART6);
	interrupt_Init();

    // Enable the UART1, 6 interrupt
	HAL_UART_Receive_IT(&USB_UART,(uint8_t *)input, 1);
	HAL_UART_Receive_IT(&WIRE_UART,(uint8_t *)input, 1);
//	char USB;
//	char WIRE;
	while(1){
		if(Rev_Flag == 0){  // UART1
            // ESCAPE stop program
			if(input[0] == 0x1b){
				uart_print(&USB_UART," ");
				uart_print(&USB_UART,"\r\n  stop the program");
				break;
			}
			Rev_Flag = 1;
		}
		if(Rev_Flag_2 == 0){    // UART6
            // ESCAPE stop program
			Rev_Flag_2 = 1;
			if(input[0] == 27){
				uart_print(&USB_UART," ");
				uart_print(&USB_UART,"\r\n  stop the program");
				break;
			}
		}
	}
}

// Enable NVIC interrupt
void interrupt_Init(){
	NVIC ->ISER [37/32] = (uint32_t) 1 << (37 % 32);//enable usart1 interrupt
	NVIC ->ISER [71/32] = (uint32_t) 1 << (71 % 32);//enable usart6 interrupt
}

void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(&USB_UART);
}
void USART6_IRQHandler(void) {
	HAL_UART_IRQHandler(&WIRE_UART);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* uart) {
    // remove IRQ handler
	if(uart -> Instance == USART1)  // UART1
	{
		uart_putchar(&USB_UART,input);
		uart_putchar(&WIRE_UART,input);
		Rev_Flag=0;
		HAL_UART_Receive_IT(&USB_UART,(uint8_t *)input, 1);
	}
	if(uart ->Instance == USART6){  // UART6
		uart_putchar(&USB_UART,input);
		Rev_Flag_2=0;
		HAL_UART_Receive_IT(&WIRE_UART,(uint8_t *)input, 1);
	}
	for (int i = 0; i<10;i++){}
}



