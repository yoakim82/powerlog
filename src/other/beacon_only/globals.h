#ifndef GLOBALS_H
#define GLOBALS_H


#define POWERLOG	0x10
#define OTHER_EP	0x20

#define MYADDRESS	POWERLOG




volatile int pulse = 0;
volatile uint16_t power = 0;
volatile uint8_t timer_of = 0;

volatile uint16_t voltage = 0;


#endif