/*
 * UART.h
 *
 * 
 *  Author: dulce
 */ 


#ifndef UART_H_
#define UART_H_


#include <avr/io.h>

void initUART();
void writeChar(char caracter);
void writeString(char* texto);



#endif /* UART_H_ */