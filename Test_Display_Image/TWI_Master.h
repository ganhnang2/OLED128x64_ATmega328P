/*
	TWI_Master.h

*/

#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

#include <avr/io.h>
#include <math.h>

/*---------Clock frequency--------*/	
#define F_CPU				12000000UL													// Clock CPU: 12MHz										

#define PRESCALER(value)	(pow(4, value & ((1 << TWPS0) | (1 << TWPS1))))				// Prescaler
#define F_SCL				400000UL													// Assume frequency SCL = 400kHz
#define TWBR_VALUE(value)	((F_CPU / F_SCL - 16) / (2 * PRESCALER(value)))				// Calculate value in TWBR

/*---------Marco---------*/
#ifndef cbi
#define cbi(port, bit)		(port &= ~(1 << bit))				// Clear bit
#endif

#ifndef cbi
#define sbi(port, bit)		(port |= (1 << bit))				// Set bit
#endif


/*	Prototype function	*/
void I2C_Master_Init();						// Initialize I2C in master mode
uint8_t I2C_Start();						// Start condition
uint8_t I2C_Repeated_Start();				// Repeated Start condition
uint8_t I2C_Master_SLA_W(uint8_t address);	// Load SLA+W into TWDR register
uint8_t I2C_Master_SLA_R(uint8_t address);
uint8_t I2C_Write(char data);				// Transmit data to slave in Master transmitter mode
uint8_t I2C_Read_ACK();						// Receive data in Master receiver mode
uint8_t I2C_Read_NACK();					// Receive last data byte
void I2C_Stop();							// Stop condition


#endif /* TWI_MASTER_H_ */