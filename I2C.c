/*
 * I2C.c
 *
 * Created: 1/11/2016 11:48:30 PM
 *  Author: ahmed
 */

#include "config.h" 
#include "I2C.h"

void I2C_init(uint16 clock_rate)
{
	TWSR = 0x00 ;  // prescale = 0 .
	TWBR = (MCU_FREQ_khz/clock_rate -16)>>1 ;
	TWCR =1<<TWEN ;
}

void I2C_start(void)
{
	TWCR =(1<<TWEN) | (1<<TWSTA) | (1<<TWINT) ;
	while( ! (TWCR & (1<<TWINT)) ) ;
}

void I2C_stop(void)
{
	TWCR =(1<<TWEN) | (1<<TWSTO) | (1<<TWINT) ;
}

void I2C_write(uint8 data_s)
{
	TWDR = data_s ;
	TWCR =(1<<TWEN) | (1<<TWINT);
	while( ! (TWCR & (1<<TWINT)) ) ;
}	

uint8 I2C_Read(uint8 ack)
{
	if(ack)
	   TWCR =(1<<TWEN) | ( 1<<TWINT) ;
	else
	   TWCR =(1<<TWEN) | ( 1<<TWINT) | (1<<TWEA) ;   
	while( ! (TWCR & (1<<TWINT)) ) ;
	return TWDR ;   
}