#include <ADuC845.h>


#include "timer3.h"
#include "proto_uso/proto_uso.h"
#include "preferences.h"
#include "eeprom/eeprom.h"
#include "timer1.h"
#include "adc.h"
#include "watchdog.h"
#include "proto_uso/channels.h"
#include "frequency.h"

#include "menu.h"
#include "keyboard.h"
#include "ulongsort.h"

#include "calibrate/calibrate.h"
#include "pt/pt.h"



//extern unsigned char idata i2c_buffer[6];

//extern struct I2C_Channel xdata i2c_channels;
extern struct pt pt_proto, pt_wdt,pt_display,pt_keyboard;
volatile struct pt /*pt_i2c_read, pt_freq_measure,*/pt_sort;//,pt_i2c_process;


//-----------------------------------------
//void Set_Handlers(void);//установить обработчики событий
//PT_THREAD(I2C_RepeatRead(struct pt *pt));
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	
	PLLCON&=PLLCON_VAL;//настройка частоты процессора
	
//	RestoreCalibrate();
	
	ChannelsInit();//инициализаци€ настроек каналов
	Protocol_Init();	
	Timer1_Initialize(); //таймер шедулера 200√ц	
	ADC_Initialize();
	UART_Init();


//	WDT_Init(WDT_2000);//включить сторожевой таймер

	PT_INIT(&pt_sort);

	startMenu();
	EA=1;


	while(1)
	{	
//		ProtoProcess(&pt_proto);
	
		ulongsort_process(&pt_sort);
		DisplayProcess(&pt_display);  
//		WDT_Process(&pt_wdt);
		KeyboardProcess(&pt_keyboard);	    
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------
void Timer1_Interrupt(void) interrupt 3  //таймер шедулера
{
//---------------------------------------
	TH1	= TH1_VAL; ///200 Hz;
	TL1 = TL1_VAL;//
	pt_display.pt_time++;;
	pt_sort.pt_time++;
//	pt_proto.pt_time++;
	pt_wdt.pt_time++;
	pt_keyboard.pt_time++;
	return;	
}