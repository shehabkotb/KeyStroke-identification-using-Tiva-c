#include "tm4c123gh6pm.h"

void Configure_SysTick(void){

  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x05;      // enable SysTick with core clock

}

void SysTick_Wait1ms(unsigned long delay){
	
	for(unsigned long i=0; i<delay; i++){
		NVIC_ST_RELOAD_R = 16000-1;  // number to count 1ms
		NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
		
		while((NVIC_ST_CTRL_R&0x00010000)==0){ }
		
	}
}

void SysTick_Wait1us(unsigned long delay){
	
	for(unsigned long i=0; i<delay; i++){
		NVIC_ST_RELOAD_R = 16-1;  // number to count 1ms
		NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
		
		while((NVIC_ST_CTRL_R&0x00010000)==0){ }
		
	}
 }