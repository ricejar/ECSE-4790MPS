/*
 * task4.c
 *
 *  Created on: Sep 14, 2021
 *      Author: wangy62
 */

//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include "stm32f769xx.h"
#include "hello.h"

#include<stdint.h>

GPIO_InitTypeDef GPIO_InitStruct;
//set up the wall of maze
int maze[110]={101,102,103,104,105,106,107,108,109,110,111,112,113,114,
		201,214,301,302,303,305,306,307,309,310,311,312,313,314,401,407,
		409,414,501,503,504,505,507,509,511,513,514,601,603,605,611,614,
		701,703,705,706707,708,709,710,711,712,714,801,803,809,814,901,905,
		906,907,909,911,912,913,914,1001,1002,1003,1004,1005,1007,1014,1101,
		1107,1109,1110,1111,1114,1201,1203,1204,1205,1209,1213,1214,1301,1305,
		1307,1309,1311,1314,1401,1402,1403,1404,1405,1406,1407,1408,1409,1410,1411,1412,1413,1414};
int maze_x = 0;
int maze_y = 0;

void LED_GPIO_Init(void);
void INPUT_GPIO_Init(void);
void maze_init(void);
bool find_maze(int x, int y, int maze[]);

int main(void)
{
    Sys_Init();

    char choice;
    int player_y=2;	//initialization of player position
    int player_x=2;

    printf("\033[0;33;44m");
    printf("\033[2J\033[;H"); // Erase screen & move cursor to home position
    fflush(stdout);

    LED_GPIO_Init();
    INPUT_GPIO_Init();
    maze_init();


    while(1){

		//input from keyboard
    	choice = getchar();
    
    	//player moving up
    	if(choice == 0x57 || choice  == 0x77){ // type "a" or "A"
    		player_y --;
    		if (find_maze(player_y, player_x, maze)){	//Cannot move to that position 
    			printf("\033[20;1H");
    			printf("\033[K");
    			printf("CANNOT MOVE TO THAT POSITION\n");
    			player_y ++;
    			printf("\033[%d;%dH",player_y, player_x);
    			fflush(stdout);
    		}else{	//player moving up
    			printf("\033[20;1H");
    			printf("\033[K");
    			printf("Player position (%d,%d)\r\n",player_y, player_x);
    			printf("\033[%d;%dH",player_y, player_x);
    			fflush(stdout);
    		}
    	}

    	//player moving right
    	if(choice == 0x41 || choice  == 0x61){	//Cannot move to that position 
    	    player_x --;
    	    if (find_maze(player_y, player_x, maze)){
    	    	printf("\033[20;1H");
    	    	printf("\033[K");
    	    	printf("CANNOT MOVE TO THAT POSITION\n");
    	    	player_x ++;
    	    	printf("\033[%d;%dH",player_y, player_x);
    	    	fflush(stdout);
    	    }else{	//player moving right
    	    	printf("\033[20;1H");
    	    	printf("\033[K");
    	    	printf("Player position (%d,%d)\r\n",player_y, player_x);
    	    	printf("\033[%d;%dH",player_y, player_x);
    	    	fflush(stdout);
    	    }
    	}

    	//player moving down
    	if(choice == 0x53 || choice  == 0x73){	//Cannot move to that position 
    	    player_y ++;
    	    if (find_maze(player_y, player_x, maze)){
    	    	printf("\033[20;1H");
    	    	printf("\033[K");
    	    	printf("CANNOT MOVE TO THAT POSITION\n");
    	    	player_y --;
    	    	printf("\033[%d;%dH",player_y, player_x);
    	    	fflush(stdout);
    	    }else{		//player moving down
    	    	printf("\033[20;1H");
    	    	printf("\033[K");
    	    	printf("Player position (%d,%d)\r\n",player_y, player_x);
    	    	printf("\033[%d;%dH",player_y, player_x);
    	    	fflush(stdout);
    	    }
    	}

    	//player moving left
    	if(choice == 0x44 || choice  == 0x64){
    	   player_x ++;
    	   if (find_maze(player_y, player_x, maze)){	//Cannot move to that position 
    	    	printf("\033[20;1H");
    	    	printf("\033[K");
    	    	printf("CANNOT MOVE TO THAT POSITION\n");
    	    	player_x --;
    	    	printf("\033[%d;%dH",player_y, player_x);
    	    	fflush(stdout);
    	   }else{	//player moving down
    	    	printf("\033[20;1H");
    	    	printf("\033[K");
    	    	printf("Player position (%d,%d)\r\n",player_y, player_x);
    	    	printf("\033[%d;%dH",player_y, player_x);
    	    	fflush(stdout);
    	   }
    	}

    	if (player_y == 13 && player_x == 13){
    		printf("\033[20;1H");
    		printf("\033[K");
    		printf("YOU WIN!!! Push button to restart\n");
    		printf("\033[13;13H");
    		fflush(stdout);
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_SET);	//turn on the LED1
			//Hold the porgram until the pushbutton pushed
    		while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7));
    		//Restart the maze game
    		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);	//reset LED1
    		choice = "";
    		printf("\033[20;1H");	//remove the player status line
    		printf("\033[K");
    		printf("\n");
    		printf("\033[2;2H");	//reset play location
    		fflush(stdout);
    		player_y=2;
    		player_x=2;
    	}
    }
}

//------------------open LED1 for end fo the game------------------------
void LED_GPIO_Init(void){
	__HAL_RCC_GPIOJ_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_13;	//GPIO J13 for LED1
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);

}

//------------------pushbutton initialization----------------------------
void INPUT_GPIO_Init(void){
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//------------------Maze initialization-----------------------------------
void maze_init(void){
	//construct the maze on screen
	for (int i=0; i<110; i++){
		maze_x = maze[i] % 100;	//last 2 digits as row
		maze_y = maze[i] / 100; //first 2 digits as column
		printf("\033[%d;%dH",maze_x,maze_y);
		printf("#");
	}
	//Set destination
	printf("\033[13;13H");
	printf("*");
	fflush(stdout);
	//Print game information
	printf("\033[16;1H");
	printf("Game on! you are on the top left corner\r\n");
	printf("The destination is on the bottom right corner with a symbol *\r\n");
	printf("Push A,S,W,D FOR LEFT, DOWN, UP, RIGHT\r\n");
	printf("\033[2;2H");
	fflush(stdout);
	//Turn off Led1
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_13, GPIO_PIN_RESET);
}

//find if the position is a maze wall if it si return TRUE ,if it is not return FALSE
bool find_maze(int x, int y, int maze[]){
	maze_x = 0;
	maze_y = 0;
	for (int i=0; i<110; i++){
		maze_x = maze[i] % 100;
		maze_y = maze[i] / 100;
		if (maze_x == x && maze_y == y){
			return TRUE;
		}
	}
	return FALSE;
}


