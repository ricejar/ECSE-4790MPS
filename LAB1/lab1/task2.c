/*
 *
 * Task 2 ANSI Terminal Control Sequences
 *
 */

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>

void red(){ // print char color red
	printf("\033[1;31m");
}

void yellow(){  //print char color yellow
	printf("\033[1;33m");
}

void top_msg(){ //top message printed
	printf("\n");
	printf("                      PRESS <ESC> OR <CTL>+[ TO QUIT\n");
	printf("\n");
	printf("\033[6;0H");
}


int main(void)
{
    Sys_Init(); 

    char choice;
    int row = 12;
    int init_char = 0;

    printf("\033[0;33;44m");
    printf("\033[2J\033[;H"); 

    fflush(stdout);


    top_msg();

    while(1)
    {
		choice = getchar(); //get input
        // quit the programm with ESC pr "^+[""
        if (choice == 0x1B)
            {return 1;}

        // For the case which the char is not printable
        if(choice < 0x21 || choice > 0x7E){
        	if (row < 25){
        		printf("\033[5;33;44m");	//set blinking, font and background color
        		printf("\033[%d;1H",row);
        		if (row < 24){  // when the line is less than 24
                    // set underline
        			printf("The keyboard character $%02x is \e[4m\'not printable\'\e[0m\a\n",choice);
        			row++;
        		}
        		else{   // when the line is 24
        			printf("The keyboard character $%02x is \e[4m\'not printable\'\e[0m\a",choice);
        			fflush(stdout);
        			row++;
        		}

        	}
        	else{   // when the number of line is larger than 24
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
        	  if (init_char == 0){  //First time print on the screen
        		  printf("\033[6;0H");
        		  printf("\033[0;33;44m");
        	  	   printf("\rThe keyboard character is ");
        	  	   red();   // set the char red
        	  	   putchar(choice);
        	  	   yellow();
        	  	   printf("\r\n");
        	  	   init_char = 1;   // set the varibale to indicate the line is printed once
        	  }
        	  else{
        		  printf("\033[6;27H"); //change only the input character
        		  printf("\033[k");
        		  printf("\033[0;31;44m");
        		  putchar(choice);
        		  printf("\n");
        		  printf("\033[0;33;44m");
        	  }
          }
    }
}
