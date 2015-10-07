#ifndef HDLC_H
#define HDLC_H

#define READ_POWER		0x30
#define READ_PULSES		0x31
#define READ_VOLTAGE	0x40

#define CTRL_1			PA2
#define CTRL_0			PA3


typedef struct
{
	unsigned char startFlag;				// Start of frame marker
	unsigned char devAddr;					// endpoint address (0 is master) 
	unsigned char command;					// ack/nack/command 
	unsigned char size;						// data field size	
	unsigned char *data_ptr;				// Pointer to the received bytes
	unsigned char chksum;
}msgType;									




unsigned char pollMsg(void);

unsigned char sendMsg(msgType msg);


// Global variables used by hdlc
unsigned char rxCommand[10]; 		// receive buffer for USART ISR
unsigned char txData[3];			// tx buffer for data part of hdlc reply message
unsigned char pos, msgComplete;		// used for communication b/w ISR and pollMsg

void startTx(void);
void stopTx(void);

msgType rxMsg;
#endif