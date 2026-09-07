#ifndef __EEPROM_ASM_H
#define __EEPROM_ASM_H

// === PROTOTYPES (Indexed assembly mappings) ===
uint16_t eepromReadU16(uint8_t index, uint16_t defaultValue);
void eepromWriteU16(uint8_t index, uint16_t data);

#endif /* __EEPROM_ASM_H */