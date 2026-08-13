#ifndef __EEPROM_H
#define __EEPROM_H

#include "stm8s.h"

#define EEPROM_DEFAULT_ADDR 0x4000

uint16_t eepromReadU16(uint16_t, uint16_t);
void eepromWriteU16(uint16_t addr, uint16_t data);

#endif /* __EEPROM_H */