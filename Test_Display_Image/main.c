/*
 * Test_Display_Image.c
 
 */ 

#define F_CPU		12000000UL			// Clock CPU: 12MHz
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "OLED_128x64.h"
#include "My_Image.h"

int main(void)
{
	OLED_Init();
	OLED_Display_Mode(1);
	while (1)
	{
		OLED_Draw_Bit_Map(bitmap_image);
		_delay_ms(500);
		OLED_Draw_Bit_Map(bitmap_UET);
		_delay_ms(500);
	}
}

