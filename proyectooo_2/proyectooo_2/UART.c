/*
 * UART.c
 *
 * 
 *  Author: dulce
 */ 

#include "UART.h"

void initUART() {
	// P1: Configurar PD1 (TX) como salida y PD0 (RX) como entrada
	DDRD |= (1 << DDD1); // TX (Salida)
	DDRD &= ~(1 << DDD0); // RX (Entrada)
	
	// P2: Configurar UCSR0A (Modo normal)
	UCSR0A = 0;
	
	// P3: Habilitar TX, RX e interrupción por RX
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	// P4: Configurar formato: 8 bits, sin paridad, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	// P5: Baud rate a 9600 (@16MHz)
	UBRR0 = 103;
}

void writeChar(char caracter) {
	while ((UCSR0A & (1 << UDRE0)) == 0); // Esperar buffer TX vacío
	UDR0 = caracter; // Enviar carácter
}

void writeString(char*texto){
	for (uint8_t i = 0; *(texto + i) != '\0'; i++)
	{
		writeChar(*(texto+i));
	}
	
}

void mostrarMenu() {
	writeString("\r\n--- MENU DE COMANDOS ---\r\n");
	writeString("1:Mover globo ocular ARRIBA\r\n");
	writeString("2:Mover globo ocular ABAJO\r\n");
	writeString("3:Mirar IZQUIERDA\r\n");
	writeString("4:Mirar DERECHA\r\n");
	writeString("5:ABRIR parpados\r\n");
	writeString("6:CERRAR parpados\r\n");
	writeString("R:RESET (Posicion inicial)\r\n");
	writeString("-----------------------\r\n");
	writeString("Ingrese comando: ");
}