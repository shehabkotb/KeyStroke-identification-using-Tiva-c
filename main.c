#include "tm4c123gh6pm.h"
#include <stdio.h>  
#include <stdint.h> 
#include <String.h>
#include <stdlib.h> 
#include <stdbool.h>
#include "globals.h"
#include "Systick_Timer.h"
#include "KeyStroke.h"
#include "DecisionTree.h"

#include "driverlib/sysctl.h"
#include "driverlib/eeprom.h"

int main(){

	Uart_init();
	Port_init();
	oneShotConfigure();
	Configure_SysTick();
	EEPROM_init();
	
	while(1){

		EEPROMRead((uint32_t*)&read, 0x400, sizeof(read));

		PrintString("choose one of the following \n\r");
		PrintString("1) train a new user \n\r");
		PrintString("2) predict a user \n\r");
		
		int choice = getchoice();
		
		if(choice == 1){
			PrintString("choose the user to retrain \n\r");
			
			PrintString("1) ");
			PrintString((char*)read.users[0]);
			PrintString("\n\r");
			PrintString("2) ");
			PrintString((char*)read.users[1]);
			PrintString("\n\r");
			int user = getchoice();
			train(user,&read);
		}
		else if(choice == 2){
			
			struct Node root = tree();
			unsigned short record[WORDLENGTH-1] = {0};
			int valid = getRecord(record);
			if(valid != -1){		
				struct Prediction p = predict(record, root, 2);
				int user = p.label;
				user--;
				PrintString("Prediction: ");
				PrintString((char*)&read.users[user]);
				PrintString("\n\r");
			}
		}
	}	
}

//	EEPROM write = {{"shehab", "hazem"}, {0}};
//	memcpy(write.data, dataset, sizeof(dataset));
//	EEPROMProgram((uint32_t*)&write, 0x400, sizeof(write));

//unsigned short dataset[20][10] = {{164,175,125,926,692,166,135,289,595,1},
//						{175,210,303,225,352,136,174,2206,537,1},
//						{210,170,93,229,153,565,143,261,569,1},
//						{174,187,112,591,208,146,124,240,536,1},
//						{180,177,139,1213,426,163,201,267,446,1},
//						{153,180,107,632,218,140,111,398,705,1},
//						{150,208,127,240,136,115,117,259,626,1},
//						{244,183,101,181,132,138,133,395,621,1},
//						{137,179,81,250,272,141,194,433,496,1},
//						{912,182,341,217,128,114,117,233,530,1},
//						{141,207,328,241,356,263,617,169,539,2},
//						{165,194,223,219,415,261,266,339,674,2},
//						{165,193,425,205,1063,239,448,234,366,2},
//						{159,173,238,242,534,235,247,356,175,2},
//						{174,197,270,243,494,256,576,170,270,2},
//						{218,186,317,199,424,250,251,346,194,2},
//						{452,193,264,209,552,229,288,220,258,2},
//						{266,208,287,255,602,206,199,196,231,2},
//						{151,200,292,273,426,209,5507,170,333,2},
//						{146,207,173,235,226,228,205,245,220,2}
//}; 
