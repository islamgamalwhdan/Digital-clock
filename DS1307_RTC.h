/*
 * DS1307_RTC.h
 *
 * Created: 1/13/2016 10:58:45 PM
 *  Author: ahmed
 */ 


#ifndef DS1307_RTC_H_
#define DS1307_RTC_H_

#define RTC_ADDRESS 0x68 
#define SEC_ADDRESS 0x00
#define MINT_ADDRESS 0x01
#define HOUR_ADDRESS 0x02
#define DAY_NAME_ADDRESS 0x03
#define DAY_ADDRESS 0x04
#define MONTH_ADDRESS 0x05
#define YEAR_ADDRESS 0x06
#define MODE_24H12H_BIT 6
#define AM_PM_BIT 5
   /* Macros */
   
#define RTC_ADDRESS_WR ( ((RTC_ADDRESS)<<1) & ~(1<<0) )
#define RTC_ADDRESS_RD ( ((RTC_ADDRESS)<<1) | 0x01)

#endif /* DS1307_RTC_H_ */