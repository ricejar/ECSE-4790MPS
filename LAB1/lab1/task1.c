/*
 * Task 1 print the input char
 *
 *
 *
 */



#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>

void top_msg(){ // top message
	printf("\n");
	printf("                      PRESS <ESC> OR <CTL>+[ TO QUIT\n");
	printf("\n");
	printf("\033[6;0H");
}

int main(void)
{
    Sys_Init();
    top_msg();

    while(1){   //exit the porgramm with ESC or "^+["
        if (choice == 0x1B)
            {return 1;}

        printf("\rThe keyboard character is \r\n");
    }




}