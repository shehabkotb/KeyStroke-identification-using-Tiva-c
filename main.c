#include "tm4c123gh6pm.h"
#include <stdio.h>  
#include <stdint.h> 
#include <String.h>
#include <stdlib.h> 
#include "Systick_Timer.h"
#include "KeyStroke.h"
#include "DTC.h"

//#define WORDLENGTH (10)
//#define WORD ".tie4Ronal"
//#define ENTRIES (10)

#define WORDLENGTH (10)
#define WORD "voldemortx"
#define ENTRIES (10)

unsigned short dataset[20][11] = {{164,175,125,926,692,166,135,289,595,1},
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






//unsigned short** copy(){
//	unsigned short **arr = (unsigned short **)malloc(20 * sizeof(unsigned short *)); 
//	for (int i=0; i<20; i++) {
//		arr[i] = (unsigned short *)malloc(11 * sizeof(unsigned short)); 
//	}
//	
//	for(int i=0;i<20;i++){
//		for(int j=0;j<11;j++){
//			arr[i][j] = dataset[i][j];
//		}
//	}
//	return arr;
//}
//
//struct Node tree(){
//	unsigned short** z = copy();
//	struct Node x = build_tree(z,20,2);
//
//	return x;
//}


void printDelays(int delays[ENTRIES][WORDLENGTH-1]) {
	for(int i = 0;i < ENTRIES;i++){
		for(int j = 0;j < WORDLENGTH-1;j++){
			printInt(delays[i][j]);
			printChar(',');
		}
		PrintString("\n\r");
	}
}

int main(void) {
	
	int delays[ENTRIES][WORDLENGTH-1] = {-1};
	
	char c;
	Uart_init();
	Port_init();
	oneShotConfigure();
	Configure_SysTick();
  
	while(1){
		
		int j = 0;
		while(j < ENTRIES){
			PrintString("Enter the word "WORD"\n\r");		
			c = waitForFirstInput();
			printChar(c);
			PrintString("\n\r");
			
			char input[WORDLENGTH+1] = {'\0'};
			input[0] = c;
			
			for(int i = 0; i < WORDLENGTH-1; i++) {
				
				c = getDelay(delays[j], i);
				printInt(delays[j][i]);
				input[i+1] = c;
				printChar(c);
				PrintString("\n\r");
			}
			
			PrintString(input);
			PrintString("\n\r");
			
			int res = strcmp(input,WORD);				 
			
			if(res == 0){
				PrintString("accepted");
				PrintString("\n\r");
				blinkGreen();
				j++;
			}
			else{
				PrintString("rejected");
				PrintString("\n\r");
				blinkRed();
			}
		}
		GPIO_PORTF_DATA_R ^= 0x08;
		printDelays(delays);
		
	}
	
//	struct Node x = tree();
//	unsigned short rec[11] = {159,196,212,208,392,453,163,271,523};
//	struct Prediction p = predict(rec, x, 2);
//	printInt(p.label);
//	GPIO_PORTF_DATA_R ^= 0x08;
	
	
	
}
