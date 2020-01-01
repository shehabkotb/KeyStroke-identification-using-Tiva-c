#include "tm4c123gh6pm.h"
#include "globals.h"

void Uart_init(void);
void Port_init(void);
void EEPROM_init(void);
void oneShotConfigure(void);
void blinkRed(void);
void blinkGreen(void);

void reverse(char s[]);
void itoa(int n, char s[]);
void printChar(char c);
void PrintString(char* string);
void printInt(int i);

char waitForFirstInput();
char getDelay(unsigned short* d);
int getchoice(void);
char getUartchar(void) ;
void printDelays(unsigned short delays[20][WORDLENGTH]);

int getRecord(unsigned short rec[WORDLENGTH-1]);
void train(int user, EEPROM* memory);
