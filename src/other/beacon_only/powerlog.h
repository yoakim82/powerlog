#ifndef POWERLOG_H
#define POWERLOG_H

#include <stdio.h>
#include "hdlc.h"

#define TRUE -1
#define FALSE 0x00
#define BYTE unsigned char
#define WORD unsigned short




#define one_wire_data	4


unsigned char parseMsg(msgType msg);




#endif