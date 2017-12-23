/*
 * button.c
 *
 * Created: 10/2/2015 5:42:07 PM
 *  Author: islam
 */ 
#include "config.h"
#include "types.h"
#include "gpoi__.h"
#include "button.h"

#define PULL_UP 1
#define PULL_DOWN 0
void init_button( pin_t *pin)            
{
        pin->val = PIN_INPUT;
        
        init_pin(pin) ;
}

uint8 button_read( pin_t *pin )
{
        return read_pin(pin) ^ pin->state ;      
}