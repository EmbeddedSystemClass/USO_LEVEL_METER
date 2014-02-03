
#include "keyboard.h"
//#include "lcd.h"
#include <stdio.h>
#include "menu.h"
#include <intrins.h>
#include "watchdog.h"

sbit BIP=P0^7;

unsigned char lastKey,prevKey;

volatile struct pt pt_keyboard;
volatile unsigned char key_code=0x0;

sbit LED=P0^6;

unsigned char Key_Ask(void);
void Beep(unsigned char time);

unsigned char beep_counter;

void Beep(unsigned char time)
{
   beep_counter=time;
   BIP=0;
}

PT_THREAD(KeyboardProcess(struct pt *pt))
 {

//  static unsigned char i=0;
 // static unsigned int  key_code_1=0xFFFF,key_code_2=0xFFFF, last_key_code=0xFFFF;
  static unsigned char key_1, key_2, last_key=0;

  PT_BEGIN(pt);
  wdt_count[Key_Proc].process_state=RUN;
  while(1) 
  {  	
		PT_DELAY(pt,5);
		key_1= Key_Ask();
		PT_DELAY(pt,5);
		key_2= Key_Ask();			

		if((key_1==key_2)&&(key_1!=last_key))
		{
			last_key=key_1;	
			key_code=key_1;
			
			if(key_1!=0)
			{
				menuKey(key_code);
//				BIP=0;
//				PT_DELAY(pt,1);
//				BIP=1;
//				PT_DELAY(pt,1);
//				BIP=0;
//				PT_DELAY(pt,1);
//				BIP=1;
//				PT_DELAY(pt,1);
//				BIP=0;
//				PT_DELAY(pt,1);
//				BIP=1;
				Beep(2);
			}			
		}
	wdt_count[Key_Proc].count++;	
  }

  PT_END(pt);
 }

 unsigned char Key_Ask(void)
 {
 	 KB_PI&=~KB_MASK;
	 KB_PO|=KB_MASK;
	 KB_PO&=(~(1<<0));	

	 switch(KB_PI&KB_MASK)
	 {
	 	case 0x1E:{return '0';}break;
		case 0x1D:{return '.';}break;
		case 0x1B:{return 'P';}break;
		case 0x17:{return '+';}break;
		case 0x0F:{return '-';}break;
	 }
//
 	 KB_PI&=~KB_MASK;
	 KB_PO|=KB_MASK;
	 KB_PO&=(~(1<<1));	

	 switch(KB_PI&KB_MASK)
	 {
	 	case 0x1E:{return '1';}break;
		case 0x1D:{return '2';}break;
		case 0x1B:{return '3';}break;
		case 0x17:{return 'x';}break;
		case 0x0F:{return '/';}break;
	 }

 	 KB_PI&=~KB_MASK;
	 KB_PO|=KB_MASK;
	 KB_PO&=(~(1<<2));	

	 switch(KB_PI&KB_MASK)
	 {
	 	case 0x1E:{return '4';}break;
		case 0x1D:{return '5';}break;
		case 0x1B:{return '6';}break;
		case 0x17:{return '=';}break;
		case 0x0F:{return '>';}break;
	 }

 	 KB_PI&=~KB_MASK;
	 KB_PO|=KB_MASK;
	 KB_PO&=(~(1<<3));	

	 switch(KB_PI&KB_MASK)
	 {
	 	case 0x1E:{return '7';}break;
		case 0x1D:{return '8';}break;
		case 0x1B:{return '9';}break;
		case 0x17:{return '[';}break;
		case 0x0F:{return ']';}break;
	 }
//
 	 KB_PI&=~KB_MASK;
	 KB_PO|=KB_MASK;
	 KB_PO&=(~(1<<4));	

	 switch(KB_PI&KB_MASK)
	 {
	 	case 0x1E:{return 'C';}break;
		case 0x1D:{return 'B';}break;
		case 0x1B:{return 'H';}break;
		case 0x17:{return 'F';}break;
		case 0x0F:{return 'A';}break;
	 }

	 return 0;
 }
