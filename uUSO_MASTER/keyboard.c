
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

  static unsigned char key_1, key_2, last_key=0;

  PT_BEGIN(pt);
  wdt_count[Key_Proc].process_state=RUN;
  while(1) 
  {  	
		PT_DELAY(pt,50);
		key_1= Key_Ask();
		PT_DELAY(pt,50);
		key_2= Key_Ask();			

		if((key_1==key_2)&&(key_1!=last_key)/*&&(key_1!=0x0)*/)
		{
			last_key=key_1;	
			key_code=key_1;
			
			if(key_1!=0)
			{
				menuKey(key_code);

//				Beep(2);
			}			
		}

//		if((key_1==0)&&(key_2==0))
//		{
//			last_key=0;
//		}
	wdt_count[Key_Proc].count++;	
  }

  PT_END(pt);
 }

 unsigned char Key_Ask(void)
 {
	KEY_1=KEY_2=KEY_3=KEY_4=1;	 //вход с подтяжкой к 1

	if(!KEY_1)
	{
		return 'E';
	}

	if(!KEY_2)
	{
		return 'Q';
	}

	if(!KEY_3)
	{
		return '>';
	}

	if(!KEY_4)
	{
//		return '+';
return 0;
	}

	 return 0;
 }
