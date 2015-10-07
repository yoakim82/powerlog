#include <avr/io.h>

/********************************************************/
/* Init ADC :											*/	
/* Internal Aref, leftshift result 						*/
/* ADC enable, CLK/32 = sampleClk = 125 kHz 			*/
/********************************************************/
void adc_init(void)
{
	//ADMUX = (REFS1 | REFS0 | ADLAR);				
	ADMUX = ADLAR; // REFS(1:0) = 00: Vref external
	ADCSRA = (ADEN | ADPS2 | ADPS1); // ADPS(2:0) = 110: prescaler div by 64 	 
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

		if((ADCSRA & ADSC) == 0 )		/* Test if A/D conversion done */
		{
			ADCSRA = (ADCSRA | ADSC);		/* start AD conversion */
		}

   
      while((ADCSRA & ADSC) == ADSC);		/* check if A/D conversion is done */

   	highbyte[i] = ADCH;					/* read high byte of sample */
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
