/*
 * digital_clock.c
 *
 * Created: 1/9/2016 10:44:23 PM
 *  Author: ahmed
 
 * Line: 1194 line
 */ 
       /*Constants */
 
#define F_CPU 1000000UL
#define MAX_MENU_PRESS 5
#define TIME_DATE 1
#define _12HR_24HR 2
#define ALARM 3
#define DISPLAY_OFF 4
#define AM 0
#define PM 1
#define ALARM_PIN 3
#define DISPALY_PIN 4
#define ON 1 
#define OFF 0

     /* Includes */
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/io.h> 

#include "config.h"
#include "lcd.h"
#include "button.h"
#include "DS1307_RTC.h"
#include "Add_button.h"
#include "digital_clock.h"
	
         /* Macros */
#define SET_BIT( REG_ , PIN_  )  ((REG_) |= (1<<(PIN_)))
#define CLEAR_BIT( REG_ , PIN_  )  ((REG_) &= ~(1<<(PIN_)))
#define BCD2LwrCHR(BCD) ( ((BCD) & 0x0F) +'0' )
#define BCD2UprCHR(BCD) ( ((BCD)>>4) + '0' )
#define BCD2DEC(BCD) ( ((BCD)&0x0F ) + ((BCD) >> 4 )*10 )
#define DEC2BCD(DEC) (  (((DEC)/10)<<4) | ((DEC)%10) )	
#define LCD_FIRST_DISPLAY  {Lcd_text(1,5,"00:00:00");Lcd_text(2,4,"00/00/2000");}	
      /*Types*/
	  
#define MAX_NUM 10000UL
		  
     /* Functions prototype */
void Menu_press_1(void) ;
void Menu_press_2(void) ;
void Menu_press_3(void) ;
void Menu_press_4(void) ;
void Menu_press_5(void) ;
void Get_Real_Time(Time_st *) ;
void Display_time(Time_st *) ;
void Alarm_check(Time_st *) ;
void Menu_Button_action( Buttons_st*);
void Ok_Button_action( Time_st * , Buttons_st* ) ;
void MoveCursorRight(uint8) ;
void init_timer0();	

////////////////////////////////////////////////////////////////////////
   /*Global variables */
   
uint8 in_MainMenu_control = 0 ;
uint8 TimeDate_Adjust = 0 ;
uint8 _12HR_24HR_Adjust =0 ;
uint8 Alarm_Adjust =0 ;
uint8 DisplayOff_Adjust =0 ;
uint8 TimeDate_Adjust_24H = 0 ;
uint8 TimeDate_Adjust_12H = 0 ;
uint8 Alarm_Adjust_24H = 0 ;
uint8 Alarm_Adjust_12H =0 ; 
uint8 mode_24H ; 
uint8 Am_Pm ;
uint8 Just_OutOF_Control = 0 ;
uint8 Alarm_status = 0 ;
uint8 Alarm_Checking = 0 ;
////////////////////////////////////////////////////////////////////////
int main(void)
{
	volatile pin_t menu_button = { BASE_B , 0 , PIN_INPUT , PULL_UP , 0 } ;
	volatile pin_t ok_button =   { BASE_B , 1 , PIN_INPUT , PULL_UP , 0 } ;
	volatile pin_t add_button =  { BASE_B , 2 , PIN_INPUT , PULL_UP , 0 } ;
    volatile Time_st rtc_time = {0}  ;		
	volatile Buttons_st Buttons_states = {&menu_button , &ok_button , &add_button} ;	
	
	
	long int num = MAX_NUM * 4069 ;
	PORTB  = &num ; 
	init_button(&menu_button);
	init_button(&ok_button) ;
	init_button(&add_button) ;
	write_port(BASE_B , 0x07) ; // make menu , ok and add buttons connected to internal pull-up resistors .
	
	CLEAR_BIT(PORTD , DISPALY_PIN) ; // make it on .
	Lcd_init();
	Lcd_cmd(CURSOR_OFF_UNDERLINE_OFF);
	Lcd_cmd(CURSOR_SECOND_LINE) ;
	LCD_FIRST_DISPLAY ;
	
	I2C_init(10) ; // 10kbps
	
	init_timer0() ;
	TIMSK |=1<<TOIE0 ;  // Enable timer0 overflow interrupts .
	sei();   // Enable global interrupts .
	while(1)
	{
		Get_Real_Time(&rtc_time) ;
		Display_time(&rtc_time) ;
	    Alarm_check(&rtc_time) ; 
		if(button_read(&menu_button) ) 
	  { 
		  in_MainMenu_control = 1;		
	      menu_button.No_Offpress = 0 ;
		  while(in_MainMenu_control || TimeDate_Adjust || Alarm_Adjust)
		  {
			  Menu_Button_action(&Buttons_states) ;
			  if(button_read(&ok_button))
			  {
				 __delay(400);  
			     Ok_Button_action(&rtc_time , &Buttons_states) ;
				 while(button_read(&ok_button)) ;
			  }	
			   if(button_read(&add_button))
			  {	
				  __delay(400) ;
				  Add_Button_action(&rtc_time , &Buttons_states);
				  while(button_read(&add_button)) ; 		 
			  }				  
		  }					  
	  }	
	  
		    set_sleep_mode(SLEEP_MODE_IDLE) ;
		    sleep_mode() ; 	
	}	
	
	return 0 ;
}

void Menu_press_1(void)
{
	    Lcd_cmd(CLEAR_DISPLAY) ;  
		Lcd_text(1,2,"Time/Date");
		Lcd_text(2,2,"12H/24H");
		Lcd_cmd(CURSOR_FIRST_LINE);
		Lcd_cmd(CURSOR_ON_UNDERLINE_OFF) ;
}

void Menu_press_2(void)
{
		//Lcd_text(1,5,"islam") ;
		//Lcd_cmd(CURSOR__LINE) ;
	    Lcd_cmd(CURSOR_SECOND_LINE) ;
}

void Menu_press_3(void)
{
		Lcd_cmd(CLEAR_DISPLAY) ;
		Lcd_cmd(CURSOR_OFF_UNDERLINE_OFF);	
        Lcd_text(1,2,"12H/24H");	
	    Lcd_text(2,2,"Alarm");
		Lcd_cmd(CURSOR_SECOND_LINE);
		Lcd_cmd(CURSOR_ON_UNDERLINE_OFF) ;
}

void Menu_press_4(void)
{
	 Lcd_cmd(CLEAR_DISPLAY) ;
	 Lcd_cmd(CURSOR_OFF_UNDERLINE_OFF);
	 Lcd_text(1,2,"Alarm");
	 Lcd_text(2,2,"Display off");
	 Lcd_cmd(CURSOR_SECOND_LINE) ;
	 Lcd_cmd(CURSOR_ON_UNDERLINE_OFF) ;
}


void TimeDate_MNT_Move(void)
{
	Lcd_move_cursor_to(1,8) ;
}

void TimeDate_AmPm_Move(void)
{
	Lcd_move_cursor_to(1,14) ;
}

void TimeDate_DY_Move(void)
{
	Lcd_move_cursor_to(2,4) ;
}


void TimeDate_MO_Move(void)
{
	Lcd_move_cursor_to(2,7) ;
}

void TimeDate_YR_Move(void)
{
	Lcd_move_cursor_to(2,12) ;
}

void TimeDate_DYN_Move(void)
{
	Lcd_move_cursor_to(2,14);
}


void Alarm_MNT_Move(void)
{
	Lcd_move_cursor_to(2,UPR_MNT_LCD) ;
}

void Alarm_AM_PM_Move(void)
{
	Lcd_move_cursor_to(2,15) ;
}

void Out_OfControl(void)
{
	      Lcd_cmd(CLEAR_DISPLAY) ;
		  Lcd_cmd(CURSOR_OFF_UNDERLINE_OFF) ;
		  LCD_FIRST_DISPLAY ;
		  in_MainMenu_control = 0 ;
		  Alarm_Adjust  = 0 ;
		  Alarm_Adjust_24H = 0 ;
		  Alarm_Adjust_12H = 0 ;
		  TimeDate_Adjust =0 ;
		  TimeDate_Adjust_24H = 0 ;
		  TimeDate_Adjust_12H =0 ; 
		  Just_OutOF_Control = 1; 
}


void Get_Real_Time(Time_st *rtc_time)
{
	static uint8 FirstTime =1 ;
	
	I2C_start() ;
	
	I2C_write(RTC_ADDRESS_WR) ;
	I2C_write(SEC_ADDRESS) ;
	I2C_start() ;   // Repeated start .
	I2C_write(RTC_ADDRESS_RD) ;
	if(FirstTime || rtc_time->sec == 0x59 || !in_MainMenu_control ) 
	{	
					
	     rtc_time->sec = I2C_Read(0) ;
		 rtc_time->mint = I2C_Read(0) ;
	     rtc_time->hour = I2C_Read(0) ;
	     rtc_time->day_name = I2C_Read(0) ;
	     rtc_time->day = I2C_Read(0) ;
	     rtc_time->month = I2C_Read(0) ;
	     rtc_time->year = I2C_Read(1) ;  // 1 mean not ack (no receive more) .
		 
		 FirstTime = 0 ;
	}		 
	else
	     rtc_time->sec = I2C_Read(1) ;	// get only second reading. 
		 if(rtc_time->sec >0x59)
		 {
			 Rtc_Write(SEC_ADDRESS , 0x01) ;
		 }	
	I2C_stop();
}

void Display_time(Time_st *rtc_time)
{
	static uint8 FirstTime = 1 ;
	static uint8 prev_day  ;
	uint8 HourToDisplay = rtc_time->hour ;
	
	if(FirstTime ||  rtc_time->sec == 0x00 || prev_day!=rtc_time->day || Just_OutOF_Control) 
	{
		/*const*/ char* day_name[] = {"Su" , "Mo" , "Tu" , "We" , "Th" , "Fr" , "Sa"} ;
		
		/* Check 24H mode or 12Hmode */
		
		mode_24H = !((rtc_time->hour & (1<<MODE_24H12H_BIT)) >> MODE_24H12H_BIT) ; // Mode24h is selected when bit6 of hour address of DS1307 is 0 otherwise mode12h is selected .
		if(!mode_24H)
		{
			/*Get am_pm bit value to know it's AM or PM */ 
			Am_Pm = (rtc_time->hour & (1<<AM_PM_BIT)) >>AM_PM_BIT ;
			HourToDisplay = rtc_time->hour & 0x1F ;   // Shown only the value of hours without 24H_mode bit and AmPm_mode bit .
			if(Am_Pm == AM)
			   Lcd_text(1 , UPR_DYN_LCD , "AM") ;
			else
			   Lcd_text(1 , UPR_DYN_LCD , "PM") ;    
		} 
		
		Lcd_chr(1 , UPR_HR_LCD , BCD2UprCHR(HourToDisplay)) ;
		Lcd_chr_cp(BCD2LwrCHR(rtc_time->hour)) ;
	    Lcd_chr(1 , UPR_MNT_LCD , BCD2UprCHR(rtc_time->mint)) ;
		Lcd_chr_cp(BCD2LwrCHR(rtc_time->mint)) ;
		Lcd_chr(1 , UPR_SC_LCD , BCD2UprCHR(rtc_time->sec)) ;
		Lcd_chr_cp(BCD2LwrCHR(rtc_time->sec)) ;
		Lcd_chr(2, UPR_DY_LCD , BCD2UprCHR(rtc_time->day) ) ;
		Lcd_chr_cp(BCD2LwrCHR(rtc_time->day)) ;
		Lcd_chr(2 , UPR_MO_LCD , BCD2UprCHR(rtc_time->month)) ;
		Lcd_chr_cp(BCD2LwrCHR(rtc_time->month)) ;
		Lcd_chr(2 , UPR_YR_LCD , BCD2UprCHR(rtc_time->year)) ;
		Lcd_chr_cp(BCD2LwrCHR(rtc_time->year)) ;
		Lcd_text(2 , UPR_DYN_LCD, day_name[rtc_time->day_name -1]) ;	
		FirstTime =0 ;
		Just_OutOF_Control = 0 ;
	}		
	else
	{
		Lcd_chr(1 , UPR_SC_LCD , BCD2UprCHR(rtc_time->sec)) ;
		Lcd_chr_cp(BCD2LwrCHR(rtc_time->sec)) ;
	}
	prev_day = rtc_time->day ;
}

void Alarm_check(Time_st *rtc_time)
{
	
	if( rtc_time->hour == rtc_time->alarm_hr  &&  rtc_time->mint == rtc_time->alarm_mnt && Alarm_Checking  )
	{
		SET_BIT(PORTD , ALARM_PIN) ;
		Alarm_status = ON ;
	}		
} 
void Menu_Button_action(Buttons_st* Button)
{ 		
          if(button_read(Button->menu))
	        {
				__delay(400) ;
				//Rtc_Write(SEC_ADDRESS , 0x00) ;
				if(Alarm_status == ON)
				  {
					  Alarm_status = Alarm_Checking = 0 ;
					  CLEAR_BIT(PORTD , ALARM_PIN) ;
					  Lcd_chr(2 , 1 , ' ');
					  Out_OfControl();
				  }				  
				if(in_MainMenu_control)
				{
					void (*main_menu_pfun[])(void) = { Menu_press_1 , Menu_press_2 , Menu_press_3 , Menu_press_4 , Out_OfControl } ;  // pointer-to-functions .
						
		            main_menu_pfun[Button->menu->No_Offpress++]() ;	
				}				   
				else if(TimeDate_Adjust)
				{
			       if(TimeDate_Adjust_24H)
				   {
					   void (*TimeDateAdjust_24H_pfun[])(void) = {TimeDate_MNT_Move , TimeDate_DY_Move , TimeDate_MO_Move , TimeDate_YR_Move , TimeDate_DYN_Move , Out_OfControl};
						   
					   TimeDateAdjust_24H_pfun[Button->menu->No_Offpress]() ;
				       Button->menu->No_Offpress++; 
				   }	
				   else if(TimeDate_Adjust_12H)
				   {
					   void (*TimeDateAdjust_12H_pfun[])(void) = {TimeDate_MNT_Move ,TimeDate_AmPm_Move, TimeDate_DY_Move , TimeDate_MO_Move , TimeDate_YR_Move , TimeDate_DYN_Move , Out_OfControl};
						
					   TimeDateAdjust_12H_pfun[Button->menu->No_Offpress]() ;
					   Button->menu->No_Offpress ++ ;      
				   } 
				   
				}
				else if( Alarm_Adjust)
				{
					if(Alarm_Adjust_24H)
					{
						void (*Alarm_Adjust_24H_pfun[])(void) = {  Alarm_MNT_Move  , Out_OfControl } ;
							
							Alarm_Adjust_24H_pfun[Button->menu->No_Offpress]();
							Button->menu->No_Offpress++ ;
					}
					else
					{
						void (*Alarm_Adjust_12H_pfun[])(void) = {  Alarm_MNT_Move  ,Alarm_AM_PM_Move, Out_OfControl } ;
							
							Alarm_Adjust_12H_pfun[Button->menu->No_Offpress]();
							Button->menu->No_Offpress++ ;
					}
				}									         
				while(button_read(Button->menu)) ;
			}
				  					
}

void TimeDateOk(Time_st* rtc_time , Buttons_st *button)
{
	Lcd_cmd(CLEAR_DISPLAY) ;
	LCD_FIRST_DISPLAY ;
	Just_OutOF_Control = 1 ;
	Get_Real_Time(rtc_time) ;
	Display_time(rtc_time) ;
	button->menu->No_Offpress = 0 ;
	Lcd_cmd(CURSOR_OFF_UNDERLINE_OFF);
	Lcd_move_cursor_to(1,5) ;
	Lcd_cmd(CURSOR_ON_UNDERLINE_OFF) ;
	TimeDate_Adjust = 1 ;
	mode_24H = !((rtc_time->hour & (1<<MODE_24H12H_BIT)) >> MODE_24H12H_BIT) ; // Mode24h is selected when bit6 of hour address of DS1307 is 0 otherwise mode12h is selected .
	TimeDate_Adjust_24H = mode_24H ;
	TimeDate_Adjust_12H = !mode_24H ;
}
void _12HR_24HR_OK(Time_st* rtc_time, Buttons_st *button)
{
	Lcd_cmd(CLEAR_DISPLAY) ;
	if(mode_24H)
	   Mode24H_To_Mode12H (rtc_time) ;
	else
	   Mode12H_To_Mode24H (rtc_time) ; 
	   
	 Rtc_Write(HOUR_ADDRESS , rtc_time->hour) ;
	 mode_24H ^=1;    
	 Out_OfControl() ;
}

void Mode24H_To_Mode12H(Time_st* rtc_time)
{	
	rtc_time->hour = BCD2DEC(rtc_time->hour) ;
	Am_Pm =  rtc_time->hour > 11 ; // if hour >11 bit5 must be 1 on 24H mode . 
	rtc_time->hour = (rtc_time->hour > 12)? rtc_time->hour - 12 : rtc_time->hour ;
	if(!rtc_time->hour)
	   rtc_time->hour = 12 ;
	rtc_time->hour = DEC2BCD(rtc_time->hour) | (1<<MODE_24H12H_BIT) ;
    rtc_time->hour = (Am_Pm)? rtc_time->hour| (1<<AM_PM_BIT) : rtc_time->hour &~(1<<AM_PM_BIT) ; 
}

void Mode12H_To_Mode24H(Time_st* rtc_time)
{
   Am_Pm = (rtc_time->hour & (1<<AM_PM_BIT)) >> AM_PM_BIT ;  // AM or PM?
   rtc_time->hour = BCD2DEC(rtc_time->hour & 0x1F)  ; // Clear bit 5 & bit 6 .
   
   if( Am_Pm == PM && rtc_time->hour< 12) // PM and hour not equal 12 pm   [1PM to 11 PM]
       rtc_time->hour +=12 ;
   else if( Am_Pm == AM && rtc_time->hour == 12) // 12 AM ?
       rtc_time->hour = 0 ;
	   
	rtc_time->hour = DEC2BCD(rtc_time->hour) ;      
}
void AlarmOk(Time_st* rtc_time,Buttons_st *button)
{
	char* Alarm_time = "00:00" ;
	
	mode_24H = !get_bit_value(rtc_time->hour , MODE_24H12H_BIT) ; //mode24 = 1 if bit6 = 0 and vice versa .
	rtc_time->am_pm_alrm =  get_bit_value(rtc_time->alarm_hr , AM_PM_BIT);
	
	button->menu->No_Offpress = 0 ;
	Lcd_cmd(CLEAR_DISPLAY) ;
	Alarm_time[0] = BCD2UprCHR(rtc_time->alarm_hr & 0x1F) ;
	Alarm_time[1] = BCD2LwrCHR(rtc_time->alarm_hr & 0x1F) ;
	Alarm_time[3] = BCD2UprCHR(rtc_time->alarm_mnt) ;
	Alarm_time[4] = BCD2LwrCHR(rtc_time->alarm_mnt);
	Lcd_text(2 , UPR_HR_LCD , Alarm_time) ;
	if(!mode_24H)
	  (rtc_time->am_pm_alrm )? Lcd_text(2,14,"PM") : Lcd_text(2,14,"AM") ;
	Lcd_move_cursor_to(2,UPR_HR_LCD);
	Alarm_Adjust = 1;  
	Alarm_Adjust_24H = mode_24H ;
	Alarm_Adjust_12H = !mode_24H ;
}

void Dispaly_Off_Ok(Time_st* rtc_time, Buttons_st *button)
{
  SET_BIT( PORTD , DISPALY_PIN) ;
}


void Ok_Button_action(Time_st *rtc_time , Buttons_st * button )
{
	
	if(in_MainMenu_control)
	{
		in_MainMenu_control =  0 ;
		void (*Ok_MainMenu_pfun[])(Time_st *,Buttons_st *) = {TimeDateOk , _12HR_24HR_OK , AlarmOk , Dispaly_Off_Ok } ;
	    Ok_MainMenu_pfun[button->menu->No_Offpress-1](rtc_time,button) ;
	}
	else if(TimeDate_Adjust)
	{
	    Out_OfControl() ;
	}		
	else if(Alarm_Adjust)
	{
		Out_OfControl();
		if(Alarm_Checking)
		    Lcd_chr( 2 , 1 ,0xFC ) ;
	}			  	
}

ISR(TIMER0_OVF_vect)
{
	//PORTC = ~PORTC ;	
	TCNT0 = 12 ;
	TIFR |= 1<<TOV0 ;
}

void init_timer0(void)
{
	TCNT0 = 12 ;
	TCCR0 |= (1<<CS00) | (1<<CS02) ;
	TIFR |= 1<<TOV0 ;
}