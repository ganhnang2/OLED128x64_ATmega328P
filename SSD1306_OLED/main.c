/*
 * SSD1306_OLED.c
 *
 * Created: 2/10/2026 3:57:50 PM
 * Author : ASUS
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
	OLED_Clear_Buffer();
	uint8_t count = 0;
	char buffer[100];
    while (1) 
    {
		OLED_Clear_Buffer();
		sprintf(buffer, "Number: %d", count);
		OLED_Print_String(buffer, 0, 0);
		OLED_Update_Display();
		count++;
		_delay_ms(500);
    }
}



