# SSD1306 128x64 OLED Library

## Overview of SSD1306 128x64 OLED

- OLED (Organic Light Emitting Diode) emits light when an electric current passes through it. OLED screens operate without a backlight, so they can display very deep blacks. They are compact in size and lighter than liquid crystal displays (LCDs).
- The 128x64 OLED screen is a simple dot matrix graphic display. It has 128 columns and 64 rows, making a total of 128 × 64 = 8192 pixels. By turning the LEDs of each pixel on or off, we can display graphic images of any shape.
- Communication interfaces for OLED can include I2C, SPI, etc. In this project, I use an OLED that uses the I2C protocol for communication.

## SSD1306 128x64 OLED Specifications

- Display Type: OLED (Organic Light Emitting Diode)
- Screen Size: 128x64 pixels
- Controller Driver: SSD1306
- Screen Color: Monochrome (White), Yellow, and Blue
- Operating Voltage: 3.3V to 5V
- Interface: I2C
- Operating Current: ~20mA

## OLED Screen Pinout (I2C Interface)

- SDA: Data transfer between master and slave.
- SCL: Generates the clock pulse to the slave. Only the Master can control the SCL line.
- VCC: Power supply for the screen, required to be 3.3V - 5V.
- GND: Ground.

**I2C Address of OLED Screen**
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1       | 0       |
|:----|:-:|:-:|:-:|:-:|:-:|:-:|:-------:|:-------:|
|     | 0 | 1 | 1 | 1 | 1 | 0 | SA0 bit | R/W bit |

- SA0 bit: Used to determine the Slave address
	+ SA0 = 0 -> address = 0b00111100 = 0x3C
	+ SA0 = 1 -> address = 0b00111101 = 0x3D
- R/W (Read/Write) bit: Determines the operation mode of the Master, where: 1 = Read, 0 = Write

## OLED Screen Structure

* The OLED screen is mapped to the GDDRAM of the SSD1306.
* GDDRAM (Graphics Display Data RAM) is a type of bit-mapped static RAM. It stores the bit pattern that will be displayed on the screen.
	* The size of GDDRAM is 128x64 bits and is divided into 8 pages (PAGE0 ... PAGE7).
	* When a byte of data is written to GDDRAM, all pixel rows of the current column in a specific PAGE are filled. With data bit D0 located at the top row, D7 located at the bottom row.
	
## Algorithm for Printing Data to the Screen

1. Step 1: Create a `bufferRAM[]` array consisting of 1024 elements (A replica of the 128x64 OLED screen). Every 128 elements in the array represent 1 PAGE.
2. Step 2: Clear the buffer before drawing.
3. Step 3: Draw pixels into the buffer instead of sending I2C immediately. Instead of sending pixels to the OLED right away, we do the following:
	- With input (x, y) where: x belongs to [0, 63] and y belongs to [0, 127].
	- Calculate the Page containing the pixel: `page = x / 8`.
	- Bit order in a Byte (of a column in a PAGE): `bit = y % 8`.
	- Determine the `bufferRAM[]` index corresponding to position (x, y): `index = (page * 128) + x`.
	- Draw a pixel at position (x, y): `bufferRAM[index] |= (1 << bit)`.
	-> As mentioned, the index essentially represents 8-bit data - 8 pixels in a column of a specific PAGE.
4. Step 4: Draw a character at position (x, y) into the buffer. Each 5x7 character requires 5 columns and 7 rows.
	- For each column, we take the 8-bit data of a column.
	- Examine each row (pixel) of the column by checking the bits in the data. If that row is active (high), we draw the pixel.
5. Step 5: Draw a string into the buffer.
6. Step 6: Update the buffer to the OLED.

## Programming SSD1306 128x64 OLED with AVR ATmega328P

* In this section, I have implemented 3 groups of functions:
	* Fundamental Functions
	* Display Functions
	* Scroll Functions

**NOTE: The following functions only apply to the 128x64 OLED and Page Addressing Mode.**

### Fundamental Functions

- `OLED_Init()`: Initialization command for SSD1306 OLED128x64.
- `OLED_Set_Cursor(uint8_t page, uint8_t col)`: Moves the cursor to a specific position with 2 parameters: page - PAGE0..7, col - column 0...127.
- `OLED_Set_Brightness(uint16_t brightness)`: Adjusts the brightness of the OLED.
- `OLED_Display_Mode(uint8_t mode)`: Selects the display mode on the OLED screen with 2 modes: 0 (TOP to BOTTOM), 1 (BOTTOM to TOP).
- `OLED_Sleep()`: Puts the OLED into Sleep mode, but the GDRAM data is not lost.
- `OLED_Clear_Display()`: Clears all content in GDRAM.
- `OLED_Draw_Bit_Map(const uint8_t* image)`: This command takes a matrix array of pixels and prints them to the OLED.

**NOTE:** TOP here refers to the text marking the pins (VCC, GND, SCK, SDA).

### Display Functions

- `OLED_Print_Character(char ch, uint8_t x, uint8_t y)`: Helper for the `OLED_Print_String()` function.
- `OLED_Print_String(const char* str, uint8_t x, uint8_t y)`: This function pushes the character string to be printed into the `bufferRAM[]` array.
- `OLED_Update_Display()`: Pushes the `bufferRAM[]` to the GDRAM screen of the OLED by mapping each memory cell of `bufferRAM` to a pixel.

**NOTE:** 
- With x (Column) in the range [0, 127] and y (Row) in the range [0, 63].
- `bufferRAM[]`: Buffer array used to store the data bytes of a specific (page, column).

### Scroll Functions

**NOTE**:
1. All scroll functions here have a fixed interval of 5 frames between two scrolls, and the default scroll direction is to the right.
2. Vertical scrolling is not possible in the SSD1306 OLED 128x64.

- `OLED_Horizontal_Scroll(uint8_t startPage, uint8_t endPage)`: Pages in the range [startPage, endPage] will be scrolled horizontally to the right.
- `OLED_Scroll_Area(uint8_t fixedRows, uint8_t scrollingRows)`: The purpose of this function is to scroll allowed rows and fix specified rows.
	- Example: fixedRows = 3, scrollingRows = 6 -> Rows 0 - 3 are fixed, rows 4 - 9 are allowed to scroll, and rows 10 - 63 are fixed.
- `OLED_Vertical_And_Horizontal_Scroll(uint8_t startPage, uint8_t endPage, uint8_t verticalOffset)`: This command performs simultaneous horizontal scrolling and vertical shifting of the image, where verticalOffset is the vertical shift amount per horizontal scroll.

## Contribution

Programmer: Nguyễn Hải Nam

Email: namnguyenhai228@gmail.com