
/*
 * types.h
 *
 * Created: 10/2/2015 4:15:49 PM
 *  Author: islam
 */ 


    #ifndef TYPES_H_
#define TYPES_H_

typedef unsigned long int uint32 ;
typedef unsigned int uint16 ;
typedef unsigned char uint8 ;

typedef struct pin_ifo
{
	volatile uint8 reg ,
	      pin_num,
		  val,
		  state,
		  No_Offpress;
}pin_t;

typedef struct time
{
	volatile uint8 sec  ,
	      mint ,
		  hour ,
		  day  ,
		  day_name ,
		  month ,
		  year ,
		  alarm_hr ,
		  am_pm_alrm ,
		  alarm_mnt;
} Time_st ;

typedef struct buttons
{
	pin_t* menu ,
	      ok ,
		  add ;
}	Buttons_st ;

 /*typedef struct pins
{
	uint8 F0 : 1 ,
	      F1 : 1 ,
		  F2 : 1 ,
		  F3 : 1 ,
		  F4 : 1 ,
		  F5 : 1 ,
		  F6 : 1 ,
		  F7 : 1 ;
}Pin_t;

typedef union var
{
	uint8 var_8 ;
	Pin_t pin ;
}Var_t ; */			  
	     

#endif /* TYPES_H_ */