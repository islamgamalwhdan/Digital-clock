/*
 * gpoi__.h
 *
 * Created: 10/2/2015 9:16:23 PM
 *  Author: ahmed
 */ 


#ifndef GPOI___H_
#define GPOI___H_


#include "types.h"


uint8 init_port(uint8 reg , uint8 val);
uint8 init_pin( pin_t *pin);
uint8 read_port( uint8 reg );
uint8 write_port( uint8 reg , uint8 val ) ;
uint8 write_pin(volatile uint8* reg , uint8 pin , uint8 val);
uint8 read_pin(pin_t *pin);
uint8 toggle_pin(pin_t *pin);

#endif /* GPOI___H_ */