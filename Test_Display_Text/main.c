/*
	SSD1306_OLED.c
 */ 


#define F_CPU		12000000UL			// Clock CPU: 12MHz
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "OLED_128x64.h"

int main(void)
{
	OLED_Init();
	OLED_Display_Mode(1);
	OLED_Clear_Buffer();
	uint8_t count = 0;
	char buffer[100];
    while (1) 
    {
		OLED_Clear_Buffer();						// Reset buffer to prevent overwriting
		OLED_Print_String("Nguyen Hai Nam", 0, 20);
		OLED_Print_String("UET", 0, 30);
		sprintf(buffer, "Number: %d", count);		
		OLED_Print_String(buffer, 0, 0);			// Update data on bufferRAM[]
		OLED_Update_Display();						// Mapping bufferRAM[] to the GDRAM of OLED128x64
		count++;
		_delay_ms(500);
    }
}



