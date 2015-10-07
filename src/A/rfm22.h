/*
 * RFM22.h
 *
 *  Created on: Mar 30, 2010
 *      Author: Jim Lindblom
 */

#ifndef RFM22_H_
#define RFM22_H_


#include <avr/io.h>
#include <stdio.h>
#include "globals_rfm.h"

#define DTYPE 0x00
#define DVERS 0x01
#define DSTATUS 0x02

#define INTEN1 0x05
#define INTEN2 0x06
#define OMFC1 0x07
#define OMFC2 0x08
#define CRYSLOADCAP 0x09
#define UCOUTCLK 0x0A

#define TEMPCAL 0x12
#define TEMPOFF 0x13

#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

void write_RFM(uint8_t address, char data);

char read_RFM(uint8_t address);

char rxdata_SPI(void);


void txdata_SPI(char data);


void init_SPI(void);

void init_RFM22(void);

void to_tx_mode(void);

void get_packet(void);

int checkINT(void);


#endif /* RFM22_H_ */