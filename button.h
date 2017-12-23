/*
 * button.h
 *
 * Created: 10/2/2015 5:41:52 PM
 *  Author: islam
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

#define PULL_UP 1
#define PULL_DOWN 0

void init_button( pin_t *pin) ;
uint8 button_read( pin_t *pin ) ;



#endif /* BUTTON_H_ */