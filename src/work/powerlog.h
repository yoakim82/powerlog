#ifndef POWERLOG_H
#define POWERLOG_H

#include <stdio.h>
#include "hdlc.h"

#define TRUE -1
#define FALSE 0x00
#define BYTE unsigned char
#define WORD unsigned short



void delay_us(uint16_t x);

void delay_ms(uint16_t x);

unsigned char parseMsg(msgType msg);




#endif