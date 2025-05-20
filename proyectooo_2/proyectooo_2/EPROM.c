/*
 * EPROM.c
 *
 * 
 *  Author: dulce
 */ 
#include "EPROM.h"

void writeEEPROM(uint8_t dato, uint16_t direcc)
{
	while (EECR &(1<<EEPE));
	EEAR = direcc;
	EEDR = dato;
	EECR |= (1<<EEMPE);
	EECR |= (1<<EEPE);
	
}

uint8_t readEEPROM(uint16_t direcc)
{
	while (EECR &(1<<EEPE));
	EEAR = direcc;
	EECR |= (1<<EERE);
	return EEDR;
	
}