/*
	OLED_128x64.c

 */ 

#include "OLED_128x64.h"

/*------------------------ Fundamental Function -----------------------*/

// Initialize OLED
void OLED_Init()
{
	// Enable I2C to transfer data with OLED
	I2C_Master_Init();
	
	/*	Steps to initialize	OLED (Look up the datasheet) */
	I2C_Start();						// Step 1: Send START condition
	I2C_Master_SLA_W(OLED_ADDRESS);		// Step 2: Slave Address + R/W# (bit 0 - Write mode)
	
	// This line indicate that OLED defines the following data byte as a command.
	OLED_WRITE_ALL_COMMANDS
	
	I2C_Write(0xAE);					// Step 3: Turn the OLED off
	I2C_Write(0xA8);					// Step 4: Set MUX ratio with command 0xA8, data 0x3F (MUX = 63 + 1 = 64)
	I2C_Write(0x3F);
	I2C_Write(0xD3);					// Step 5: Set Display Offset with command 0xD3, data 0x00
	I2C_Write(0x00);
	I2C_Write(0xA0);					// Step 6: Set Segment Re-map with A0 - column address 0 is mapped to SEG0 
	I2C_Write(0xC0);					// Step 7: Set COM Output Scan Direction with command 0xC0
	I2C_Write(0xDA);					// Step 8: Set COM Pins hardware configuration
	I2C_Write(0x12);
	I2C_Write(0x81);					// Step 9: Set Contrast Control with command 0x81, data: 0x7F (brightness = 127 + 1 = 128)
	I2C_Write(0x7F);
	I2C_Write(0xA4);					// Step 10: Set output follows RAM content  
	I2C_Write(0xA6);					// Step 11: Set Normal Display
	I2C_Write(0xD5);					// Step 12: Set OSC frequency with data 0x80
	I2C_Write(0x80);					// A7:4 = 8 and A3:0 = 1 -> setup fOsc, A3:0 = 1 -> Divide ratio = 1 + 1 = 2
	I2C_Write(0x8D);					// Step 13: Enable charge pump regulator
	I2C_Write(0x14);
	I2C_Write(0xAF);					// Step 14: Display on
	
	I2C_Stop();							// Step 15: Send STOP condition to finish the transmission
}

// Move cursor to specific location
// NOTE: This function is only suitable for the Page Addressing Mode
void OLED_Set_Cursor(uint8_t page, uint8_t col)
{		
	if (page > OLED_MAX_PAGE_INDEX || page < 0)
		page = OLED_MAX_PAGE_INDEX;
	if (col > OLED_MAX_COLUMN_INDEX || col < 0)
		col = OLED_MAX_COLUMN_INDEX;
		
	I2C_Start();
	I2C_Master_SLA_W(OLED_ADDRESS);		
	
	/*	Set column start address	*/
	OLED_WRITE_COMMAND
	I2C_Write(col & 0x0F);			// lower nibble
	OLED_WRITE_COMMAND
	I2C_Write(0x10 | (col >> 4));	// upper nibble
	
	/*	Set GDDRAM Page Start Address  */
	OLED_WRITE_COMMAND
	I2C_Write(0XB0 | page);
	
	I2C_Stop();
}	

// Modify the brightness
void OLED_Set_Brightness(uint16_t brightness)
{
	if (brightness > 255)
		brightness = 255;
		
	I2C_Start();
	I2C_Master_SLA_W(OLED_ADDRESS);
	OLED_WRITE_ALL_COMMANDS
	I2C_Write(0x81);
	I2C_Write(brightness);		
	
	I2C_Stop();
}

// Display mode
// This function create image displayed from top to bottom or otherwise.
// (Top: Header in OLED 128x64, Bottom: The bottom of the OLED)
// mode: Select the print style you want.
// 0: Normal mode (Top to Bottom)
// 1: Inverting mode (Bottom to Top) - remapped mode
void OLED_Display_Mode(uint8_t mode)
{
	I2C_Start();
	I2C_Master_SLA_W(OLED_ADDRESS);
	OLED_WRITE_ALL_COMMANDS
	
	if (mode == 0)
	{
		I2C_Write(0xC0);		// Scan from COM0 to COM[N-1]
		I2C_Write(0xA0);		// Column address 0 is mapped to SEG0
	}
	else if (mode == 1)
	{
		I2C_Write(0xC8);		// Scan from COM[N-1] to COM0
		I2C_Write(0xA1);		// Column address 127 is mapped to SEG0
	}
	
	I2C_Stop();
}
	
// Draw image
void OLED_Draw_Bit_Map(const uint8_t* image)
{
	uint8_t dataBuffer;
	for (uint8_t page = 0; page < 8; page++)
	{
		// Mover cursor to next page
		OLED_Set_Cursor(page, 0);
		
		// Send data to RAM
		I2C_Start();
		I2C_Master_SLA_W(OLED_ADDRESS);
		OLED_WRITE_ALL_DATAS
		
		// Display data on all columns at selected page
		for (uint8_t col = 0; col < 128; col++)
		{
			dataBuffer = pgm_read_byte(image);			// read data from FLASH memory
			I2C_Write(dataBuffer);
			image++;									// increment pointer
		}
		
		I2C_Stop();
	}
}	

// Sleep mode to save the energy
// This function make OLED sleep, but the data in GDRAM remains intact
void OLED_Sleep()
{
	I2C_Start();
	I2C_Master_SLA_W(OLED_ADDRESS);
	OLED_WRITE_ALL_COMMANDS
	I2C_Write(0x8D);				
	I2C_Write(0x10);				// Disable charge pump
	I2C_Write(0xAE);				// Display off
	I2C_Stop();
}	

// Clear all data in GDRAM
void OLED_Clear_Display()
{	
	for (uint8_t page = 0; page < 8; page++)
	{
		OLED_Set_Cursor(page, 0);
		
		I2C_Start();
		I2C_Master_SLA_W(OLED_ADDRESS);
		OLED_WRITE_ALL_DATAS
		
		for (uint8_t col = 0; col < 128; col++)
			I2C_Write(0x00);
			
		I2C_Stop();
	}
}

/*------------------------End for Fundamental Function-----------------------*/					

/*------------------------Display Function-----------------------*/	
// Clear data in Buffer	
void OLED_Clear_Buffer()
{
	for (uint16_t i = 0; i < 1024; i++)
		bufferRAM[i] = 0x00;
}

// Draw a pixel onto buffer at specific location (x,y)
// x - column, y - row
void OLED_Draw_Pixel(uint8_t x, uint8_t y)
{
	uint8_t page = y / 8;
	uint16_t index = (uint16_t)(page * 128) + x;		// the index of buffer
	uint8_t bit = y % 8;								// the row position within a page 
	bufferRAM[index] |= (1 << bit);						// draw a pixel
}

// Print character with font 5x7
void OLED_Print_Character(uint8_t ch, uint8_t x, uint8_t y)
{	
	// The staring address of selected character in array OLED_FONT_DATA
	const uint8_t* startPtr = OLED_FONT_DATA + (ch - 32) * 5;
	uint8_t data;
	for (uint8_t i = 0; i < OLED_CHAR_WIDTH; i++)
	{
		data = pgm_read_byte(startPtr + i);
		for (uint8_t j = 0; j < OLED_CHAR_HEIGHT; j++)
		{
			// Each element of the bufferRAM[] array corresponds to an 8-bit data value 
			// for one column within a given PAGE
			if (data & (1 << j))
				OLED_Draw_Pixel(x + i, y + j);
		}
	}
}

// Display string on OLED
// x - column, y - row
void OLED_Print_String(const char* str, uint8_t x, uint8_t y)
{
	uint8_t i = 0;
	
	// Write data onto buffer 
	while (str[i] != '\0')
	{
		OLED_Print_Character(str[i], x, y);
		i++;
		
		// If the cursor exceeds the allowed column, move to a new line
		if (x + OLED_CHAR_WIDTH + OLED_SEPARATE_SPACING > OLED_MAX_COLUMN_INDEX)
		{
			y += (OLED_CHAR_HEIGHT + OLED_SEPARATE_SPACING);		// +1: Create a separate space between the characters
			x = 0;					// reset to the first index column
			
			// Special case: The cursors go beyonds the maximum row limit
			if (x > OLED_MAX_ROW_INDEX)	
				break;
		}
		else
			x += (OLED_CHAR_WIDTH + OLED_SEPARATE_SPACING );		// move onto
	}	
}

// This function is generated for writing buffer to GDRAM 
// -> Therefore, it must be used after calling the function OLED_Print_String()
void OLED_Update_Display()
{
	// Burst Write buffer to GDRAM
	for (uint8_t page = 0; page <= OLED_MAX_PAGE_INDEX; page++)
	{
		OLED_Set_Cursor(page, 0);
		
		I2C_Start();
		I2C_Master_SLA_W(OLED_ADDRESS);
		OLED_WRITE_ALL_DATAS
		
		uint16_t position = page * 128;
		for (uint8_t col = 0; col <= OLED_MAX_COLUMN_INDEX; col++)
		{
			I2C_Write(bufferRAM[position + col]);
		}
		
		I2C_Stop();
	}
}

/*------------------------End For Display Function-----------------------*/	

/*------------------------Scrolling Function-----------------------*/					

/*	Horizontal Scroll Setup */
// This function has two parameters:
// +) startPage: Scrolling start page
// +) endPage: Scrolling end page
// NOTE: Scrolling speed - time interval between each scroll step in terms of 
// frame frequency (E.x: 3 frames, 5 frames, ...) -> Look up the datasheet
void OLED_Horizontal_Scroll(uint8_t startPage, uint8_t endPage)
{
	if (startPage > 7 || startPage < 0)
		startPage = 0;
	if (endPage > 7 || endPage < 0)
		endPage = 7;

	I2C_Start();
	I2C_Master_SLA_W(OLED_ADDRESS);
	OLED_WRITE_ALL_COMMANDS
	
	I2C_Write(0x2E);		// Deactivate scroll 
	I2C_Write(0x26);		// Default: Right Horizontal Scroll   
	// I2C_Write(0x27)		// Left Horizontal Scroll 
	I2C_Write(0x00);		// Dummy Byte
	I2C_Write(startPage);	// Start page address
	I2C_Write(0x06);		// Set timer interval as 25 frames
	I2C_Write(endPage);		// End page address
	I2C_Write(0x00);		// Two dummy bytes
	I2C_Write(0xFF);
	I2C_Write(0x2F);		// Active scroll
	I2C_Stop();
}		


/*	Continuous Vertical and Horizontal Scroll Setup	*/
// verticalOffset: vertical scrolling offset
void OLED_Vertical_And_Horizontal_Scroll(uint8_t startPage, uint8_t endPage, uint8_t verticalOffset)
{
	if (verticalOffset > 63 || verticalOffset < 0)
		verticalOffset = 1;

	I2C_Start();
	I2C_Master_SLA_W(OLED_ADDRESS);
	OLED_WRITE_ALL_COMMANDS
	
	I2C_Write(0x2E);			// Deactivate scroll
	I2C_Write(0x29);			// Default: Vertical and Right Horizontal Scroll
	//I2C_Write(0x2A)			// Vertical and Left Horizontal Scroll
	I2C_Write(0x00);			// Dummy Byte
	I2C_Write(startPage);		// Start page address
	I2C_Write(0x07);			// Set timer interval as 2 frames
	I2C_Write(endPage);			// End page address
	I2C_Write(verticalOffset);	// offset
	I2C_Write(0x2F);			// Active scroll
	I2C_Stop();
}

/*	Vertical Scroll Setup -> Test failure, need to modify */
// This function supports defining the scrollable area and the fixed area.
// +) fixedRows: Set number of rows in top fixed area. Fixed area is referenced to the
// top of the GDDRAM (i.e row 0).
// +) scrollingRows: Set number of rows  to be used for vertical scrolling. The scroll area
// starts in the first row below the top fixed area.
// Example: fixedRow = 5, scrollRow = 3 -> Row 0 - 4: fixed area, row 5 - 7: scroll area
// NOTE: 
// 1. fixedRow + scrollRow <= MUX ratio (in this sketch, i have assigned MUX ratio to 64)
// 2. When command A3h is issued, a command 29h/A2h must be issued immediately after it.
void OLED_Scroll_Area(uint8_t fixedRows, uint8_t scrollingRows)
{
	// In this sketch, i have assigned MUX ratio to 64
	if (fixedRows + scrollingRows > 64)
	{
		fixedRows = 0;
		scrollingRows = 64;
	}
	
	I2C_Start();
	I2C_Master_SLA_W(OLED_ADDRESS);
	OLED_WRITE_ALL_COMMANDS
	
	I2C_Write(0x2E);			// Deactivate scroll
	
	/*	Vertical Scroll Area	*/
	I2C_Write(0xA3);			// Vertical Scroll Area
	I2C_Write(fixedRows);		// Set number of rows in fixed and scroll area	
	I2C_Write(scrollingRows);
	
	/*	Continuous Vertical and Horizontal Scroll Setup	*/
	I2C_Write(0x29);			// Default: Vertical and Right Horizontal Scroll
	//I2C_Write(0x2A)			// Vertical and Left Horizontal Scroll
	I2C_Write(0x00);			// Dummy Byte
	I2C_Write(0x00);			// Start page address
	I2C_Write(0x06);			// Set timer interval as 25 frames
	I2C_Write(0x07);			// End page address
	I2C_Write(0x01);			// offset
	
	I2C_Write(0x2F);			// Active scroll
	I2C_Stop();
}	
		
/*------------------------End for Scrolling Function-----------------------*/		