#include <avr/interrupt.h>
#include "hdlc.h"
#include "uart.h"
//#include "globals.h"	


// serial port 1 (to Aircom) interupt
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


	sei();						/* enable interrupts */
}


unsigned char pollMsg(void)
{
	if (pos > 0 && msgComplete == 1)
	{	
		rxMsg.devAddr = rxCommand[0];
		rxMsg.command = rxCommand[1];
		rxMsg.size = rxCommand[2];
		rxMsg.data_ptr = &rxCommand[3]; // set pointer to rx buffer
		rxMsg.chksum = rxMsg.size + 3;
		
		return 1;
	}
	else
		return 0;
}

unsigned char sendMsg(msgType msg)
{
	
	unsigned char i;
	unsigned char header[4];
	unsigned char trailer[2];
	
	header[0] = 0x7E; // Start byte
	header[1] = msg.devAddr;
	header[2] = msg.command;
	header[3] = msg.size;

	
	trailer[0] = msg.chksum; 	// update below with correct value
	trailer[1] = 0x7D; 			// stop byte
	
	
	// send header
	for (i=0; i <= 3; i++)
	{
		/* Wait for empty transmit buffer */
		while ( !( UCSR1A & (1<<UDRE0)) )
		;
	
		/* Put data into buffer, sends the data */
		UDR1 = header[i];
	}

	// send data
	for (i=0; i < msg.size; i++)
	{
		/* Wait for empty transmit buffer */
		while ( !( UCSR1A & (1<<UDRE0)) )
		;
	
		/* Put data into buffer, sends the data */

		UDR1 = *(msg.data_ptr);
		
		// calc chksum, increase data pointer
		msg.chksum += *(msg.data_ptr++);
	}
	
	trailer[0] = msg.chksum;
	
	// send trailer
	for (i=0; i < 2; i++)
	{
		/* Wait for empty transmit buffer */
		while ( !( UCSR1A & (1<<UDRE0)) )
		;
	
		/* Put data into buffer, sends the data */
		UDR1 = trailer[i];
	}
	
	// message sent

	return 1;
}




