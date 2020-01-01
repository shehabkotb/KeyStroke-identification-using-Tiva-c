#ifndef __GLOBALS__
#define __GLOBALS__

#include <stdint.h>

#define WORDLENGTH (10)
#define WORD "voldemortx"
#define ENTRIES (2)


extern struct EEPROM{
	uint8_t users[2][12];
	unsigned short data[20][WORDLENGTH];
};

typedef struct EEPROM EEPROM;

extern unsigned short dataset[20][10];

extern EEPROM read;

#endif