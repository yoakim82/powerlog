#ifndef GLOBALS_H
#define GLOBALS_H


#define POWERLOG	0x10
#define OTHER_EP	0x20

#define MYADDRESS	POWERLOG

#define one_wire_data	4

//RFM22 Defines
#define CS 3 //pin for chip select
#define CSPORT PORTA
#define NIRQ 4
#define TXANT 5	// PD5
#define RXANT 6	// PD6



volatile uint16_t pulse = 0;
volatile uint16_t power = 0;
volatile uint8_t timer_of = 0;

volatile uint8_t voltage = 0;

//char txData[17];



#endif