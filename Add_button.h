/*
 * Add_button.h
 *
 * Created: 1/19/2016 7:37:44 PM
 *  Author: ahmed
 */ 
#include "config.h"

#ifndef ADD_BUTTON_H_
#define ADD_BUTTON_H_

////////////////////////////////////
/* Global variables */
extern uint8 TimeDate_Adjust_24H  ;
extern uint8 TimeDate_Adjust_12H  ;
extern uint8 Alarm_Adjust_24H  ;
extern uint8 Alarm_Adjust_12H  ;
extern uint8 mode_24H  ; 
extern uint8 Am_Pm ;
extern uint8 Alarm_status  ;
extern uint8 Alarm_Checking  ;
////////////////////////////////////////

/*Global Functions */

void Add_Button_action(Time_st*,Buttons_st* ) ;
void Rtc_Write( uint8 , uint8 );

/*Internal functions to Add button */

static void Hour_12H_Edit(Time_st* ) ;
static void Hour_24H_Edit(Time_st* ) ;
static void Mint_Edit(Time_st* ) ;
static void Day_Edit(Time_st* ) ;
static void Month_Edit(Time_st* );
static void Year_Edit(Time_st* );
static void DayName_Edit(Time_st* );
static void AmPm_Edit(Time_st*) ;
static void Hour_Alrm_24H(Time_st* );
static void Hour_Alrm_12H(Time_st* );
static void Mint_Alrm_Edit(Time_st* );
static void AmPm_Alrm_Edit(Time_st * ) ;


#endif /* ADD_BUTTON_H_ */