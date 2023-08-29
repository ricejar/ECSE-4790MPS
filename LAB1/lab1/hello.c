//------------------------------------------------------------------------------------
// Hello.c
//------------------------------------------------------------------------------------
//
// Test program to demonstrate serial port I/O.  This program writes a message on
// the console using the printf() function, and reads characters using the getchar()
// function.  An ANSI escape sequence is used to clear the screen if a '2' is typed.
// A '1' repeats the message and the program responds to other input characters with
// an appropriate message.
//
// Any valid keystroke turns on the green LED on the board; invalid entries turn it off
//


//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>


//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void red(){
	printf("\033[1;31m");
}

void yellow(){
	printf("\033[1;33m");
}

void top_msg(){
	printf("\n");
	printf("                      PRESS <ESC> OR <CTL>+[ TO QUIT\n");
	printf("\n");
	printf("\033[6;0H");
}


int main(void)
{
    Sys_Init(); // This always goes at the top of main (defined in init.c)

    char choice;
    int row = 12;
    int init_char = 0;

    printf("\033[0;33;44m");
    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position

    fflush(stdout); // Need to flush stdout after using printf that doesn't end in \n


    top_msg();
    // Need to enable clock for peripheral bus on GPIO Port J
    __HAL_RCC_GPIOJ_CLK_ENABLE(); 	// Through HAL
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOJEN; // or through registers
    //// Below two lines are example on how to access a register by knowing it's memory address
    //volatile uint32_t * RCC_AHB1_CLOCK_ENABLE = (uint32_t*) 0x40023830U; // Enable clock for peripheral bus on GPIO Port J
    //*RCC_AHB1_CLOCK_ENABLE |= 512U; // Bitmask for RCC AHB1 initialization: 0x00000200U or 512U in decimal

    GPIOJ->MODER |= 1024U; //Bitmask for GPIO J Pin 5 initialization (set it to Output mode): 0x00000400U or 1024U in decimal
    GPIOD->MODER |= 256U;

    GPIOJ->BSRR = (uint16_t)GPIO_PIN_5; // Turn on Green LED (LED2)
    GPIOJ->BSRR = (uint32_t)GPIO_PIN_5 << 16; // Turn off Green LED (LED2)
    GPIOJ->ODR ^= (uint16_t)GPIO_PIN_5; // Toggle LED2

    GPIOD->BSRR > 0x00000010U;
    //GPIOD->ODR ^= (uint16_t)GPIO_PIN_4; // Toggle LED4

// It doesn't get lower level than this!
//    volatile uint32_t * GREENLEDMODER = (uint32_t*) 0x40022400U; // Init GPIO J Pin 5 (LED2 with no Alt. func.) to Output
//    *GREENLEDMODER |= 1024U; // Bitmask for GPIO J Pin 5 initialization: 0x00000400U or 1024U in decimal

    volatile uint32_t * GREENLEDBSRR = (uint32_t*) 0x40022418U; // Address of GPIO J Bit Set/Reset Register
    *GREENLEDBSRR = (uint16_t)0x0020U; // Turn on Green LED (LED2)

    HAL_Delay(1000); // Pause for a second

//    volatile uint32_t * GREENLEDODR = (uint32_t*) 0x40022414U; // Address of GPIO J Output Data Register
//    *GREENLEDODR ^= (uint16_t)0x0020U; // Toggle Green LED (LED2)

    while(1)
    {
    	//choice = "";
        //printf("Hello World!\r\n\n");
        //printf("( Welcome to Microprocessor Systems )\r\n\n\n");
        //printf("1=repeat, 2=clear, 0=quit.\r\n\n"); // Menu of choices


    	// Don't forget to end printf with newline or run fflush(stdout) after it!

        //choice = uart_getchar(&USB_UART, 1);
				choice = getchar();
				//putchar(choice);

// Messing around with stuff:
//        putchar('9'); // Putchar is weird, man.
//				choice = uart_getchar(&USB_UART, 0);
//				uart_putchar(&USB_UART, &choice);
//				puts("stuff\r\n");

        // select which option to run
//        HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, GPIO_PIN_SET);         // Turn green LED on (GPIO_PIN_SET == 1)
        *GREENLEDBSRR = (uint16_t)0x0020U; // Turn on Green LED (LED2)
        if (choice == 0x1B)
            {return 1;}
//        else if(choice == '1')
 //           printf("\r\n\nHere we go again.\r\n\n");
//        else if(choice == '2')          // clear the screen with <ESC>[2J
//        {
//            printf("\033[2J\033[;H");
//            fflush(stdout);
//        }
        if(choice < 0x21 || choice > 0x7E){
        	if (row < 25){
        		printf("\033[5;33;44m");	//set blinking, font and background color
        		printf("\033[%d;1H",row);
        		if (row < 24){
        			printf("The keyboard character $%02x is \e[4m\'not printable\'\e[0m\a\n",choice);

        			row++;
        		}
        		else{
        			printf("The keyboard character $%02x is \e[4m\'not printable\'\e[0m\a",choice);
        			fflush(stdout);
        			row++;

        		}

        	}
        	else{
        		printf("\033[5;33;44m");//set attribute blinking
        		printf("\033[12;24r");
        		printf("\033M");	//scroll up
        		printf("\033[24;1H");
        		printf("\033[k");	//erase new line for blue background
        		printf("\n"); // activate scroll up and erase
        		printf("\033[24;1H");
        		printf("The keyboard character $%02x is \e[4m\'not printable\'\e[0m\a", choice);
        		fflush(stdout);		//print the new line
        	}
        }
        else {
        	  if (init_char == 0){
        		  printf("\033[6;0H");
        		  printf("\033[0;33;44m");
        	  	   printf("\rThe keyboard character is ");
        	  //putchar(choice);
        	  //uart_print(&USB_UART, choice);
        	  	   red();
        	  	   putchar(choice);
        	  	   yellow();
        	  	   printf("\r\n");
        	  	   init_char = 1;
        	  }
        	  else{
        		  printf("\033[6;27H");
        		  printf("\033[k");
        		  printf("\033[0;31;44m");
        		  putchar(choice);
        		  printf("\n");
        		  printf("\033[0;33;44m");
        	  }
//            // inform the user how bright he is
//            *GREENLEDBSRR = (uint32_t)0x0020U << 16; // Turn off Green LED (LED2)
//            printf("\r\nA \"");
//						putchar(choice);
//						uart_print(&USB_UART, choice);
//            printf("\" is not a valid choice.\r\n\n");
          }

// Messing around with more stuff
//				printf("Enter an alphanumeric sequence (press <enter> if you want to send less than %d characters): \r\n", size + 1);
//				int a = uart_getline(&USB_UART, in, size);
//				printf("\r\nuart_getline result: %d\r\n", a);


    }
}

//------------------------------------------------------------------------------------
//Extra thing to consider...
//------------------------------------------------------------------------------------
void serial_print_things(void) {
	//Input Buffer
	char input[2];
	input[0]=0;
	input[1]=0;

	//Initialize the system
	Sys_Init();
	initUart(&Second_UART, 9600, USART6); // Allow printing over USART6 (Arduino pins D0 - Rx and D1 - TX)
	uart_print(&USB_UART, "\033[2J\033[;H");
	uart_print(&Second_UART, "\033[2J\033[;H");
	uart_print(&USB_UART, "Hello World: This is the USB Serial Connection\r\n");
	uart_print(&Second_UART, "Hello World: This is the other UART Connection\r\n");
	uart_print(&USB_UART, "In order to send newlines to this terminal,\r\n");
	uart_print(&USB_UART, "Press <ESC> and type \"[20h\" (without quotes)\r\n");
	uart_print(&USB_UART, "To get out of newline mode and back to line feed mode,\r\n");
	uart_print(&USB_UART, "Press <ESC> and type \"[20l\" (without quotes)\r\n");
	uart_print(&USB_UART, "\"Thanks for Playing!\"\r\n");

	printf("THIS SENTENCE USES PRINTF!!!\r\n");
  // Don't forget to end printf with newline or run fflush(stdout) after it!

	while(1) {
		input[0]=uart_getchar(&USB_UART, 0);
		uart_print(&USB_UART, input);
		uart_print(&Second_UART, input);
	}

	while(1);// HALT AND CATCH FIRE
}
