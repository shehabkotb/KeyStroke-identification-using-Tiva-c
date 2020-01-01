#include "tm4c123gh6pm.h"
#include <stdbool.h>
#include "KeyStroke.h"
#include "Systick_Timer.h"
#include "string.h"
#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"

unsigned short dataset[20][10] = {{164,175,125,926,692,166,135,289,595,1},
						{175,210,303,225,352,136,174,2206,537,1},
						{210,170,93,229,153,565,143,261,569,1},
						{174,187,112,591,208,146,124,240,536,1},
						{180,177,139,1213,426,163,201,267,446,1},
						{153,180,107,632,218,140,111,398,705,1},
						{150,208,127,240,136,115,117,259,626,1},
						{244,183,101,181,132,138,133,395,621,1},
						{137,179,81,250,272,141,194,433,496,1},
						{912,182,341,217,128,114,117,233,530,1},
						{141,207,328,241,356,263,617,169,539,2},
						{165,194,223,219,415,261,266,339,674,2},
						{165,193,425,205,1063,239,448,234,366,2},
						{159,173,238,242,534,235,247,356,175,2},
						{174,197,270,243,494,256,576,170,270,2},
						{218,186,317,199,424,250,251,346,194,2},
						{452,193,264,209,552,229,288,220,258,2},
						{266,208,287,255,602,206,199,196,231,2},
						{151,200,292,273,426,209,5507,170,333,2},
						{146,207,173,235,226,228,205,245,220,2}
};

EEPROM read = {"","",{0}};



void Uart_init(void){
	SYSCTL_RCGCUART_R |= 0x01; 						// activate UART0
	SYSCTL_RCGCGPIO_R |= 0x01; 						// activate port A
	while((SYSCTL_PRGPIO_R&0x0001) == 0){};			// ready?
	UART0_CTL_R &= ~UART_CTL_UARTEN;      			// disable UART
	UART0_IBRD_R = 104;                     		// baud rate 9600
	UART0_FBRD_R = 11;                    
										
	UART0_LCRH_R = (0x3<<5);						// 8 bit word length (no parity bits, one stop bit, FIFOs)
	UART0_CC_R = 0x0;								// default clock

	UART0_CTL_R |= (1<<0) | (1<<8) | (1<<9);       	// enable UART
}

void Port_init(void){
	GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
	GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0									
	GPIO_PORTA_PCTL_R |= 0x11;			  // configure PA1-0 as UART
	GPIO_PORTA_AMSEL_R &= ~0x03;          // disable analog functionality on PA

	unsigned int delay;
	SYSCTL_RCGC2_R |= (1U << 5);
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_DIR_R |= 0x0e; 			  // leds PF1-3
	GPIO_PORTF_DEN_R |= 0x0e;
}

void oneShotConfigure(void){
	
	SYSCTL_RCGCTIMER_R |= 0x01;
    TIMER0_CTL_R = 0x0;
    TIMER0_CFG_R = 0x0; 
    TIMER0_TAMR_R = 0x11;
    TIMER0_TAILR_R = 0xffffffff;                     
}

void EEPROM_init(void){
	uint32_t ui32EEPROMInit;
		
	// Enable the EEPROM module.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
	
	// Wait for the EEPROM module to be ready.
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0)){}
	
	// Wait for the EEPROM Initialization to complete
	ui32EEPROMInit = EEPROMInit();
	
	// Check if the EEPROM Initialization returned an error
	// and inform the application
	if(ui32EEPROMInit != EEPROM_INIT_OK)
	{
		while(1){}
	}
}

void blinkRed(void){
	for(int i = 0; i < 4; i++) {
		GPIO_PORTF_DATA_R ^= 0x2;
		SysTick_Wait1ms(500);
	}	
}

void blinkGreen(void){
	
	GPIO_PORTF_DATA_R ^= 0x08;
	SysTick_Wait1ms(1000);
	GPIO_PORTF_DATA_R ^= 0x08;

}


void reverse(char s[]) {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}

// convert from integer to c string
void itoa(int n, char s[]) {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void printChar(char c) {
	while((UART0_FR_R & (1<<5)) != 0);
	UART0_DR_R = c;
}

void PrintString(char* string) {
	while(*string) {
		printChar(*(string++));
	}
}

void printInt(int i){
	
	char s[10];
	itoa(i,s);
	PrintString(s);
}

char waitForFirstInput(){
	char c;
	while((UART0_FR_R & (1<<4)) != 0);
	TIMER0_CTL_R |= 0x03;
	TIMER0_TAV_R = 0;
	c = UART0_DR_R;
	return c;

}

char getDelay(unsigned short* d) {
	char c;
	while((UART0_FR_R & (1<<4)) != 0);
	int f = (TIMER0_TAV_R * 0.0000625);
	*d = (unsigned short)f;
	TIMER0_TAV_R = 0;
	c = UART0_DR_R;
	return c;
}

int getchoice(void){
	char c;
	do{
	while((UART0_FR_R & (1<<4)) != 0);
	c = UART0_DR_R;
	}while( (c != '1') && (c != '2'));
	return (c-48);

}

char getUartchar(void) {
	char c;
	while((UART0_FR_R & (1<<4)) != 0);
	c = UART0_DR_R;
	return c;
}

void printDelays(unsigned short delays[20][WORDLENGTH]) {
	for(int i = 0;i < 20;i++){
		for(int j = 0;j < WORDLENGTH;j++){
			printInt(delays[i][j]);
			printChar(',');
		}
		PrintString("\n\r");
	}
}

int getRecord(unsigned short rec[WORDLENGTH-1]){
	char c;
  		
	PrintString("Enter the word "WORD"\n\r");		
	c = waitForFirstInput();
	printChar(c);
	PrintString("\n\r");
	
	char input[WORDLENGTH+1] = {'\0'};
	input[0] = c;
	
	for(int i = 0; i < WORDLENGTH-1; i++) {
		
		c = getDelay((&rec[i]));
		printInt(rec[i]);
		input[i+1] = c;
		printChar(c);
		PrintString("\n\r");
	}
		
	PrintString(input);
	PrintString("\n\r");
	
	int res = strcmp(input,WORD);				 
	
	if(res == 0){
		PrintString("accepted \n\r");
		blinkGreen();
		return 1;
	}
	else{
		PrintString("rejected \n\r");
		blinkRed();
	}
	return -1;
}

void train(int user, EEPROM* memory){
	
	char c;
  	
	user--;
	
	int j = 0;
	while(j < ENTRIES){
		PrintString("Enter the word "WORD"\n\r");		
		c = waitForFirstInput();
		printChar(c);
		PrintString("\n\r");
		
		char input[WORDLENGTH+1] = {'\0'};
		input[0] = c;
		
		for(int i = 0; i < WORDLENGTH-1; i++) {
			
			c = getDelay( &(memory->data[j+(10 * user)][i]) );
			printInt(memory->data[j+(10 * user)][i]);
			input[i+1] = c;
			printChar(c);
			PrintString("\n\r");
		}
		
		PrintString(input);
		PrintString("\n\r");
		
		int res = strcmp(input,WORD);				 
		
		if(res == 0){
			PrintString("accepted \n\r");
			blinkGreen();
			j++;
		}
		else{
			PrintString("rejected \n\r");
			blinkRed();
		}
	}
	PrintString("enter your name \n\r");
	
	for(int k = 0; k < 11; k++){
		char q = getUartchar();
		if(q == 13){
			memory->users[user][k] = '\0';
			break;
		}

		
		memory->users[user][k] = q;

		// fixing backspace		
		if(q == 127){
			printChar((char)127);
			if(k > 2)
				k -= 2;
			else
				k = -1;
		}
		else{
			printChar((char)memory->users[user][k]);
		}
		
		
	}
	PrintString("\n\r");
	PrintString((char*)&memory->users[user]);
	PrintString("\n\r");
	
	EEPROMProgram((uint32_t*)memory, 0x400, sizeof(*memory));	
	
	GPIO_PORTF_DATA_R ^= 0x04;
	SysTick_Wait1ms(500);
	GPIO_PORTF_DATA_R ^= 0x04;
	printDelays(memory->data);
		
	
}
