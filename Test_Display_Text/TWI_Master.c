/*
	TWI_Master.c
 */ 
#include "TWI_Master.h"

// Initialize I2C in master mode
void I2C_Master_Init()
{
	TWSR = 0x00;									// prescaler = 1
	TWBR = TWBR_VALUE(TWSR);
	TWCR =  (1 << TWEN);							// enable TWI
}

// Start condition
uint8_t I2C_Start()
{
	uint8_t status;
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA);	// enable TWI, start condition and clear TWINT flag
	
	while (!(TWCR & (1 << TWINT)));			// wait until the start condition is complete
	status = TWSR & 0xF8;
	if (status != 0x08)						// start condition is failure
		return 0;
	return 1;
}

// Load SLA+W into TWDR register
// address: Slave address
uint8_t I2C_Master_SLA_W(uint8_t address)
{
	uint8_t status;
	TWDR = (address << 1);				// load SLA+W into TWDR register.
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));		// wait until the flag is set
	status = TWSR & 0xF8;				// read TWI status register
	if (status == 0x18)					// Check for SLA+W transmitted and ACK returned
		return 1;						// load SLA+W successfully
	else if (status == 0x20)			// Check for SLA+W transmitted and NACK returned
		return 2;
	else
		return 3;
}	

uint8_t I2C_Master_SLA_R(uint8_t address)
{
	uint8_t status;
	TWDR = (address << 1) | 1;			// load SLA+R into TWDR register.
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));		// wait until the flag is set
	status = TWSR & 0xF8;				// read TWI status register
	if (status == 0x40)					// check for SLA+R transmitted and ACK returned
		return 1;
	else if (status == 0x48)			// check for SLA+R transmitted and NACK returned
		return 2;
	else
		return 3;

	return 0;
}

// Repeated Start condition
uint8_t I2C_Repeated_Start()
{
	uint8_t status;
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA);	// enable TWI, start condition and clear TWINT flag
	
	while (!(TWCR & (1 << TWINT)));			// wait until the repeated start condition is complete
	status = TWSR & 0xF8;
	if (status != 0x10)						// start condition is failure
		return 0;
	return 1;
}				

// Transmit data to slave in Master transmitter mode
uint8_t I2C_Write(char data)
{
	uint8_t status;
	TWDR = data;						// load data into TWDR
	TWCR = (1 << TWINT) | (1 << TWEN);	// clear the TWINT flag
	while (!(TWCR & (1 << TWINT)));	
	status = TWSR & 0xF8;
	
	if (status == 0x28)					// check for data transmitted and ACK returned
		return 1;						// 1: data is sent successfully
	else if (status == 0x30)			// check for data transmitted and NACK returned	
		return 2;						// 2: don't need to transmit data to slave
	else
		return 3;						// 3: data transmission failure
}

// Receive data in Master receiver mode
uint8_t I2C_Read_ACK()
{
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);		// enable TWI, ACK pulse generation and clear the flag
	while (!(TWCR & (1 << TWINT)));	
	
	return (TWDR);			// return received data
}
	
// Receive last data byte
uint8_t I2C_Read_NACK()
{
	TWCR = (1 << TWEN) | (1 << TWINT);		// enable TWI and send NACK pulse
	while (!(TWCR & (1 << TWINT)));
	
	return (TWDR);			// return last data byte
}	

// Stop condition
void I2C_Stop()
{
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);	// enable TWI, send stop condition and clear TWINT flag
}							