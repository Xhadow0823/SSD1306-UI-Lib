#ifndef __EEPROM_AGENT_H__
  #define __EEPROM_AGENT_H__
#include <Arduino.h>
#include <EEPROM.h>

// ========== eeprom information ==========
// total: 512 bytes = 2^9 bytes = 0b0 00000000 ~ 0b1 11111111 = 0x000 ~ 0x1FF
// reserved for system: 0x000~0x03F (64 bytes (0x40))
// ========== eeprom information end ==========

// ========== eeprom block usage ==========
// block size for a app: 32 bytes (0x20)
#define EEPROM_POMODORO_ADDRESS_START_AT (0x40) // 0x040~(0x040+0x20)
#define EEPROM_POMODORO_ADDRESS_END_AT (0x60)   // 0x040~(0x040+0x20)
// ========== eeprom block usage end ==========


#endif
