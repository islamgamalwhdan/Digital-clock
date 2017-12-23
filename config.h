/*
 * config.h
 *
 * Created: 10/3/2015 8:11:09 PM
 *  Author: ahmed
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#include <AVR/io.h>
#include "types.h"
#include "AVR_gpio.h"
#define MCU_FREQ_khz 1000   // 4Mhz = 4000Khz
#define PRESSED 1
#define REALSED 0
#define PULL_UP 1
#define PULL_DOWN 0
#define HIGH 1
#define LOW 0
#define PINS_MAX_NUM 7
#define F_PASS 1
#define F_FAIL -1
#define PORT_OUTPUT 0xFF
#define PORT_INPUT 0
#define PIN_OUTPUT 1
#define PIN_INPUT  0

 /* ===================================== MACROS================================*/
 
 #define bit_access(_reg,_bit,_value) \
										_reg = ((_reg) & ~ _BV(_bit)) | ( (_value)? _BV(_bit) : 0 )
#define get_bit_value(_reg,_bit) \
										( ( (_reg) & _BV(_bit) )>>(_bit) )									 	
#endif /* CONFIG_H_ */