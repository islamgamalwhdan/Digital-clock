/*
 * Gpio.c
 *
 * Created: 10/2/2015 4:14:48 PM
 *  Author: islam
 */ 
#include "config.h"
#include "types.h"
#include "gpoi__.h"


uint8 init_port(uint8 reg , uint8 val)
{
          *( (volatile uint8 *)(reg+DIR_OFFSET) ) = val ;
          
           return F_PASS ;   
}

uint8 init_pin( pin_t *pin)
{

        if( pin->pin_num > PINS_MAX_NUM || (pin->val > 1) )
           return F_FAIL ;
           
        if( pin->val == 1)
           *( (volatile uint8 *)(pin->reg+DIR_OFFSET) ) |=1<<pin->pin_num ;
        else
           *( (volatile uint8 *)(pin->reg+DIR_OFFSET) ) &=~(1<<pin->pin_num) ; 
           
         return F_PASS ;     
}

uint8 write_port( uint8 reg , uint8 val )
{
        *( (volatile uint8 *)(reg+OUTPUT_OFFSET) ) = val ;
        
        return F_PASS ;
}

uint8 read_port( uint8 reg  )
{        
        return *( (volatile uint8 *)(reg+INPUT_OFFSET) ) ;
}

uint8 write_pin(volatile uint8* reg , uint8 pin , uint8 val)
{
        if( pin > PINS_MAX_NUM || (val > 1) )
            return F_FAIL ;
           
        if( val == 1)
            *(reg) |=1<<pin ;
        else
            *(reg) &=~(1<<pin) ; 
                 
        return F_PASS ;
}

uint8 read_pin(pin_t *pin)
{
        uint8 pin_rd ;
        if( pin->pin_num > PINS_MAX_NUM  )
            return F_FAIL ;
        
        pin_rd = *( (volatile uint8 *)(pin->reg+INPUT_OFFSET) ) & (1<<(pin->pin_num)) ;
        
        return pin_rd>=1  ; 
}

uint8 toggle_pin(pin_t *pin)
{
        if( pin->pin_num > PINS_MAX_NUM)
           return F_FAIL ;
        
        *( (volatile uint8 *) (pin->reg+OUTPUT_OFFSET)) ^= (1<<pin->pin_num) ; // hint !!   
        
        return F_PASS ;
}