#ifndef UART_H
#define UART_H

#include <stdio.h>

int uart_putchar(char c, FILE *stream);
uint8_t uart_getchar(void);

extern void uart_init(unsigned int ubrr);
extern void uart_send_byte(unsigned char *data);
extern void copy_str(const char *in, unsigned char *msg, unsigned char length);

#endif