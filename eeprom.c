#include "eeprom.h"

uint16_t eepromReadU16(uint16_t addr, uint16_t defaultValue) {
	uint16_t value = FLASH_ReadByte(addr + 1)|(FLASH_ReadByte(addr) << 8);
	if(value == 0xffff || value == 0) {
		return defaultValue;
	}	
	return value;
}

void eepromWriteU16(uint16_t addr, uint16_t data) {
	FLASH_Unlock(FLASH_MEMTYPE_DATA);
	FLASH_ProgramByte(addr, (uint8_t)(data >> 8));
	FLASH_ProgramByte(addr + 1, (uint8_t)data);
	while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
	FLASH_Lock(FLASH_MEMTYPE_DATA);
}