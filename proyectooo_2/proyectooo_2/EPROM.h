/*
 * EPROM.h
 *
 * 
 *  Author: dulce
 */ 


#ifndef EPROM_H_
#define EPROM_H_

#include <avr/io.h>

void writeEEPROM(uint8_t dato, uint16_t direcc);
uint8_t readEEPROM(uint16_t direcc);



#endif /* EPROM_H_ */