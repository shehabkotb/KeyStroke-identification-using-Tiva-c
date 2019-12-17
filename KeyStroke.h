#include "tm4c123gh6pm.h"
//#include "Systick_Timer.h"



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

void printChar(char c) {
	while((UART0_FR_R & (1<<5)) != 0);
	UART0_DR_R = c;
}

void PrintString(char* string) {
	while(*string) {
		printChar(*(string++));
	}
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

char waitForFirstInput(){
	char c;
	while((UART0_FR_R & (1<<4)) != 0);
	TIMER0_CTL_R |= 0x03;
	TIMER0_TAV_R = 0;
	c = UART0_DR_R;
	return c;

}

void oneShotConfigure(void){
	
	SYSCTL_RCGCTIMER_R |= 0x01;
    TIMER0_CTL_R = 0x0;
    TIMER0_CFG_R = 0x0; 
    TIMER0_TAMR_R = 0x11;
    TIMER0_TAILR_R = 0xffffffff;                     
}

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

char getDelay(int* d, int len) {
	char c;
	while((UART0_FR_R & (1<<4)) != 0);
	d[len] = TIMER0_TAV_R * 0.0000625;
	TIMER0_TAV_R = 0;
	c = UART0_DR_R;
	return c;
}

void printInt(int i){
	
	char s[10];
	itoa(i,s);
	PrintString(s);
}