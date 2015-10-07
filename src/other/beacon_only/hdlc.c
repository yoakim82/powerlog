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

void startTx(void)
{
	// ASK mode:
	// enable Tx by setting CTRL(1:0) = 10
	//PORTA = (1 << CTRL_1) | (0 << CTRL_0); 
	
	
	//OOK mode:
	// enable Tx by setting CTRL(1:0) = 01
	PORTA = (0 << CTRL_1) | (1 << CTRL_0);
	
	//define CTRL_1			PA2
	//define CTRL_0			PA3

	//PA2 = CTRL_1: always high
	//PA3 = CTRL_0: low for ASK Tx

}

void stopTx(void)
{
	// enable Rx by setting CTRL(1:0) = 11
	PORTA = (1 << CTRL_1) | (1 << CTRL_0); 
	
}


unsigned char sendMsg(msgType msg)
{
	
	unsigned char i;
	unsigned char header[4];
	unsigned char trailer[2];
	
//	header[0] = 0x7E; // Start byte
//	header[1] = msg.devAddr;
//	header[2] = msg.command;
//	header[3] = msg.size;

	header[0] = 'h';
	header[1] = 'e';
	header[2] = 'l';
	header[3] = 'o';

	
//	trailer[0] = msg.chksum; 	// update below with correct value
//	trailer[1] = 0x7D; 			// stop byte
	
	trailer[0] = 'p'; 	// update below with correct value
	trailer[1] = 'p'; 			// stop byte
	
	startTx();
	
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
	
//	trailer[0] = msg.chksum;
	trailer[0] = 'p';
	
	// send trailer
	for (i=0; i < 2; i++)
	{
		/* Wait for empty transmit buffer */
		while ( !( UCSR1A & (1<<UDRE0)) )
		;
	
		/* Put data into buffer, sends the data */
		UDR1 = trailer[i];
	}
	
	while ( !( UCSR1A & (1<<UDRE0)) )
		;
	delay_us(10);
	
	// message sent
	stopTx();

	return 1;
}




