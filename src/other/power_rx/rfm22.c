#include "rfm22.h"
#include "powerlog.h"



void init_RFM22(void)
{
	write_RFM(INTEN2, 0x00);	// Disable all interrupts
	write_RFM(OMFC1, 0x01);		// Set READY mode
	write_RFM(0x09, 0x7F);		// Cap = 12.5pF
	//write_RFM(0x0A, 0x05);		// Clk output is 2 MHz
	//write_RFM(0x0A, 0x06);		// Clk output is 1 MHz (default)		
	
	write_RFM(0x0B, 0xF4);		// GPIO0 is for RX data output
	write_RFM(0x0C, 0xEF);		// GPIO1 is TX/RX data CLK output
	write_RFM(0x0D, 0x00);		// GPIO2 for MCLK output
	write_RFM(0x0E, 0x00);		// GPIO port use default value
	
	write_RFM(0x0F, 0x70);		// NO ADC used
	write_RFM(0x10, 0x00);		// no ADC used
	write_RFM(0x12, 0x00);		// No temp sensor used
	write_RFM(0x13, 0x00);		// no temp sensor used
	
	write_RFM(0x70, 0x20);		// No manchester code, no data whiting, data rate < 30Kbps
	
	write_RFM(0x1C, 0x1D);		// IF filter bandwidth
	write_RFM(0x1D, 0x40);		// AFC Loop
	//write_RFM(0x1E, 0x0A);	// AFC timing
	
	write_RFM(0x20, 0xA1);		// clock recovery
	write_RFM(0x21, 0x20);		// clock recovery
	write_RFM(0x22, 0x4E);		// clock recovery
	write_RFM(0x23, 0xA5);		// clock recovery
	write_RFM(0x24, 0x00);		// clock recovery timing
	write_RFM(0x25, 0x0A);		// clock recovery timing
	
	//write_RFM(0x2A, 0x18);
	write_RFM(0x2C, 0x00);
	write_RFM(0x2D, 0x00);
	write_RFM(0x2E, 0x00);
	
	write_RFM(0x6E, 0x27);		// TX data rate 1
	write_RFM(0x6F, 0x52);		// TX data rate 0
	
	write_RFM(0x30, 0x8C);		// Data access control
	
	write_RFM(0x32, 0xFF);		// Header control
	
	write_RFM(0x33, 0x42);		// Header 3, 2, 1, 0 used for head length, fixed packet length, synchronize word length 3, 2,
	
	write_RFM(0x34, 64);		// 64 nibble = 32 byte preamble
	write_RFM(0x35, 0x20);		// 0x35 need to detect 20bit preamble
	write_RFM(0x36, 0x2D);		// synchronize word
	write_RFM(0x37, 0xD4);
	write_RFM(0x38, 0x00);
	write_RFM(0x39, 0x00);
	write_RFM(0x3A, 's');		// set tx header 3
	write_RFM(0x3B, 'o');		// set tx header 2
	write_RFM(0x3C, 'n');		// set tx header 1
	write_RFM(0x3D, 'g');		// set tx header 0
	write_RFM(0x3E, 17);		// set packet length to 17 bytes
	
	write_RFM(0x3F, 's');		// set rx header
	write_RFM(0x40, 'o');
	write_RFM(0x41, 'n');
	write_RFM(0x42, 'g');
	write_RFM(0x43, 0xFF);		// check all bits
	write_RFM(0x44, 0xFF);		// Check all bits
	write_RFM(0x45, 0xFF);		// check all bits
	write_RFM(0x46, 0xFF);		// Check all bits
	
	write_RFM(0x56, 0x01);
	
	write_RFM(0x6D, 0x07);		// Tx power to max
	
	write_RFM(0x79, 0x00);		// no frequency hopping
	write_RFM(0x7A, 0x00);		// no frequency hopping
	
	write_RFM(0x71, 0x22);		// GFSK, fd[8]=0, no invert for TX/RX data, FIFO mode, txclk-->gpio
	
	write_RFM(0x72, 0x48);		// Frequency deviation setting to 45K=72*625
	
	write_RFM(0x73, 0x00);		// No frequency offset
	write_RFM(0x74, 0x00);		// No frequency offset
	
	write_RFM(0x75, 0x53);		// frequency set to 434MHz
	write_RFM(0x76, 0x64);		// frequency set to 434MHz
	write_RFM(0x77, 0x00);		// frequency set to 434Mhz
	
	write_RFM(0x5A, 0x7F);
	write_RFM(0x59, 0x40);
	write_RFM(0x58, 0x80);
	
	write_RFM(0x6A, 0x0B);
	write_RFM(0x68, 0x04);
	write_RFM(0x1F, 0x03);
}

void to_tx_mode(void)
{
	unsigned char i;
	
	write_RFM(0x07, 0x01);	// To ready mode
	cbi(PORTD, RXANT);
	sbi(PORTD, TXANT);
	delay_ms(50);
	
	write_RFM(0x08, 0x03);	// FIFO reset
	write_RFM(0x08, 0x00);	// Clear FIFO
	
	write_RFM(0x34, 64);	// preamble = 64nibble
	write_RFM(0x3E, 17);	// packet length = 17bytes
	for (i=0; i<17; i++)
	{
		write_RFM(0x7F, txData[i]);	// send payload to the FIFO
	}

	write_RFM(0x05, 0x04);	// enable packet sent interrupt
	i = read_RFM(0x03);		// Read Interrupt status1 register
	i = read_RFM(0x04);
	
	write_RFM(0x07, 9);	// Start TX
	
	while ((PIND & (1<<NIRQ)) != 0)
		; 	// need to check interrupt here
	
	write_RFM(0x07, 0x01);	// to ready mode
	
	cbi(PORTD, RXANT);	// disable all interrupts
	cbi(PORTD, TXANT);
}

void to_rx_mode(void)
{
	write_RFM(0x07, 0x01);	// to ready mode
	
	sbi(PORTD, RXANT);
	//printf("a");
	cbi(PORTD, TXANT);
	printf("b");
	delay_ms(50);
	printf("bb");
	rx_reset();
}

void rx_reset(void)
{
	unsigned char i;
	
	printf("rst");	
	
	write_RFM(0x07, 0x01);	// to ready mode
	printf("c");
	i = read_RFM(0x03);
	i = read_RFM(0x04);
	
	write_RFM(0x7E, 17);
	
	write_RFM(0x08, 0x03);
	write_RFM(0x08, 0x00);
	
	write_RFM(0x07, 5);
	
	write_RFM(0x05, 2);
}

void get_packet(void)
{
	//unsigned char i, chksum;
	//
	//for(i=0; i<16; i++)
	//{
	//	tx_buf[i] = uart_getchar();
	//	printf("Received %c, %d characters remaining for packet\n", tx_buf[i], 15-i);
	//}
	//
	//chksum = 0;
	//for(i=0; i<16; i++)
	//	chksum += tx_buf[i];
	//
	//tx_buf[16] = chksum;
}

int checkINT(void)
{
int retval = 0;

	if ((PIND & (1<<NIRQ)) == 0)
		printf("INT == 0\n");
	else
	{
		printf("INT == 1\n");
		retval = 1;
	}
	return retval;
}



void write_RFM(uint8_t address, char data)
{
	//write any data byte to any single address
	//adds a 0 to the MSB of the address byte (WRITE mode)
	
	address |= 0x80;

	cbi(CSPORT,CS);
	delay_ms(1);
	txdata_SPI(address);
	delay_ms(1);
	txdata_SPI(data);
	delay_ms(1);
	sbi(CSPORT,CS);
}

char read_RFM(uint8_t address)
{
	//returns the contents of any 1 byte register from any address
	//sets the MSB for every address byte (READ mode)

	char byte;
	
	address &= 0x7F;

	cbi(CSPORT,CS);
	txdata_SPI(address);
	byte = rxdata_SPI();
	sbi(CSPORT,CS);

	return byte;
}

char rxdata_SPI(void)
{
	SPDR = 0x55;
	while((SPSR&0x80) == 0x00)
		;

	return SPDR;
}

void txdata_SPI(char data)
{
	SPDR = data;
	while((SPSR&0x80) == 0x00);
}

void init_SPI(void)
{
	// enable SPI
	// make SPI master
	// SCLK idle low
	// sample data on rising edge
	// Fosc/4 is SPI frequency = 2MHz
	//SPCR |= 0b01010000;	// Fosc/4
	SPCR |= 0b01010000;		// Fosc/4 = 250 kHz
}
