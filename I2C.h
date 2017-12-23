/*
 * I2C.h
 *
 * Created: 1/11/2016 11:48:59 PM
 *  Author: ahmed
 */ 


#ifndef I2C_H_
#define I2C_H_

#define CLOCK_RATE_khz 10   // 10khz = 10kbps
void I2C_init(uint16);
void I2C_start(void);
void I2C_stop(void);
void I2C_write(uint8);
uint8 I2C_Read(uint8 );



#endif /* I2C_H_ */