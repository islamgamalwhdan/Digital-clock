/*
 * Lcd.h
 *
 * Created: 10/5/2015 8:17:53 PM
 *  Author: ahmed
 */ 


#ifndef LCD_H_
#define LCD_H_

#include "types.h"
//#include <util/delay.h>
#define LCD_REG (volatile uint8 *)(&PORTA)
#define LCD_D4  4
#define LCD_D5  3
#define LCD_D6  2
#define LCD_D7  1
#define LCD_RS  7 
#define LCD_RW 
#define LCD_E   6

#define CLEAR_DISPLAY 0x01
#define RETURN_HOME 0x02
#define SHIFT_CURSOR_RIGHT 0x14
#define SHIFT_CURSOR_LEFT 0x10
#define CURSOR_OFF_UNDERLINE_OFF 0x0C
#define CURSOR_ON_UNDERLINE_OFF  0x0D
#define CURSOR_OFF_UNDERLINE_ON  0x0E
#define CURSOR_ON_UNDERLINE_ON   0x0F
#define SHIFT_DISPLAY_LEFT 0x18
#define SHIFT_DISPLAY_RIGHT 0x1C
#define CURSOR_FIRST_LINE 0x81
#define CURSOR_SECOND_LINE 0xC1
#define CURSOR_THIRD_LINE 0x94
#define CURSOR_FOURTH_LINE 0xD4
#define LCD_4BIT_DATA 0x28
#define LCD_8BIT_DATA 0x38

/* Project dependant constant */
#define UPR_HR_LCD 5
#define UPR_MNT_LCD 8
#define UPR_SC_LCD 11
#define UPR_DY_LCD 4
#define UPR_MO_LCD 7
#define UPR_YR_LCD 12
#define UPR_DYN_LCD 15
             /* Macros */
			 
#define delay_ms(X) _delay_ms(X)
#define delay_us(X) _delay_us(X)	
//#define SET_BIT(REG__,BIT) ( (REG__) |= (1<<(BIT)) )
//#define CLEAR_BIT(REG__,BIT) ( (REG__) &= ~(1<<(BIT)) )
/* Functions prototypes */
void bit_access_a( uint8 reg , uint8 bit , uint8 value_) ;
void Lcd_init(void);
void Lcd_cmd( const uint8) ;
void Lcd_chr_cp( volatile uint8) ;
void Lcd_chr(  volatile uint8 , volatile uint8 ,volatile uint8);
void Lcd_text_cp( volatile uint8 *);
void Lcd_text(  volatile uint8  ,volatile  uint8 ,volatile  char *);
void Lcd_move_cursor_to(uint8  , uint8 );
void send_nipple( uint8 nipple ) ;
void Lcd_move_cursor_to(uint8 row , uint8 colum) ;
void delay__us(volatile uint16 time__);
void __delay(volatile unsigned long int _time) ;
#endif /* LCD_H_ */