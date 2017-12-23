/*
 * Lcd.c
 *
 * Created: 10/5/2015 8:17:22 PM
 *  Author: ahmed
 */ 

#include "types.h"
#include "Lcd.h"
#include "config.h"
#include "gpoi__.h"

		 
/* functions definitions */

void Lcd_init(void)
{
	DDRA = 0xFF;
	write_pin(LCD_REG , LCD_E , 0) ;   // E = 0 .
	__delay(2000) ;
	Lcd_cmd(0x33) ;
	Lcd_cmd(0x32) ;
	Lcd_cmd(LCD_4BIT_DATA) ;
	Lcd_cmd(CURSOR_ON_UNDERLINE_ON) ;
	Lcd_cmd(CLEAR_DISPLAY) ;
}

void send_nipple( volatile uint8 nipple )
{
	uint8 temp_nipple ;
	
	if( nipple > 0x0F)
		temp_nipple = nipple>>4 ;
	else
	    temp_nipple = nipple ;	
	write_pin( LCD_REG , LCD_D4 , (temp_nipple & (1<<0))>>0 ) ;
	write_pin( LCD_REG , LCD_D5 , (temp_nipple & (1<<1))>>1 ) ;
	write_pin( LCD_REG , LCD_D6 , (temp_nipple & (1<<2))>>2 ) ;
	write_pin( LCD_REG , LCD_D7 , (temp_nipple & (1<<3))>>3 ) ;
	
}

void Lcd_cmd(volatile uint8 command) 
{
	
	send_nipple(command & 0xF0) ;
	write_pin( LCD_REG , LCD_RS , 0 ) ;  // RS = 0 .
	write_pin(LCD_REG , LCD_E , 1) ; // E = 1 .
	__delay(2) ;
	write_pin(LCD_REG , LCD_E , 0) ; // E = 0 .
	__delay(100) ;
	send_nipple(command & 0x0F) ;
	write_pin(LCD_REG , LCD_E , 1) ; // E = 1 .
	__delay(2) ;
	write_pin(LCD_REG , LCD_E , 0) ; // E = 0 .
	__delay(100) ;
	if( command == CLEAR_DISPLAY || command == RETURN_HOME )
	__delay(1000) ;
}

void Lcd_chr_cp(volatile uint8 ch)
{
	send_nipple(ch & 0xF0) ;
	write_pin( LCD_REG , LCD_RS , 1 ) ;  // RS = 0 .
	write_pin(LCD_REG , LCD_E , 1) ; // E = 1 .
	__delay(2) ;
	write_pin(LCD_REG , LCD_E , 0) ; // E = 0 .
	__delay(100) ;
	send_nipple(ch & 0x0F) ;
	write_pin(LCD_REG , LCD_E , 1) ; // E = 1 .
	__delay(2) ;
	write_pin(LCD_REG , LCD_E , 0) ; // E = 0 .
	__delay(100) ;
}

void Lcd_chr(volatile  uint8 row ,volatile uint8 col ,volatile uint8 ch)
{
	/*const*/ uint8 row_add[] = { CURSOR_FIRST_LINE , CURSOR_SECOND_LINE, CURSOR_THIRD_LINE , CURSOR_FOURTH_LINE} ;
	
	Lcd_cmd( row_add[row-1] + (col-2) ) ;
	Lcd_chr_cp(ch) ;
}

void Lcd_text_cp(volatile uint8 *str)
{
	while(*str)
	{
		Lcd_chr_cp(*str) ;
		++str ;
	}
}

void Lcd_text(volatile  uint8 row  ,volatile  uint8 col ,volatile   char *str)
{
	/*const*/ uint8 row_add[] = { CURSOR_FIRST_LINE , CURSOR_SECOND_LINE, CURSOR_THIRD_LINE , CURSOR_FIRST_LINE} ;
	
	Lcd_cmd( row_add[row-1] + (col-2) ) ;
	Lcd_text_cp(str);
}

static void MoveCursorRight(uint8 num)
{
	while(num--)
	   Lcd_cmd(SHIFT_CURSOR_RIGHT) ;
}

void Lcd_move_cursor_to(uint8 row , uint8 colum)
{
	uint8 Lcd_row[] = { CURSOR_FIRST_LINE , CURSOR_SECOND_LINE , CURSOR_THIRD_LINE , CURSOR_FOURTH_LINE };
		
		Lcd_cmd(Lcd_row[row-1]) ;
		MoveCursorRight(colum-1) ;
}

void __delay(volatile unsigned long int _time) 
{
	_time *= 2 ;
	while(_time>0)
	{
		_time-- ;
	}
}