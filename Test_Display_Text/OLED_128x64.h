/*
	OLED_128x64.h
	This implementation is only suitable for SSD1306 OLED 128x64
	
	Source: Nguyen Hai Nam
 */ 

#ifndef OLED_128X64_H_
#define OLED_128X64_H_

#ifndef	F_CPU
#define F_CPU	12000000UL
#endif

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "TWI_Master.h"
#include "global.h"

/****************************MARCO**************************/

/*	Define the parameters for the print function	*/
// Can modify these settings for your purpose
#define OLED_CHAR_HEIGHT		7						// the size of character 5x7
#define OLED_CHAR_WIDTH			5
#define OLED_SEPARATE_SPACING	1						// the distance (pixel) between two sequential characters
#define OLED_FONT_DATA			font_data_5x7			// generated character array

/*	Define the specification	
	These parameters can be modified to be compatible with various OLED type	*/
#define OLED_ADDRESS				0x3C
#define OLED_MAX_COLUMN				128
#define OLED_MAX_ROW				64						
#define OLED_MAX_PAGE				8

// Don't modify
#define OLED_MAX_ROW_INDEX			(OLED_MAX_ROW - 1)
#define OLED_MAX_COLUMN_INDEX		(OLED_MAX_COLUMN - 1)
#define OLED_MAX_PAGE_INDEX			(OLED_MAX_PAGE - 1)

/*	Define control bytes	*/
// Control Bytes:
// +) C0 (Bit 7): 
// 0 -> the transmission of the following information will contain data bytes only.
// +) D/C# (Bit 6):
// 1 -> data byte is interpreted as display data written to Graphic Display Data RAM (GDDRAM).
// 0 -> data byte is interpreted as a command			
// +) Bit 5:0 = 0
#define OLED_WRITE_COMMAND			I2C_Write(0x80);
#define OLED_WRITE_DATA_TO_RAM		I2C_Write(0xC0);
#define OLED_WRITE_ALL_COMMANDS		I2C_Write(0x00);
#define OLED_WRITE_ALL_DATAS		I2C_Write(0x40);

/****************************END FOR MARCOS**************************/

/**************************PROTOTYPE FUNCTIONS*************************/
/*	Fundamental Function  */
void OLED_Init();											// Initialize OLED
void OLED_Draw_Bit_Map(const uint8_t* image);				// Draw image
void OLED_Set_Cursor(uint8_t page, uint8_t col);			// Move cursor to specific location (page, column)
void OLED_Set_Brightness(uint16_t brightness);				// Modify the brightness
void OLED_Display_Mode(uint8_t mode);						// Display mode
void OLED_Sleep();											// Sleep mode to save the energy
void OLED_Clear_Display();									// Clear all data in GDRAM

/*	Display Function	*/	
void OLED_Draw_Pixel(uint8_t x, uint8_t y);						// Turn on pixel at selected position (x, y)
void OLED_Clear_Buffer();										// Clear data in bufferRAM[]
void OLED_Print_Character(uint8_t ch, uint8_t x, uint8_t y);	// Helper function of OLED_Print_String()
void OLED_Print_String(const char* str, uint8_t x, uint8_t y);	// Write data on bufferRAM[]
void OLED_Update_Display();										// Display on OLED by mapping bufferRAM[] to GDRAM

/*	Scrolling Function	*/
// NOTE: No continuous vertical scrolling is available in SSD1306 OLED 128x64 -> From the datasheet
void OLED_Horizontal_Scroll(uint8_t startPage, uint8_t endPage);
void OLED_Scroll_Area(uint8_t fixedRows, uint8_t scrollingRows);
void OLED_Vertical_And_Horizontal_Scroll(uint8_t startPage, uint8_t endPage, uint8_t verticalOffset);

/***********************END FOR PROTOTYPE FUNCTIONS*************************/

#endif /* OLED_128X64_H_ */