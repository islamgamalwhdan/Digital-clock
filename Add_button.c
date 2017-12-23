/*
 * Add_button.c
 *
 * Created: 1/19/2016 7:37:16 PM
 *  Author: ahmed
 */ 

#include "config.h"
#include "I2C.h"
#include "DS1307_RTC.h"
#include "Lcd.h"
#include "Add_button.h"


    /* Macros */
#define BCD2DEC(BCD) ( ((BCD)&0x0F ) + ((BCD) >> 4 )*10 )
#define DEC2BCD(DEC) (  (((DEC)/10)<<4) | ((DEC)%10) )	
#define BCD2LwrCHR(BCD) ( ((BCD) & 0x0F) +'0' )
#define BCD2UprCHR(BCD) ( ((BCD)>>4) + '0' ) 
#define SET_BIT( REG_ , PIN_  )  ((REG_) |= (1<<(PIN_)))
#define CLEAR_BIT( REG_ , PIN_  )  ((REG_) &= ~(1<<(PIN_)))
////////////////////////////////

/* Constants */

#define AM 0
#define PM 1

/////////////////////////////

static void Hour_24H_Edit(Time_st* rtc_time)    /* 1 , 2 , 3 , 4 ,...,12 ,13 ,14 ,15 ,..,23 , 00 */
{
	rtc_time->hour = BCD2DEC(rtc_time->hour) + 1 ;
	rtc_time->hour = (rtc_time->hour == 24)? 0 : DEC2BCD(rtc_time->hour) ; 
	Rtc_Write(HOUR_ADDRESS , rtc_time->hour) ;  // Save the new modified value to DS1307 chip memory at hour address .
	Lcd_FieldWrite(1 , UPR_HR_LCD , rtc_time->hour ) ; // Display the new modified value at it's field on LCD display . 
}

static void Hour_12H_Edit(Time_st* rtc_time) /* 1 am, 2 am , 3 am ,  , .... , 11 am, 12 PM , 1 Pm , 2 Pm , ..., 10Pm , 11Pm , 12AM */  
{	
	uint8 _AmPm_bit = get_bit_value(rtc_time->hour , AM_PM_BIT) ;
	
	rtc_time->hour = DEC2BCD( BCD2DEC(rtc_time->hour) + 1 ) ;
	if( ( (rtc_time->hour) &~ ( (1<<MODE_24H12H_BIT) | (1<<AM_PM_BIT) ) ) == 0x13 )
	{
		rtc_time->hour = 0x01 ;    
	}		 
	SET_BIT(rtc_time->hour , MODE_24H12H_BIT) ;
	bit_access(rtc_time->hour , AM_PM_BIT , _AmPm_bit ) ; 
	Rtc_Write(HOUR_ADDRESS , rtc_time->hour) ;  // Save the new modified value to DS1307 chip memory at hour address .
	Lcd_FieldWrite(1 , UPR_HR_LCD , rtc_time->hour & 0x1F ) ; // make mask with 0x1F to display the actual hour value without bit6 and bit5 .
}

static void Mint_Edit(Time_st* rtc_time)
{
   rtc_time->mint = BCD2DEC(rtc_time->mint) + 1 ;
   rtc_time->mint = (rtc_time->mint == 60)? 0 : DEC2BCD(rtc_time->mint) ;
   Rtc_Write(MINT_ADDRESS , rtc_time->mint) ; // Save the new modified value to DS1307 chip memory at hour address . 
   Lcd_FieldWrite(1 , UPR_MNT_LCD , rtc_time->mint ) ; // Display the new modified value at it's field on LCD display .	
}

static void Day_Edit(Time_st* rtc_time)
{
   	rtc_time->day = BCD2DEC(rtc_time->day) + 1 ;
	rtc_time->day = (rtc_time->day ==32)? 1 : DEC2BCD(rtc_time->day) ; // Max number of days per month = 31 .
	Rtc_Write(DAY_ADDRESS , rtc_time->day) ; // Save the new modified value to DS1307 chip memory at hour address . 
   Lcd_FieldWrite(2 , UPR_DY_LCD , rtc_time->day ) ; // Display the new modified value at it's field on LCD display .   
}	

static void Month_Edit(Time_st* rtc_time)
{
   	rtc_time->month = BCD2DEC(rtc_time->month) + 1 ;
	rtc_time->month = (rtc_time->month==13)? 1 : DEC2BCD(rtc_time->month) ; 
	Rtc_Write(MONTH_ADDRESS , rtc_time->month) ; // Save the new modified value to DS1307 chip memory at hour address . 
    Lcd_FieldWrite(2 , UPR_MO_LCD , rtc_time->month ) ; // Display the new modified value at it's field on LCD display . 
}

static void Year_Edit(Time_st* rtc_time)
{
    rtc_time->year= BCD2DEC(rtc_time->year) + 1 ;
	rtc_time->year = (rtc_time->year ==100)? 1 : DEC2BCD(rtc_time->year) ; // Max year is 2099 .
	Rtc_Write(YEAR_ADDRESS , rtc_time->year) ; // Save the new modified value to DS1307 chip memory at hour address . 
    Lcd_FieldWrite(2 , UPR_YR_LCD , rtc_time->year ) ; // Display the new modified value at it's field on LCD display .   	
}

static void DayName_Edit(Time_st* rtc_time)
{
	/*const*/ char* day_name[] = {"Su" , "Mo" , "Tu" , "We" , "Th" , "Fr" , "Sa"} ;
		
   	rtc_time->day_name = BCD2DEC(rtc_time->day_name) + 1 ;
	rtc_time->day_name = (rtc_time->day_name ==8)? 1 : DEC2BCD(rtc_time->day_name) ; // Max number of days per month = 31 .
	Rtc_Write(DAY_NAME_ADDRESS , rtc_time->day_name) ; // Save the new modified value to DS1307 chip memory at hour address . 
    Lcd_text(2 , UPR_DYN_LCD, day_name[rtc_time->day_name -1]) ;
	Lcd_cmd(SHIFT_CURSOR_LEFT) ;
}

static void AmPm_Edit(Time_st* rtc_time)
{
	if(Am_Pm == AM)
	{
	   SET_BIT(rtc_time->hour , AM_PM_BIT) ; // Make it as PM .
	   Lcd_text(1 , UPR_DYN_LCD , "PM") ;
	   Lcd_cmd(SHIFT_CURSOR_LEFT) ;
	   Lcd_cmd(SHIFT_CURSOR_LEFT) ;
	   Am_Pm = PM ;
	}	   
	else
	{
	  CLEAR_BIT(rtc_time->hour , AM_PM_BIT) ; // make it as AM.
	  Lcd_text(1 , UPR_DYN_LCD , "AM") ;  
	  Lcd_cmd(SHIFT_CURSOR_LEFT) ;
	  Lcd_cmd(SHIFT_CURSOR_LEFT) ; 
	  Am_Pm = AM ;
	}	
	Rtc_Write(HOUR_ADDRESS , rtc_time->hour) ;
}

 /*Alarm Adjsutment */
 
 static void Hour_Alrm_24H(Time_st* rtc_time)
 {
	rtc_time->alarm_hr = BCD2DEC(rtc_time->alarm_hr) + 1 ;
	rtc_time->alarm_hr = (rtc_time->alarm_hr == 24)? 0 : DEC2BCD(rtc_time->alarm_hr) ; 
	Lcd_FieldWrite(2 , UPR_HR_LCD , rtc_time->alarm_hr ) ; // Display the new modified value at it's field on LCD display . 
 }
 
 static void Hour_Alrm_12H(Time_st* rtc_time) /* there is a bug in that function */ // i will add am_pm_alrm and am_pm_time to rtc_time struct .
 {
	
	rtc_time->alarm_hr = DEC2BCD( BCD2DEC(rtc_time->alarm_hr) + 1 ) ;
	if( ( (rtc_time->alarm_hr) &~ ( (1<<MODE_24H12H_BIT) | (1<<AM_PM_BIT) ) ) == 0x13 )
	{
	      rtc_time->alarm_hr = 0x01 ;
	}
	SET_BIT(rtc_time->alarm_hr , MODE_24H12H_BIT);  // set bit6 by 1 because it is in mode 12h .
	bit_access(rtc_time->alarm_hr , AM_PM_BIT , rtc_time->am_pm_alrm ) ; 		  
	
	Lcd_FieldWrite(2 , UPR_HR_LCD , rtc_time->alarm_hr & 0x1F ) ; // make mask with 0x1F to display the actual hour value without bit6 and bit5 .
 }
 
 static void AmPm_Alrm_Edit(Time_st * rtc_time)
 {
	 
	 if(rtc_time->am_pm_alrm == AM)
	{
	   SET_BIT(rtc_time->alarm_hr , AM_PM_BIT) ; // Make it as PM .
	   Lcd_text(2 , 14 , "PM") ;
	   Lcd_cmd(SHIFT_CURSOR_LEFT) ;
	   Lcd_cmd(SHIFT_CURSOR_LEFT) ;
	   rtc_time->am_pm_alrm = PM ;
	}	   
	else if(rtc_time->am_pm_alrm == PM)
	{
	  CLEAR_BIT(rtc_time->alarm_hr , AM_PM_BIT) ; // make it as AM.
	  Lcd_text(2 , 14 , "AM") ;  
	  Lcd_cmd(SHIFT_CURSOR_LEFT) ;
	  Lcd_cmd(SHIFT_CURSOR_LEFT) ; 
	  rtc_time->am_pm_alrm = AM ;
	}
	SET_BIT(rtc_time->alarm_hr , MODE_24H12H_BIT) ;	
 }
 
 static void Mint_Alrm_Edit(Time_st* rtc_time)
 {
   rtc_time->alarm_mnt = BCD2DEC(rtc_time->alarm_mnt) + 1 ;
   rtc_time->alarm_mnt = (rtc_time->alarm_mnt == 60)? 0 : DEC2BCD(rtc_time->alarm_mnt) ;
   Lcd_FieldWrite(2, UPR_MNT_LCD , rtc_time->alarm_mnt ) ; // Display the new modified value at it's field on LCD display .
 }

void Rtc_Write( uint8 address , uint8 data)
{
	I2C_start();
	I2C_write(RTC_ADDRESS_WR) ;
	I2C_write(address) ;
	I2C_write(data) ;
	I2C_stop();
} 

void Lcd_FieldWrite( unsigned char row, unsigned char colum, unsigned char value)
{
  Lcd_cmd(CURSOR_OFF_UNDERLINE_ON) ;	
  Lcd_chr( row , colum , BCD2UprCHR(value) )  ;
  Lcd_chr_cp( BCD2LwrCHR(value) ) ;
  Lcd_cmd(SHIFT_CURSOR_LEFT) ;        /*HW LCD BUG*/ 
}

void Add_Button_action(Time_st* rtc_time , Buttons_st* button)
{
	if(TimeDate_Adjust_24H)
	{
		void (*T_D_24H_Adjust[])(Time_st*) = { Hour_24H_Edit , Mint_Edit , Day_Edit , Month_Edit , Year_Edit , DayName_Edit } ;
			
			T_D_24H_Adjust[button->menu->No_Offpress](rtc_time) ;
	}
	else if(TimeDate_Adjust_12H)
	{
		 void (*T_D_12H_Adjust[])(Time_st*) = {Hour_12H_Edit , Mint_Edit , AmPm_Edit , Day_Edit , Month_Edit , Year_Edit , DayName_Edit} ;
			
			 T_D_12H_Adjust[button->menu->No_Offpress](rtc_time) ;
	}
	else if(Alarm_Adjust_24H)
	{
		void (*Alarm_adjust[])(Time_st *) = { Hour_Alrm_24H , Mint_Alrm_Edit  } ;
			 Alarm_adjust[button->menu->No_Offpress](rtc_time) ;
			 Alarm_Checking = 1 ; 
	}
	else if(Alarm_Adjust_12H)
	{ 
		void (*Alarm_adjust[])(Time_st *) = { Hour_Alrm_12H , Mint_Alrm_Edit , AmPm_Alrm_Edit } ;
			 Alarm_adjust[button->menu->No_Offpress](rtc_time) ;
			 Alarm_Checking = 1 ;
	}
	
}



