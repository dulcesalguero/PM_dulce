;
; PreLaboratorio02.asm
; Timer0
; Autor: Dulce Salguero - 231208
; Progra de Micros - 10 

// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"
.cseg
.org 0x0000
.def    COUNTER = R20
.def    TIMER_COUNT = R21  // Contador de software para alcanzar 100 ms
.def    TEMP = R22         // Registro temporal para lectura del botón

/****************************************/
// Configuración de la pila
LDI     R16, LOW(RAMEND)
OUT     SPL, R16
LDI     R16, HIGH(RAMEND)
OUT     SPH, R16
/****************************************/
// Configuración MCU
SETUP:
    // Configurar Prescaler "Principal"
    LDI     R16, (1 << CLKPCE)
    STS     CLKPR, R16          // Habilitar cambio de PRESCALER
    LDI     R16, 0b00000100
    STS     CLKPR, R16          // Configurar Prescaler a 16 F_cpu = 1MHz

    // Inicializar Timer0
    CALL    INIT_TMR0

    // Configurar PB3, PB2, PB1, PB0 como salida para los LEDs
    LDI     R16, 0b00001111     // 0000 1111 ? PB3, PB2, PB1, PB0 como salidas
    OUT     DDRB, R16
    CBI     PORTB, 3            // Asegurar LEDs apagados al inicio
    CBI     PORTB, 2
    CBI     PORTB, 1
    CBI     PORTB, 0

    // Configurar PB4 como entrada con pull-up activado (botón)
    CBI     DDRB, 4             // PB4 como entrada
    SBI     PORTB, 4            // Activar pull-up interno (PB4 = HIGH cuando no se presiona)

    // Deshabilitar serial (para evitar interferencias en los pines)
    LDI     R16, 0x00
    STS     UCSR0B, R16

    LDI     COUNTER, 0x00
    LDI     TIMER_COUNT, 0x00
/****************************************/
// Loop Infinito
MAIN_LOOP:
    IN      R16, TIFR0          // Leer registro de interrupción de TIMER 0
    SBRS    R16, TOV0           // Salta si el bit TOV0 no está en 1
    RJMP    MAIN_LOOP           
    SBI     TIFR0, TOV0         // Limpiar bandera de "overflow"
    LDI     R16, 100            
    OUT     TCNT0, R16          // Recargar Timer0 para otro ciclo de 10 ms

    INC     TIMER_COUNT
    CPI     TIMER_COUNT, 0x0A    // 10 desbordamientos = 100 ms
    BRNE    MAIN_LOOP

    CLR     TIMER_COUNT

    CALL    CHECK_BUTTON        // Verificar si el botón está presionado
    CPI     R22, 1              // ¿Botón presionado?
    BRNE    MAIN_LOOP           // Si no está presionado, sigue en loop

    INC     COUNTER
    ANDI    COUNTER, 0x0F       // Limitar a 4 bits (0-15)
    OUT     PORTB, COUNTER      // Mostrar en los LEDs
    RJMP    MAIN_LOOP

/****************************************/
// Subrutina para verificar si el botón está presionado
CHECK_BUTTON:
    IN      TEMP, PINB          // Leer estado de PB4
    SBIC    PINB, 4             // Si PB4 está en HIGH, salta la siguiente instrucción
    RJMP    BUTTON_PRESSED
    LDI     R22, 0              // Botón NO presionado
    RET

BUTTON_PRESSED:
    CALL    DEBOUNCE            // Aplicar anti-rebote
    LDI     R22, 1              // Botón presionado
    RET

/****************************************/
// Subrutina de anti-rebote para el botón (simple delay)
DEBOUNCE:
    LDI     R16, 50             // Pequeña pausa (~5ms en 1MHz)
DEBOUNCE_LOOP:
    DEC     R16
    BRNE    DEBOUNCE_LOOP
    RET

/****************************************/
// Configuración del Timer0 sin interrupciones
INIT_TMR0:
    LDI     R16, (1<<CS01) | (1<<CS00) // Prescaler = 64 ? Tick cada 64 us
    OUT     TCCR0B, R16         
    LDI     R16, 100                     // Ajustar TCNT0 para 10 ms
    OUT     TCNT0, R16
    RET
