;
; Laboratorio1_231208.asm
; Laboratorio 01 - Sumador de 4 Bits
; Microcontrolador: ATMega328P (Arduino Nano)
; Ensamblador en Microchip
; AUTOR: Dulce Salguero - 231208


.INCLUDE "M328Pdef.inc"

.CSEG
.ORG 0x00
	RJMP main  ; Salto a la rutina principal

;-------------------------------------------------
; Configuración de la pila
;-------------------------------------------------
LDI     R16, LOW(RAMEND)
OUT     SPL, R16
LDI     R16, HIGH(RAMEND)
OUT     SPH, R16

;-------------------------------------------------
; Configuración del oscilador a 1MHz
;-------------------------------------------------
LDI     R16, 0x80  ; Habilitar cambio en CLKPR
STS     CLKPR, R16  ; Desbloquear registro
LDI     R16, 0x00  ; Configurar prescaler a 1 (1MHz si el reloj base es 16MHz)
STS     CLKPR, R16

;-------------------------------------------------
; Configuración inicial
;-------------------------------------------------
main:
	; Configurar pines D0-D3 como entrada con pull-ups (botones)
	LDI R16, 0x0F
	STS UCSR0B, R16
	OUT PORTD, R16
	LDI R16, 0x00
	OUT DDRD, R16

	; Configurar pines D4-D7 como salida (LEDs contador 1)
	LDI R16, 0xF0
	OUT DDRD, R16

	; Configurar pines D8-D11 como salida (LEDs contador 2)
	LDI R16, 0x0F
	OUT DDRB, R16

	; Configurar pines C0-C3 como salida para el resultado y C4 para carry
	LDI R16, 0x1F
	OUT DDRC, R16

	CLR R18  ; Inicializar contador 1
	CLR R19  ; Inicializar contador 2
	CLR R20  ; Inicializar resultado

loop:
	CALL read_buttons  ; Leer botones antes de actualizar contadores
	CALL update_counter1
	CALL update_counter2
	SBIC PIND, 4  ; Verificar si el botón de suma (D4) está presionado
	CALL check_sum
	RJMP loop  ; Repetir ciclo

;-------------------------------------------------
; Subrutina para leer botones con antirebote
;-------------------------------------------------
read_buttons:
	IN R16, PIND   ; Leer botones en D0-D3
	CP R17, R16
	BREQ read_buttons
	CALL DELAY
	IN R16, PIND
	CP R17, R16
	BREQ read_buttons
	MOV R17, R16
	RET

;-------------------------------------------------
; Contador 1 con incremento y decremento en D4-D7
;-------------------------------------------------
update_counter1:
	SBIC PIND, 0
	INC R18
	SBIC PIND, 1
	DEC R18
	ANDI R18, 0x0F
	OUT PORTD, R18  ; Mostrar contador en LEDs D4-D7
	RET

;-------------------------------------------------
; Contador 2 con incremento y decremento en D8-D11
;-------------------------------------------------
update_counter2:
	SBIC PIND, 2
	INC R19
	SBIC PIND, 3
	DEC R19
	ANDI R19, 0x0F
	OUT PORTB, R19  ; Mostrar contador en LEDs D8-D11
	RET

;-------------------------------------------------
; Suma de contadores y manejo de overflow
;-------------------------------------------------
check_sum:
	CLR R20
	ADD R20, R18   ; Sumar contador 1
	ADD R20, R19   ; Sumar contador 2
	LSL R20
	LSL R20
	LSL R20
	LSL R20
	BRCS carry  ; Si hay carry, activar LED de overflow en C4
	CBI PORTC, 4
mostrar:
	ANDI R20, 0x0F
	OUT PORTC, R20  ; Mostrar suma en LEDs C0-C3
	RJMP loop

carry:
	SBI PORTC, 4
	RJMP mostrar

;-------------------------------------------------
; Subrutina de delay para antirebote
;-------------------------------------------------
DELAY:
	LDI R21, 255
SUBDELAY:
	DEC R21
	BRNE SUBDELAY
	RET
