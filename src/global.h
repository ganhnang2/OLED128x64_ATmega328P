/*
	global.h
	This function defines the variable which will be used throughout the project
	NOTE: Mustn't modify this file
	
	Source: Nguyen Hai Nam
 */ 


#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <avr/io.h>
#include <avr/pgmspace.h>

extern const uint8_t font_data_5x7[] PROGMEM;
extern uint8_t bufferRAM[1024];				

#endif /* GLOBAL_H_ */