//#define __AVR_ATmega644P__

#define BAUD_0 9600		
#define BAUD_RATE_0 F_CPU/16/BAUD_0-1

#define BAUD_1 115200		
#define BAUD_RATE_1 F_CPU/16/BAUD_1-1


#define TIMER_CYCLE_US 1000000/(F_CPU/1024)

// F_CPU is set to 3 686 400 Hz (in makefile)

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "uart.h"
#include "powerlog.h"
#include "globals.h"


#include "adc.h"
 




/* WDR() compiles to an assembler instruction. Watch Dog reset */
#define WDR()  __asm__ __volatile__ ("wdr" ::)




/*volatile BYTE var = 0x00; 
volatile BYTE button_detect = 0x00;
volatile BYTE button_detect_3 = 0x00;

volatile BYTE timer_tick_detect = 0;
*/

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);



//General short delays
//general short delays
//Uses internal timer do a fairly accurate 1us
//Because we are using 16MHz and a prescaler of 8 on Timer0, we have to double x
void delay_us(uint16_t x)
{
	
	_delay_us(x);
	//while(x > 256)
	//{
	//	TIFR0 = (1<<TOV0); //Clear any interrupt flags on Timer0
	//	TCNT0 = 0; //Preload Timer0 for 256 clicks. Should be 1us per click
	//	while( (TIFR0 & (1<<TOV0)) == 0);
	//	
	//	x -= 256;
	//}

	//TIFR0 = (1<<TOV0); //Clear any interrupt flags on Timer0
	//TCNT0 = 256 - x; //256 - 125 = 131 : Preload Timer0 for x clicks. Should be 1us per click
	//while( (TIFR0 & (1<<TOV0)) == 0);
}


void delay_ms(uint16_t x)
{
	
	for (int i = x; i > 0; i--)
		delay_us(1000);
	//while(x > 256)
	//{
	//	TIFR0 = (1<<TOV0); //Clear any interrupt flags on Timer0
	//	TCNT0 = 0; //Preload Timer0 for 256 clicks. Should be 1us per click
	//	while( (TIFR0 & (1<<TOV0)) == 0);
	//	
	//	x -= 256;
	//}

	//TIFR0 = (1<<TOV0); //Clear any interrupt flags on Timer0
	//TCNT0 = 256 - x; //256 - 125 = 131 : Preload Timer0 for x clicks. Should be 1us per click
	//while( (TIFR0 & (1<<TOV0)) == 0);
}



/* **************************************************************
*
* Name			: ioinit
*
* Function		: Setup the hardware
*
* Parameters	: void
*
* Returns		: void
*
* Checked		:
*
* Requirements	:
*
***************************************************************** */

unsigned char ioinit(void)
{
  	/********************************************************/
	/* PORTA : 	ADC7 configured as input		 			*/
	/********************************************************/
	DDRA = 0x7F; //0111 1111

	// note: Display uses pins 6:0 on this port (outputs).
	//       pin ADC7 is for converting Vref (input)
	
	/********************************************************/
	/* Disable pullup resistors on PORTA 					*/
	/********************************************************/
	PORTA = 0x00;	
	
	/********************************************************/
	/* PORTB : 	0=led_on, output				 			*/
	/*			1=Timer 1 clock input (T1)			 		*/
	/*			2=INT2, sensor_n, input						*/
	/*			3=not in use, default input 				*/
	/*			4=one_wire_data, input	 					*/
	/*			5=not in use, input 						*/
	/*			6=not in use, input 						*/
	/*			7=not in use, input 						*/
	/********************************************************/
	DDRB = 0x01; // HIGH = output...			
	
	/********************************************************/
	/* Disable pullup resistors on PORTB 					*/
	/********************************************************/
	PORTB = 0x00;	 // led = off (0)
	
	/********************************************************/
	/* PORTC : 	0=I2C SCL									*/
	/*			1=I2C SDA									*/
	/*			2=Not in use								*/
	/*			3=Not in use								*/
	/*			4=Not in use								*/
	/*			5=not in use, input							*/
	/*			6=not in use, input							*/
	/*			7=not in use, input							*/
	/********************************************************/
	DDRC = 0x00;				
	
	/********************************************************/
	/* Disable pullup resistors on PORTC 					*/
	/* DISP_RESET default inactive							*/
	/* HB_LED default inactive								*/
	/********************************************************/
	PORTC = 0x00;	


	/********************************************************/
	/* PORTD : 	0=UART_RXD default input 					*/
	/*		 	1=UART_TXD default output 					*/
	/*			2=INT0 input 								*/
	/*			3=unused input 								*/
	/*			4=unused input								*/
	/*			5=unused input								*/
	/*			6=unused input								*/
	/*			7=unused input								*/
	/********************************************************/
	DDRD = 0x02;
	
	/********************************************************/
	/* Configure pullup resistors on PORTD 					*/
	/* UART_RXD inactive									*/
	/* UART_TXD inactive 									*/
	/* Remaining [7:2] inactive								*/
	/********************************************************/
	PORTD = 0x00;	

	/********************************************************/
	/* Configure external IRQ inputs						*/
	/********************************************************/
	
	// disable interrupt on INT2
	EIMSK = 0;
	
	// select falling edge for interrupt on INT2
	EICRA = (10<<ISC20); // (00<<ÍSC20); // low level interrupt on INT2

	
	// clear interrupt flags
	EIFR = (1<<INTF2) | (1<<INTF1) | (1<<INTF0);
	
	// enable external interrupt on INT2 only
	EIMSK = (1<<INT2);
	
	
	/********************************************************/
	/* Configure external pin change interrupts (PCINT)		*/
	
	PCMSK3 = 0;
	PCMSK2 = 0;
	PCMSK1 = 0;
	PCMSK0 = 0;
	
	PCICR = 0;
	
	
	
	/********************************************************/
	/* Configure and enable timer :							*/
	/* System clock Prescaler is set to OSC / 8             */
	/* Timer Prescaler CLK/1024	=> 976,5625 Hz @ 1 MHz CLK	*/
	/* Normal mode 											*/
	/********************************************************/
	TCCR1A = 0x00;														  
	//TCCR1B = 7<<CS10; // Tin clock source
	TCCR1B = 5<<CS10; // clkIO / 1024 clock source
										
	OCR1A = 0;    												 
	//DDRB = 1<<DDRB | 1<<PB1 | 1<<PB4 | 1<<PB5;   			
	//TIMSK1 = 1<<TOIE1;	

	//Init timer 0 for delay_us timing
	//8,000,000 / 8 = 1,000,000
    //TCCR0B = (1<<CS01); //Set Prescaler to 8. CS01=1


	PRR = 0;


										 

	/********************************************************/
	/* Init UART  											*/	
	/********************************************************/
	stdout = &mystdout; //Required for printf init
	
	uart_init(BAUD_RATE_0); // set up printf uart for 9600 bps
	
	uart1_init(BAUD_RATE_1); // set up hdlc uart for 115200 bps

	
	/********************************************************/
	/* enable interrupts :  clear the global interupt mask	*/
	/********************************************************/	
    sei ();    			 
	
	return(TRUE);
}





/* ****************************************************************
*
* Name			: SIG_OVERFLOW1
*
* Function		: Timer_1 owerflow Interupt routine
*
* Parameters	: void
*
* Returns		: void
*
* Modify		: Global: IRQ_done
*
* Checked		:
*
* Requirements	:
* Description	: Reads the contrast and brightness controls and write to OLED
*				  and DA - converter trough i2C bus.
*
***************************************************************** */
SIGNAL(SIG_OVERFLOW1)
{
	cli();						/* disable interrupts, no interupt during I2C bus transfer */

	timer_of++;
	TCNT1 = 0;
	printf("Timer overflow number %i\n\r", timer_of);

	sei();						/* enable interrupts */
}

/* ****************************************************************
*
* Name			: SIG_INTERUPT0
*
* Function		: Extern interupt on PORTD[2]
*
* Parameters	: void
*
* Returns		: void
*
* Checked		:
*
* Requirements	:
* Description	: IR Sensor input (active low about 1,6 ms)
*
***************************************************************** */
ISR(SIG_INTERRUPT2)
{
	uint16_t cycles;
	//uint32_t time_us;
	uint16_t time_ms;
	
	cli();						// disable interrupts, no interrupt during sensor active input
	
	
	
	cycles = TCNT1;
	TCNT1 = 0; 		// reset timer counter
	//time_us = cycles * TIMER_CYCLE_US;
	time_ms = (uint16_t) (cycles * (TIMER_CYCLE_US / 1000.0));
	power = (uint16_t) (3600000 / time_ms);

	pulse++;
	//printf("Puls nr %i @ Tid: %u cykler = %u ms = %u W\n\r", pulse, cycles, time_ms, power);
	
	

	sei();									/* enable interrupts */
}

SIGNAL(SIG_INTERRUPT1)
{
	cli();									/* disable interrupts, no interrupt during I2C bus transfer */

	

	sei();									/* enable interrupts */
}


ISR (SIG_INTERRUPT0)
{
	cli();									/* disable interrupts, no interrupt during I2C bus transfer */


	sei();									/* enable interrupts */
}


ISR (SIG_PIN_CHANGE3)
{
	cli();									/* disable interrupts, no interrupt during I2C bus transfer */

	

	sei();									/* enable interrupts */
}






unsigned char parseMsg(msgType msg)
{
	msgType txMsg;

	uint16_t temp;
	
	txMsg.devAddr = 0;
	txMsg.command = msg.command;
	txMsg.size = 0;
	txMsg.chksum = 0;
	txMsg.data_ptr = &txData[0];

	
	
	if (msg.devAddr != MYADDRESS)
		return 0;
	else
	{
		switch (msg.command)
		{
			case READ_POWER :
				
				temp = power;
				
				txMsg.size = 2;
				txData[0] = (uint8_t) (temp >> 8);		// msb
				txData[1] = (uint8_t) (temp & 0xFF);	// lsb
				
				break;
				
			case READ_PULSES : 
				
				temp = pulse;
				pulse = 0;
				
				txMsg.size = 2;
				txData[0] = (uint8_t) (temp >> 8);		// msb
				txData[1] = (uint8_t) (temp & 0xFF);	// lsb
				
				break;
				
			case READ_VOLTAGE :
			
				
				txMsg.size = 1;
				txData[0] = voltage;
				
				break;
			
			default : 

				return 0;
				
				break;
		}

		sendMsg(txMsg);
	
		return 1;
	}

}




/* ****************************************************************
*
* Name			: main
*
* Function		: main routine
*
* Parameters	: void
*
* Returns		: int 0
*
***************************************************************** */

int main(void)
{
	
	msgComplete = 0;
	
	uint16_t lastPulse = 0;
	pos = -1;
	
	// External Oscillator frequency = 8 MHz
	// set system clock frequency
	//CLKPR = 1<<CLKPCE;
	//CLKPR = 3; // set division factor to 8
	
	// Clk_sys set to 1 MHz
	


	delay_us(10000);		// wait for devices to startup
	
	ioinit();			// init peripherals and GPIO pin directions and pullups


	printf("Startup Completed\n\r");
    // loop forever, the interrupts are doing the rest
	while(1)
	{
		
		if(pollMsg() == 1) // check if message is completely received
		{
			pos = -1;
			msgComplete = 0;
			parseMsg(rxMsg);
		}
		else if (pos > 15)
		{
			// reset message buffer here
			pos = -1;
			msgComplete = 0;
			
		}
		else if (pulse != lastPulse)
		{
			lastPulse = pulse;
			voltage = read_adc();
		
			printf("%u W\n\r", power); // debug printout of power
		}
			
			

		
	}	

}
