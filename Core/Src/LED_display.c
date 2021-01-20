#include "LED_display.h"
#include "gpio.h"

const int b7SegmentTable[10] = {
	0b1111110, /* 0 */
	0b0011000, /* 1 */
	0b1010111, /* 2 */
	0b1011011, /* 3 */
	0b0111001, /* 4 */
	0b1101011, /* 5 */
	0b1101111, /* 6 */
	0b1011000, /* 7 */
	0b1111111, /* 8 */
	0b1111011  /* 9 */
};

const int b7SegmentTable_Mode[4] = {
	0b0000000, /* mode 0 - nothing is displayed */
	0b0000001, /* mode 1 - line G is displayed */
	0b0000010, /* mode 2 - line D is displayed */
	0b1000000, /* mode 3 - line A is displayed */
};

void display_num(int num, int dot) {
	//function that displays multi-digit numbers
	if (num < 10000) {

		if (num < 10) {
			GPIOC->ODR = (0b11100 << 7) | b7SegmentTable[num] ;
			/*first operand in OR operation represents (COM1, COM2, COM3, COM4, DP) - first 7 bits in ODR*/
			HAL_Delay(10);

		} else if (num >= 10 && num < 100) {
			GPIOC->ODR = (0b1101 << 8) | (dot << 7) | b7SegmentTable[num / 10];
			HAL_Delay(1);
			GPIOC->ODR = (0b1110 << 8) | b7SegmentTable[num % 10];
			HAL_Delay(1);

		} else if (num >= 100 && num < 1000) {
			GPIOC->ODR = (0b1011 << 8) | b7SegmentTable[num / 100];
			HAL_Delay(1);
			GPIOC->ODR = (0b1101 << 8) | (dot << 7) | b7SegmentTable[num % 100 / 10];
			HAL_Delay(1);
			GPIOC->ODR = (0b1110 << 8) | b7SegmentTable[num % 100 % 10];
			HAL_Delay(1);

		} else if (num >= 1000 && num < 10000) {
			GPIOC->ODR = (0b0111 << 8) | b7SegmentTable[num / 1000];
			HAL_Delay(1);
			GPIOC->ODR = (0b1011 << 8) | b7SegmentTable[num % 1000 / 100];
			HAL_Delay(1);
			GPIOC->ODR = (0b1101 << 8) | (dot << 7) |  b7SegmentTable[num % 1000 % 100 / 10];
			HAL_Delay(1);
			GPIOC->ODR = (0b1110 << 8) | b7SegmentTable[num % 1000 % 100 % 10];
			HAL_Delay(1);
		}
	}
}

void display_mode(int mode) {
	//display mode on left digit
	GPIOC->ODR =(0b01110 << 7) | b7SegmentTable_Mode[mode];
    HAL_Delay(1);
}

void display(int num, int mode, int dot){
	//function that displays mode and numbers simultaneously
	display_mode(mode);
	display_num(num, dot);
}



