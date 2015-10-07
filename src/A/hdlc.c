#include <avr/interrupt.h>

#include "hdlc.h"
#include "rfm22.h"
	


// serial port 1 (to Aircom) interupt
/*
ISR(USART1_RX_vect) 
{
	unsigned char rxByte;
	cli();						// disable interrupts

		rxByte = UDR1;
		
		if (rxByte == 0x7E)
			pos = 0;
		else if (rxByte == 0x7D)
			msgComplete = 1;
		else
			rxCommand[pos++] = rxByte;


	sei();
}
*/

unsigned char pollMsg(void)
{/*
	if (pos > 0 && msgComplete == 1)
	{	
		rxMsg.devAddr = rxCommand[0];
		rxMsg.command = rxCommand[1];
		rxMsg.size = rxCommand[2];
		rxMsg.data_ptr = &rxCommand[3]; // set pointer to rx buffer
		rxMsg.chksum = rxMsg.size + 3;
		
		return 1;
	}
	else*/
		return 0;
}

unsigned char sendMsg(int size, char *data_ptr)
{
	
	unsigned char i;
	//unsigned char header[4];
	unsigned char chk = 0;
	
	//header[0] = 0x7E; // Start byte
	//header[1] = msg.devAddr;
	//header[2] = msg.command;
	//header[3] = msg.size;
//
	
	
	//// send header
	//for (i=0; i <= 3; i++)
	//{
	//	/* Wait for empty transmit buffer */
	//	while ( !( UCSR1A & (1<<UDRE0)) )
	//	;
	//
	//	/* Put data into buffer, sends the data */
	//	UDR1 = header[i];
	//}
//
	// send data
	for (i=0; i < size; i++)
	{	
	//	/* Put data into buffer, sends the data */
////
	//	UDR1 = *(msg.data_ptr);
	//	
	
	// calc chksum, increase data pointer
		chk += *(data_ptr++);
	
	
	}
	
	txData[16] = chk;
	
	to_tx_mode();
	
	// message sent

	return 1;
}




