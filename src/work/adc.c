#include <avr/io.h>
#include <stdio.h>

/********************************************************/
/* Init ADC :											*/	
/* Internal Aref, leftshift result 						*/
/* ADC enable, CLK/4 = sampleClk = 1.000/4 MHz 			*/
/********************************************************/
void adc_init(void)
{
	//ADMUX = (REFS1 | REFS0 | ADLAR);				
	ADMUX = 1<<ADLAR; // REFS(1:0) = 00: Vref external
	ADCSRA = (1<<ADEN | 1<<ADPS1); // ADPS(2:0) = 010: prescaler div by 4 	 
}


/* ****************************************************************
*
* Name			: read_adc
*
* Function		: reads a filtered sample from ADC
*
* Parameters	: unsigned char channel, selected analog channel
*
* Returns		: unsigned char sample,	highest 8 bit of the 10 bit sample
*
* Checked		:
*
* Requirements	:
*
* Description	: This function reads the ADC's selected analog input
*						three times and returns the sorted middle sample
***************************************************************** */
unsigned char read_adc(void)
{
/* TODO, some kind of monitoring to recover from selflock */

   unsigned char i = 0, tmp = 0, highbyte[3], replaced, channel;
   
   channel = 7; // set this fixed to channel 7 here
	
	tmp = 0x1F & channel;
	ADMUX = ADMUX & 0xE0;			/* Clear select-bits for analog input */
	ADMUX = ADMUX | tmp;
   
   while( i < 3)						/* make 3 samples */
   {  

		if((ADCSRA & (1<<ADSC)) == 0 )		/* Test if A/D conversion done */
		{
			ADCSRA = (ADCSRA | (1<<ADSC));		/* start AD conversion */
		}

   
		while((ADCSRA & (1<<ADSC)) != 0);		/* loop until A/D conversion is done */

		highbyte[i] = ADCH;					/* read high byte of sample */
		
		printf("H%i= %u   ", i, highbyte[i]); 
		i++;      
   }							/* while i < 3 */    


   do							/* bubblesort samples, simple filter to remove extrems */
   {
      replaced = 0;
      for(i = 0; i < 2; i++)
      {
         if (highbyte[i] > highbyte[i+1])
         {
            tmp = highbyte[i];
            highbyte[i] = highbyte[i+1];
            highbyte[i+1] = tmp;
            replaced = 1;
         }
      }
   }
   while (replaced == 1);
   
   return (highbyte[1]);			/* return median sample */
}
