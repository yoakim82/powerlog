#include "uart.h"
#include <avr/io.h>	

void uart_init(unsigned int ubrr)
{
	/* Set baud rate */
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	
	UCSR0A = 0;
	
	/* Enable receiver and transmitter */
	UCSR0B = (0<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format: 8 data, 1 stop bit */
	//UCSRC = (1<<URSEL)|(1<<UCSZ)|(1<<UCSZ);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void uart1_init(unsigned int ubrr)
{
	/* Set baud rate */
	UBRR1H = (unsigned char) (ubrr>>8);
	UBRR1L = (unsigned char) ubrr;
	
	
	/* Enable receiver and transmitter */
	UCSR1B = (1<<RXEN0)|(1<<TXEN0);
	
	/* Set frame format: 8 data, 1 stop bit */
	//UCSRC = (1<<URSEL)|(1<<UCSZ)|(1<<UCSZ);
	UCSR1C = (1<<USBS0)|(3<<UCSZ00);
}

void uart_send_byte(unsigned char *data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	
	/* Put data into buffer, sends the data */
	UDR0 = *data;
}

extern void copy_str(const char *in, unsigned char *msg, unsigned char length)
{
	unsigned char i;
	
	for (i = 0; i < length; i++)
	{ 
		*msg++ = *in++; 		// copy text string byte by byte
	}

}

int uart_putchar(char c, FILE *stream)
{
    if (c == '\n') uart_putchar('\r', stream);
  
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
    
    return 0;
}

uint8_t uart_getchar(void)
{
    while( !(UCSR0A & (1<<RXC0)) );
    return(UDR0);
}

