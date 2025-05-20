/*
 * proyectooo_2.c
 * Universidad del Valle de Guatemala 
 * Programación de Microcontroladores - 10
 * Author : dulce salguero - 231208
 * Descripción: sistema de ojos mecánicos, 6 servos, 1 joystick, 1 pote, 2 botones - Control de PWM, EEPROM
 * Hardware:
 *  - 6 servos: PWM por Timers 0,1,2 - D9, D10, D11, D3, D6, D5
 *  - Joystick:  X = A6, Y = A7
 *  - Potenciómetro: A0
 *  - Botón  D2  (cambio de modo)
 *  - Botón D7 (escritura/lectura)
 *  - LED   D13  (indica el modo actual)
 *  - LED A1
 */

#define F_CPU 16000000UL 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "EPROM.h"
#include "UART.h"

typedef enum {
	MODO_FISICO,
	MODO_EPROM,
	MODO_SERIAL
} ModoOperacion;

volatile ModoOperacion modo_actual = MODO_FISICO;

// Prototipos
void setup();
void initADC();
uint16_t leerADC(uint8_t canal);
void PWM_init();
void PWM_init0();
void PWM_init2();
void PWM_setAngle(uint16_t angle);
void PWM_setAngle2(uint16_t mov);
void PWM_setAngle3(uint16_t tung);
void PWM_setAngle4(uint16_t cerati);
void PWM_setAngle5(uint16_t angle5);
void PWM_setAngle6(uint16_t angle6);
uint8_t boton_presionado();
void cambiar_modo();
void actualizar_indicadores_modo();

uint8_t escribir = 0;
uint8_t direccion = 0;
uint8_t paqueton = 0;

volatile char received_char;
volatile uint8_t new_data = 0;

int main(void)
{
	setup();
	initADC();
	PWM_init();
	PWM_init2();
	PWM_init0();
	actualizar_indicadores_modo();
	
	while (1) 
	{
		cambiar_modo();
		switch(modo_actual){
			case MODO_FISICO:
			{  
				uint16_t servo1 = leerADC(6);
				uint16_t angle = (servo1 * -1.96) + 2100.0;
				PWM_setAngle(angle);
				
				uint16_t servo2 = leerADC(6);
				uint16_t mov = (servo2 * 1.96) + 1199.0;
				PWM_setAngle2(mov);
				
				uint16_t lol = leerADC(6);
				uint16_t tung = (lol * 0.0137) + 9;
				PWM_setAngle3(tung);
				
				uint16_t caifanes = leerADC(7);
				uint16_t cerati = (caifanes * 0.00684) + 9;
				PWM_setAngle4(cerati);
				
				uint16_t cejas = leerADC(0);
				uint16_t angle5 = (cejas * 0.0039) + 9;
				PWM_setAngle5(angle5);
				
				uint16_t cejas2 = leerADC(0);
				uint16_t angle6 = (cejas2 * -0.0039) + 13;
				PWM_setAngle6(angle6);
				
				if(escribir == 1 && paqueton <=5)
				{
					writeEEPROM(OCR1A, direccion++);
					writeEEPROM(OCR1B, direccion++);
					writeEEPROM(OCR2A, direccion++);
					writeEEPROM(OCR2B, direccion++);
					writeEEPROM(OCR0A, direccion++);
					writeEEPROM(OCR0B, direccion++);
					paqueton++;
					escribir = 0;
				}
				break;  
			}
			case MODO_EPROM:
				if (escribir >= 1 && escribir <= 4){
					uint8_t paquetito = (escribir - 1)* 6;
					OCR1A = readEEPROM( paquetito);
					OCR1B = readEEPROM(paquetito + 1);
					OCR2A = readEEPROM(paquetito + 2);
					OCR2B = readEEPROM(paquetito + 3);
					OCR0A = readEEPROM(paquetito + 4);
					OCR0B = readEEPROM(paquetito + 5);
				}
				break;
			case MODO_SERIAL:
				writeString("\r\nModo SERIAL activado. Listo para comandos.\r\n");
				mostrarMenu();
				while (modo_actual == MODO_SERIAL) {
					if (new_data) {
						switch (received_char){
							case '1': OCR2A = 23; writeString("\r\nGlobo ocular DERECHA\r\n"); break;
							case '2': OCR2A = 9; writeString("\r\nGlobo ocular IZQUIERDA\r\n"); break;
							case '3': OCR0A = 6; writeString("\r\nCerrando párpado IZQUIERDO\r\n"); break;
							case '4': OCR0B = 13; writeString("\r\nCerrando párpado DERECHO\r\n"); break;
							case '5': OCR2B = 7; writeString("\r\nViendo ABAJO\r\n"); break;
							default: writeString("\r\nComando no válido. Intente de nuevo.\r\n"); break;
						}
						new_data = 0; 
						mostrarMenu(); 
					}
					PORTB ^= (1 << PB5);
					_delay_ms(200);
					cambiar_modo();
					actualizar_indicadores_modo();
				}
				break;
		}
	}
}

void setup() {
	cli();
	DDRC = 0x00;
	DDRB |= (1 << PB5);
	DDRC |= (1 << PC1);
	PORTB &= ~(1 << PB5);
	PORTC &= ~(1 << PC1);
	DDRD |= (1 << PD2) | (1 << PD7);
	PORTD |= (1 << PD2) | (1 << PD7);
	PCMSK2 |= (1 << PIND7);
	PCICR |= (1 << PCIE2);
	initUART();
	sei();
}

void initADC(){
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t leerADC(uint8_t canal) {
	ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

void PWM_init() {
	TCCR1A |= (1 << COM1A1) | (1 << WGM11) | (1 << COM1B1);
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS11);
	ICR1 = 65000;
	DDRB |= (1 << PORTB1) | (1 << PORTB2);
}

void PWM_init2() {
	TCCR2A |= (1 << COM2A1) | (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
	DDRD |= (1 << PORTD3);
	DDRB |= (1 << PORTB3);
}

void PWM_init0() {
	TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
	TCCR0B |= (1 << CS02)  | (1 << CS00);
	DDRD |= (1 << PORTD6);
	DDRD |= (1 << PORTD5);
}

void PWM_setAngle(uint16_t angle) { OCR1A = angle; }
void PWM_setAngle2(uint16_t mov) { OCR1B = mov; }
void PWM_setAngle3(uint16_t tung) { OCR2A = tung; }
void PWM_setAngle4(uint16_t cerati) { OCR2B = cerati; }
void PWM_setAngle5(uint16_t angle5) { OCR0A = angle5; }
void PWM_setAngle6(uint16_t angle6) { OCR0B = angle6; }

uint8_t boton_presionado() {
	if (!(PIND & (1 << PORTD2))) {
		_delay_ms(20);
		if (!(PIND & (1 << PORTD2))) return 1;
	}
	return 0;
}

void cambiar_modo() {
	if (boton_presionado()) {
		while (!(PIND & (1 << PORTD2)));
		_delay_ms(20);
		modo_actual = (modo_actual + 1) % 3;
		actualizar_indicadores_modo();
	}
}

void actualizar_indicadores_modo() {
	switch (modo_actual) {
		case MODO_FISICO:
			PORTB |= (1 << PB5);
			PORTC &= ~(1 << PC1);
			break;
		case MODO_EPROM:
			PORTB &= ~(1 << PB5);
			PORTC |= (1 << PC1);
			break;
		case MODO_SERIAL:
			PORTB &= ~(1 << PB5);
			PORTC &= ~(1 << PC1);
			break;
	}
}

ISR(USART_RX_vect) {
	received_char = UDR0;
	new_data = 1;
	writeChar(received_char);
}

ISR(PCINT2_vect){
	PORTB |= (1 << PORTB5);
	if (!(PIND & (1 << PORTD7))){
		escribir++;
		if (escribir == 5) escribir = 0;
	}
}
