/*
 * taks4.c
 */

#include "init.h"
#include <stdio.h>
#include <stdlib.h>

//Declase Variables&Structs
SPI_HandleTypeDef SPI_handle_type; //Declare struct
unsigned char input;
unsigned char Data;
unsigned char V_MAJ; // Major version number
unsigned char V_MIN; // Minor version number
unsigned char TMP_LO; // LOW byte of temp read
unsigned char TMP_HI; // HIGH byte of temp read
int temp1;

void configureSPI()
{
	__SPI2_CLK_ENABLE();
	SPI_handle_type.Instance = SPI2; // Please use SPI2!
	SPI_handle_type.Init.Mode = SPI_MODE_MASTER; // Set master mode
	SPI_handle_type.Init.TIMode = SPI_TIMODE_DISABLE; // Use Motorola mode, not TI mode
	SPI_handle_type.Init.Direction =  SPI_DIRECTION_2LINES;
	SPI_handle_type.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_handle_type.Init.CLKPolarity = SPI_POLARITY_LOW;
	SPI_handle_type.Init.CLKPhase = SPI_PHASE_2EDGE;
	SPI_handle_type.Init.NSS = SPI_NSS_SOFT;
	SPI_handle_type.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
	SPI_handle_type.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_handle_type.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI_handle_type.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_handle_type.Init.CRCPolynomial = 7;
	SPI_handle_type.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	SPI_handle_type.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

	HAL_SPI_Init(&SPI_handle_type);//Invoke HAL SPI Init function

	// HAL_SPI_Init() will call HAL_SPI_MspInit() after reading the properities of
	// the passed SPI_HandleTypeDef. After finishing the MspInit call, it will set
	// the SPI property bits. This is how all HAL_[peripheral]_Init() functions work.
}


void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	// SPI GPIO initialization structure here
	// Open Pin D10, 11, 12, 13
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

	// Enable GPIO B12, B15
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.Pin       = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Enable GPIO A11, A12
	GPIO_InitStruct.Pin       = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	// NSS pin need to be set to output mode
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pin       = GPIO_PIN_11;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	if (hspi->Instance == SPI2)
	{
		// Enable SPI GPIO port clocks, set HAL GPIO init structure's values for each
		// SPI-related port pin (SPI port pin configuration), enable SPI IRQs (if applicable), etc.
		// No GPIO is needed for Task 3 therefore no GPIO setup here
	}
}

void transChar(char input){
	unsigned char addr_CH_BUF = 0x05;
	unsigned char transInput = input;
	int i;
	// Transfer the input by register 5 CH_BUF
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &addr_CH_BUF,1,10);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &transInput,1,10);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	for( i = 0; i < 1000; i++){asm("nop");}
}

void receiveChar(){
	unsigned char addr_CH_BUF = 5;
	unsigned char addr_STS = 1;// Address of status register
	unsigned char dump = 0x00;
	unsigned char status1 = 0;
	unsigned char output[2];
	int i;
	while((status1&0x60U) == 0){// Will stay in loop until receive char is ready
		// Keep checking the STS_REG bit 5 and 6
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_SPI_Transmit(&SPI_handle_type, &addr_STS, 1, 10); // Choose status register
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_SPI_Receive(&SPI_handle_type, &status1, 1, 10); // Receive the number of char in buffer
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		for( i = 0; i < 1000; i++){asm("nop");}
	}
		// Receive the char from CH_BUF register
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_SPI_Transmit(&SPI_handle_type, &addr_CH_BUF,1,10);	// choose the CH_BUF register
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_SPI_TransmitReceive(&SPI_handle_type, &dump, output,1,40);	// Receive the continent of the register
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		for( i = 0; i < 1000; i++){asm("nop");}
		printf("\rReceive char:%c\r\n",output[0]);
}

void readFWVerion(void){
	int i;
	unsigned char addr_V_MAJ = 0x07;// Address V_MAJ
	unsigned char addr_V_MIN = 0x08;// Address V_MIN

	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &addr_V_MAJ, 1, 10);// Transmit the V_MAJ register address
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Receive(&SPI_handle_type, &V_MAJ, 1, 10);// Receive the first bit of firmware version
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &addr_V_MIN, 1, 10);// Transmit the V_MAJ register address
	for(i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Receive(&SPI_handle_type, &V_MIN, 1, 10);// Receive the last bit of firmware version
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

}

void ReadDeviceID(void){
	unsigned char bit_DEVID = 0x09;
	unsigned char DeviceID = 0;
	int i = 0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &bit_DEVID ,1,50); // locate register 9: DEVID Device ID number
	for(i=0; i<1000;i++){asm("nop");}
	HAL_SPI_Receive(&SPI_handle_type,&DeviceID,1,50);// read device ID
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET)
	for(i=0; i<1000;i++){asm("nop");}

	printf("\rDevice ID:%u\r\n",DeviceID);

}

void changeDeviceId(void){
	unsigned char bit_CTL_REG = 0;// Address control register
	unsigned char bit7 = 0x80;
	unsigned char bit_DEVID = 0x09;// Address of Device ID number
	unsigned char ID = 0;
	int IDint[1];
	unsigned char DeviceID = 0;
	int i = 0;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &bit_DEVID ,1,10); // locate register 9: DEVID Device ID number
	for(i=0; i<1000;i++){asm("nop");}
	HAL_SPI_Receive(&SPI_handle_type,&DeviceID,1,10);// read device ID
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET);


	printf("\rOld Device ID:%u\r\n",DeviceID);

	printf("Type in new Device ID: \r\n");
	ID = getchar();	// Get new ID
	putchar(ID);
	IDint[0] = ID - '0'; // Change the string to integer

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type,&bit_CTL_REG ,1,10); // locate register 2 : CTL_REG Control register
	for(i=0; i<1000;i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type,&bit7,1,10);// change bit 7 of register 2 to 1 enable ID modification
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET);

	for(i=0; i<1000;i++){asm("nop");}

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &bit_DEVID ,1,10); // locate register 9 : DEVID Device ID number
	for(i=0; i<1000;i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type,(uint8_t*) IDint,1,10);// send data
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET);

}

void readTemp(void){
	unsigned char addr_CTL = 0;// Address of control register
	unsigned char addr_STS = 1;// Address of status register
	unsigned char addr_TMP_L = 3;// Address of TMP_L register
	unsigned char addr_TMP_H = 4;// Address of TMP_H register
	unsigned char status = 0;// the status of tmp reading register
	unsigned char Bit1 = 0x02;
	int i = 0;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &addr_CTL, 1, 10); // Locate the Control register
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &Bit1 , 1, 10);	// choose Bit1, request temperature read
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	for( i = 0; i < 1000; i++){asm("nop");}

	while((status&0x08U) == 0){//will stay in loop until tmp read is ready
		// Check the bit 3 of the register whether the temperature is ready
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_SPI_Transmit(&SPI_handle_type, &addr_STS, 1, 10); // Locate the status register
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_SPI_Receive(&SPI_handle_type, &status, 1, 10); // Receive data from the register
		for( i = 0; i < 1000; i++){asm("nop");}
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
	}

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &addr_TMP_L , 1, 10); // Locate Low bit of the temperature measurement
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Receive(&SPI_handle_type, &TMP_LO , 1, 10);// Receive data
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);

	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type, &addr_TMP_H , 1, 10); // Locate High bit of the temperature measurement
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Receive(&SPI_handle_type, &TMP_HI , 1, 10);// Receive data
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
}


void ResetSlaveTerminal(void){
	unsigned char bit_CTL_REG = 0;
	unsigned char bit_TRM_CLR_RST = 0x0cU;	// bit 3 and 2 TRMRST and TRMCLR control the clear and rest terminal
	int i = 0;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type,&bit_CTL_REG ,1,10);// locate Control register
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_SPI_Transmit(&SPI_handle_type,&bit_TRM_CLR_RST,1,10); // Set bit 2 and 3
	for( i = 0; i < 1000; i++){asm("nop");}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET);
}

void menu(void){
	char user_in;
	printf("\033[5;0H");//Move to the position to print menu
	fflush(stdout);//flush stdout
	//print menu
	printf("1 -> Receive from peripheral device\r\n");
	printf("2 -> Read Temp\r\n");
	printf("3 -> Change Device ID\r\n");
	printf("4 -> Read Device ID\r\n");
	printf("5 -> Clear Terminal\r\n");
	user_in = getchar();

	if(user_in == '2'){
		readTemp();
		printf("\033[4;50H");//Move right to print tempreture
		fflush(stdout);//flush stdout
		temp1 = (((int)TMP_HI<<8) + TMP_LO);	// Assemble temperature
		temp1 = 357.6-0.187*temp1;	// Convert temperature
		printf("TMP: %d\r\n",temp1);

	}else if(user_in == '1'){
		printf("\033[12;0H");
		printf("\033[2K");
		fflush(stdout);
		receiveChar();
	}else if(user_in == '3'){
		printf("\033[20;24H");
		printf("\033[2K");
		fflush(stdout);
		changeDeviceId();
	}else if(user_in == '4'){
		printf("\033[2;0H");
		printf("\033[2K");
		fflush(stdout);
		ReadDeviceID();
	}
	else if(user_in == '5'){
		fflush(stdout);
		ResetSlaveTerminal();
	}
}

int main(void)
 {
	Sys_Init();
	//Configure SPI setups
	configureSPI();
	HAL_Delay(1000);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET); // Pull up the CS line
	printf("\033[2J\033[;H");	// clear the screen
	fflush(stdout);

	readFWVerion();//Initial read of firmware version
	printf("\033[1;24H");//Center cursor for head line print
	fflush(stdout);
	// Print the Firmware version
	printf("STaTS Firmware Version is %d.%d\r\n", V_MAJ,V_MIN);

	printf("\033[4;50H");
	fflush(stdout);

	char c;

	while(1){
		printf("\033[3;0H");
		printf("Enter ESC key for menu\r\n");
		// Receive the character from keyboard
		c = getchar();
		if(c == 0x1b){
			menu();
		}else{
			printf("\033[4;0H");
			putchar(c);
			transChar(c);
			fflush(stdout);
		}
	}

}
