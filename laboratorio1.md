; lab_1.asm
; Autor: Dulce Salguero - 231208
; Laboratorio 1 - Sección 10

.INCLUDE "M328Pdef.inc"

.CSEG
.ORG 0x00
	RJMP main  ; Salto a la rutina principal


;  CONFIGURACIÓN DE LA PILA

LDI     R16, LOW(RAMEND)
OUT     SPL, R16
LDI     R16, HIGH(RAMEND)
OUT     SPH, R16


; CONFIGURACIÓN PARA COMENZAR

main:
	; Configurar pines D0-D4 como entrada con pull-ups (botones)
	LDI R16, 0x00
	OUT DDRD, R16
	LDI R16, 0x1F
	OUT PORTD, R16

	; Configurar pines D5-D8 como salida (LEDs contador 1)
	LDI R16, 0xF0
	OUT DDRD, R16

	; Configurar pines D9-D12 como salida (LEDs contador 2)
	LDI R16, 0x1E
	OUT DDRB, R16

	; Configurar pines C0-C3 como salida para el resultado y C4 para carry
	LDI R16, 0x1F
	OUT DDRC, R16

	LDI R17, 0xFF   ; R17 se usará para guardar el estado de los botones
	CALL init_counter  ; Inicializar contadores

loop:
	CALL update_counter1
	CALL update_counter2
	CALL check_sum
	RJMP loop  ; Repetir ciclo


; ANTIREBOTE Y LECTURA DE BOTONES

read_buttons:
	IN R16, PIND   ; Leer botones en D0-D4
	CP R17, R16
	BREQ read_buttons
	CALL DELAY
	IN R16, PIND
	CP R17, R16
	BREQ read_buttons
	MOV R17, R16
	RET


; INICIALIZACIÓN DE CONTADORES

init_counter:
	CLR R18  ; Contador 1 en R18
	CLR R19  ; Contador 2 en R19
	RET


; CONTADOR 1 con incremento y decremento

update_counter1:
	CALL read_buttons
	SBIC PIND, 0
	INC R18
	SBIC PIND, 1
	DEC R18
	OUT PORTD, R18  ; Mostrar contador en LEDs D5-D8
	RET


; CONTADOR 2 con incremento y decremento

update_counter2:
	CALL read_buttons
	SBIC PIND, 2
	INC R19
	SBIC PIND, 3
	DEC R19
	OUT PORTB, R19  ; Mostrar contador en LEDs D9-D12
	RET


; SUMA DE CONTADORES Y OVERFLOW

check_sum:
	ADD R20, R18   ; Sumar contador 1
	ADD R20, R19   ; Sumar contador 2
	BRCC no_carry  ; Si no hay acarreo, continuar
	LDI R16, 0x10  ; Encender LED de overflow en C4
	OUT PORTC, R16
	RJMP end_sum

no_carry:
	OUT PORTC, R20  ; Mostrar suma en LEDs C0-C3

end_sum:
	RET


; SUBRUTINA DE DELAY

DELAY:
	LDI R21, 255
SUBDELAY:
	DEC R21
	BRNE SUBDELAY
	RET



