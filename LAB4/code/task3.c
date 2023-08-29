/*
 * task3.c
 *
 *  Created on: Oct 23, 2021
 *      Author: wangy62
 */

#include "init.h"




int main(void){

	Sys_Init();
	uint32_t result;
	int32_t int1 = 1145;
	int32_t int2 = 1919;
	int32_t int3  = 0;
	int32_t i2 = 2;
	int32_t i3 = 3;
	int32_t i5 = 5;
	int32_t temp = 0;
	int32_t x = 539;
	int32_t y = 0;
	int32_t z = 0;
	unsigned char c;

	printf("\033[2J\033[;H");
	printf("start\r\n");
	fflush(stdout);

	asm("LDR r1, =0x00005D8A");
	asm("LDR r2, =0x0001B742");
	asm("ADD r3, r1 , r2");
	asm("STR r3,%0" : "=m" (result));

	printf("Result hex: %ld, dec: %lx \r\n",result,result);
	//int3 = int1 * int2;
	//asm("MUL %0,%1,%2" : "+i" (int3) : "i" (int2) , "i" (int1));
	asm("MUL %[dest],%[tmp1],%[tmp2]" : [dest] "=r" (int3) : [tmp1] "r" (int1), [tmp2] "r" (int2));
	printf("Result hex: %ld, dec: %lx \r\n",int3,int3);

	//asm volatile ("MUL r4,%[tmp1],%[tmp2]" : : [tmp1] "r" (i2), [tmp2] "r" (x));
	//asm volatile ("UDIV %[dest],r4,%[tmp]" :[dest] "=r" (y) : [tmp] "r" (i3));
	//asm volatile ("ADD %[dest],r5,%[tmp]" : [dest] "=r" (y): [tmp] "r" (i5));
	asm volatile ("MUL %[dest],%[tmp1],%[tmp2]" : [dest] "=r" (y) : [tmp1] "r" (i2), [tmp2] "r" (x));
	asm volatile ("SDIV %[dest],%[tmp1],%[tmp2]" :[dest] "=r" (z) : [tmp1] "r" (y), [tmp2] "r" (i3));
	asm volatile ("ADD %[dest],%[tmp1],%[tmp2]" : [dest] "=r" (y): [tmp1] "r" (i5), [tmp2] "r" (z));
	printf("Result hex: %ld, dec: %lx \r\n",y,y);

	x = 539;
	asm("LDR r8 , =0x0000000F");
	asm("LDR r9 , =0x00000002");
	asm("LDR r7 , =0x00000003");
	asm volatile("MLA %[dest], %[tmp1], r9, r8" : [dest] "=r" (int3) : [tmp1] "r" (x));
	//asm volatile("MLA %[dest],%[tmp1],%[tmp2], r5" : [dest] "=r" (temp) : [tmp1] "r" (x), [tmp2] "r" (i2));
	//asm volatile("SDIV %[dest],%[tmp1],%[tmp2]": [dest] "=r" (temp) : [tmp1] "r" (int3), [tmp2] "r" (i3));
	asm volatile("SDIV %[dest], %[tmp1], r7" : [dest] "=r" (temp) : [tmp1] "r" (int3) );
	printf("Result hex: %ld, dec: %lx \r\n",temp,temp);


	while(1){
		c = getchar();
	}


}
