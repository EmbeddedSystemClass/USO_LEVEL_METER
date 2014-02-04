//#include "globals.h"
#include "menu.h"
#include "keyboard.h"
#include <stdio.h>
#include <intrins.h>

#include "proto_uso/proto_uso.h"
#include "proto_uso/channels.h"
#include <string.h>
#include "calibrate/calibrate.h"
#include "watchdog.h"
#include "device.h"

#define DISPLAY_WIDTH 	20
#define DISPLAY_HEIGHT	4

extern struct Channel xdata channels[CHANNEL_NUMBER];//обобщенная структура каналов
extern volatile unsigned char xdata ADRESS_DEV;

volatile struct pt pt_display;

//sbit LED=P0^6;


enum
{
	DYN_NOT_DISPLAY=0,
	DYN_DISPALY_ON =1
};


bit flag_menu_entry=0;//вошли в меню

typedef struct {
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Child;
	unsigned char     Select;
	char 		 *Text;
} code menuItem;

xdata menuItem code* selectedMenuItem; // текущий пункт меню

menuItem code* menuStack[10];
volatile unsigned char menuStackTop;

char buf[20];
unsigned char dynamic_disp=0;//номер отображаемого динамического экрана

#define MAKE_MENU(Name, Next, Previous, Parent, Child, Select, Text) \
    extern menuItem Next;     \
	extern menuItem Previous; \
	extern menuItem Parent;   \
	extern menuItem Child;  \
	menuItem Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Child, (unsigned char)Select, { Text }}

#define PREVIOUS   (void*)(selectedMenuItem->Previous)
#define NEXT       (void*)(selectedMenuItem->Next)
#define PARENT     (void*)(selectedMenuItem->Parent)
#define CHILD      (void*)(selectedMenuItem->Child)
#define SELECT	   (void*)(selectedMenuItem->Select)

 char code strNULL[]  = "";

#define NULL_ENTRY Null_Menu
menuItem        Null_Menu = {(void*)0, (void*)0, (void*)0, (void*)0, 0, {0x00}};

//                 NEXT,      PREVIOUS     PARENT,     CHILD
MAKE_MENU(m_s0i1,  NULL_ENTRY,NULL_ENTRY,  NULL_ENTRY, m_s1i1,       0, 	"DATA SCREEN");


MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  m_s0i1,     m_s2i1,       0, 					"5Et");
MAKE_MENU(m_s1i2,  NULL_ENTRY,m_s1i1,      m_s0i1,     m_s3i1,       MENU_SET_BRIGHTNESS,   "brI");

// подменю Настройка каналов
MAKE_MENU(m_s2i1,  m_s2i2,    NULL_ENTRY,  m_s1i1,     m_s3i1,       MENU_CHN1_SET, 		"Ch1");
MAKE_MENU(m_s2i2,  m_s2i3,    m_s2i1,      m_s1i1,     m_s4i1,   	 MENU_CHN2_SET, 		"Ch2");
MAKE_MENU(m_s2i3,  m_s2i4,	  m_s2i2,      m_s1i1,     m_s5i1,   	 MENU_CHN3_SET, 		"Ch3");
MAKE_MENU(m_s2i4,  m_s2i5,	  m_s2i3,      m_s1i1,     m_s6i1,   	 MENU_CHN4_SET, 		"Ch4");
MAKE_MENU(m_s2i5,  m_s2i6,	  m_s2i4,      m_s1i1,     m_s7i1,   	 MENU_CHN5_SET, 		"Ch5");
MAKE_MENU(m_s2i6,  NULL_ENTRY,m_s2i5,      m_s1i1,     m_s8i1,   	 MENU_CHN6_SET, 		"Ch6");


// Настройка канала	 1
MAKE_MENU(m_s3i1,  m_s3i2,    NULL_ENTRY,  m_s2i1,     NULL_ENTRY,   MENU_CHN1_CAL_HI, 		"ChI");
MAKE_MENU(m_s3i2,  m_s3i3,    m_s3i1,      m_s2i1,     NULL_ENTRY,   MENU_CHN1_CAL_LO, 		"CLO");
MAKE_MENU(m_s3i3,  m_s3i4,	  m_s3i2,      m_s2i1,     NULL_ENTRY,   MENU_CHN1_UST_HI, 		"UhI");
MAKE_MENU(m_s3i4,  NULL_ENTRY,m_s3i3,      m_s2i1,     NULL_ENTRY,   MENU_CHN1_UST_LO, 		"ULO");
// Настройка канала	 2
MAKE_MENU(m_s4i1,  m_s4i2,    NULL_ENTRY,  m_s2i2,     NULL_ENTRY,   MENU_CHN2_CAL_HI, 		"ChI");
MAKE_MENU(m_s4i2,  m_s4i3,    m_s4i1,      m_s2i2,     NULL_ENTRY,   MENU_CHN2_CAL_LO, 		"CLO");
MAKE_MENU(m_s4i3,  m_s4i4,	  m_s4i2,      m_s2i2,     NULL_ENTRY,   MENU_CHN2_UST_HI, 		"UhI");
MAKE_MENU(m_s4i4,  NULL_ENTRY,m_s4i3,      m_s2i2,     NULL_ENTRY,   MENU_CHN2_UST_LO, 		"ULO");
// Настройка канала	 3
MAKE_MENU(m_s5i1,  m_s5i2,    NULL_ENTRY,  m_s2i3,     NULL_ENTRY,   MENU_CHN3_CAL_HI, 		"ChI");
MAKE_MENU(m_s5i2,  m_s5i3,    m_s5i1,      m_s2i3,     NULL_ENTRY,   MENU_CHN3_CAL_LO, 		"CLO");
MAKE_MENU(m_s5i3,  m_s5i4,	  m_s5i2,      m_s2i3,     NULL_ENTRY,   MENU_CHN3_UST_HI, 		"UhI");
MAKE_MENU(m_s5i4,  NULL_ENTRY,m_s5i3,      m_s2i3,     NULL_ENTRY,   MENU_CHN3_UST_LO, 		"ULO");
// Настройка канала	 4
MAKE_MENU(m_s6i1,  m_s6i2,    NULL_ENTRY,  m_s2i4,     NULL_ENTRY,   MENU_CHN4_CAL_HI, 		"ChI");
MAKE_MENU(m_s6i2,  m_s6i3,    m_s6i1,      m_s2i4,     NULL_ENTRY,   MENU_CHN4_CAL_LO, 		"CLO");
MAKE_MENU(m_s6i3,  m_s6i4,	  m_s6i2,      m_s2i4,     NULL_ENTRY,   MENU_CHN4_UST_HI, 		"UhI");
MAKE_MENU(m_s6i4,  NULL_ENTRY,m_s6i3,      m_s2i4,     NULL_ENTRY,   MENU_CHN4_UST_LO, 		"ULO");
// Настройка канала	 5
MAKE_MENU(m_s7i1,  m_s7i2,    NULL_ENTRY,  m_s2i5,     NULL_ENTRY,   MENU_CHN5_CAL_HI, 		"ChI");
MAKE_MENU(m_s7i2,  m_s7i3,    m_s7i1,      m_s2i5,     NULL_ENTRY,   MENU_CHN5_CAL_LO, 		"CLO");
MAKE_MENU(m_s7i3,  m_s7i4,	  m_s7i2,      m_s2i5,     NULL_ENTRY,   MENU_CHN5_UST_HI, 		"UhI");
MAKE_MENU(m_s7i4,  NULL_ENTRY,m_s7i3,      m_s2i5,     NULL_ENTRY,   MENU_CHN5_UST_LO, 		"ULO");
// Настройка канала	 6
MAKE_MENU(m_s8i1,  m_s8i2,    NULL_ENTRY,  m_s2i6,     NULL_ENTRY,   MENU_CHN6_CAL_HI, 		"ChI");
MAKE_MENU(m_s8i2,  m_s8i3,    m_s8i1,      m_s2i6,     NULL_ENTRY,   MENU_CHN6_CAL_LO, 		"CLO");
MAKE_MENU(m_s8i3,  m_s8i4,	  m_s8i2,      m_s2i6,     NULL_ENTRY,   MENU_CHN6_UST_HI, 		"UhI");
MAKE_MENU(m_s8i4,  NULL_ENTRY,m_s8i3,      m_s2i6,     NULL_ENTRY,   MENU_CHN6_UST_LO, 		"ULO");



unsigned char string_buf[32];
//unsigned char input_char_count;
#define INPUT_CHAR_BUF_LEN	6
//
//unsigned char *input_char_buf;
//unsigned char input_char_buf_lo[INPUT_CHAR_BUF_LEN+1];
//unsigned char input_char_buf_hi[INPUT_CHAR_BUF_LEN+1];

enum
{
	CAL_FLOAT_LO=0,
	CAL_FLOAT_HI
};

//struct input_field
//{
//	unsigned char input_char_buf[2*INPUT_CHAR_BUF_LEN+1];
//	unsigned char char_count;
//	unsigned char has_point;
//};
//
//volatile struct input_field input_field_lo={"      ",0,0};
//volatile struct input_field input_field_hi={"      ",0,0};
//
//volatile struct input_field *input_field_ptr;


unsigned char menuHandler(menuItem* currentMenuItem,unsigned int key);	 //обработка меню
void CalibrationKey(unsigned char key,unsigned char channel);
void CalibrationScreen(unsigned char channel);//экран калибровки канала

void SettingsKey(unsigned char key);
void SettingsScreen(void);


void menuChange(menuItem code* NewMenu)
{
	if (NewMenu == &NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}

unsigned char dispMenu(void)
{
	unsigned char i=0;

	if (selectedMenuItem == &m_s0i1) 
	{ // мы на верхнем уровне
		dynamic_disp= DYN_DISPALY_ON;
	} 
	else 
	{
		dynamic_disp=DYN_NOT_DISPLAY;


	    for(i=0;i<CHANNEL_NUMBER;i++)
	    {
			sprintf(channels[i].string_buf,"   ");
		}
	//	sprintf(channels[0].string_buf,(const unsigned char *)selectedMenuItem->Text);
		memcpy(channels[0].string_buf,(const unsigned char *)selectedMenuItem->Text,3);
		Tablo_Output_Frame();
	}
	return (1);
}

unsigned char menuKey(unsigned char key) 
{
	menuItem* sel;


	if(!flag_menu_entry)
	{
		switch (key) 
		{
			case 0: 
			{
				return 1;
			}
			break;
		
			case '+': 
			{
				menuChange(PREVIOUS);
	
			}
			break;
		
			case '>': 
			{
				menuChange(NEXT);			
			}
			break;
		
		
			case 'E':
			{ // выбор пункта					
					sel = selectedMenuItem->Select;//SELECT;
					if (selectedMenuItem->Select != 0) 
					{
						menuHandler(selectedMenuItem,key);	
						return (1);
					} 
					else 
					{
						menuChange(CHILD);
					}
			}
			break;
	
			case 'Q': 
			{ // отмена выбора (возврат)
				menuChange(PARENT);
			}
			break;
	
			default:
			{
	
			}		
		} 
	
		if(key!=0)
	   	{
			dispMenu(); 
		}
	}
	else
	{
		if(key== 'Q') 
		{ // отмена выбора (возврат)
	
			flag_menu_entry=0;
//			dynamic_disp=DYN_NOT_DISPLAY;

			dispMenu(); 
		}
		else
		{
			sel = SELECT;
			if (sel != 0) 
			{
				menuHandler(selectedMenuItem,key);
			}			 
		}
	}
	return (1);
}
//-----------------------------------------------------

unsigned char menuHandler(menuItem* currentMenuItem,unsigned int key)	 //обработка меню
{
	flag_menu_entry=1;
	
	switch (currentMenuItem->Select) 
	{
//		case MENU_DEV_SET:
//		{	
//			SettingsKey(key);
//		}
//		break; 
//
//		case MENU_CHN1_CAL:
//		{
//			CalibrationKey(key,0);	
//		}
//		break;
//
//		case MENU_CHN2_CAL:
//		{
//			CalibrationKey(key,1);	
//		}
//		break;
//
//		case MENU_CHN3_CAL:
//		{
//			CalibrationKey(key,2);	
//		}
//		break;
//
//		case MENU_CHN4_CAL:
//		{
//			CalibrationKey(key,3);	
//		}
//		break;
//
//		case MENU_CHN5_CAL:
//		{
//			CalibrationKey(key,4);	
//		}
//		break;
//
//		case MENU_CHN6_CAL:
//		{
//			CalibrationKey(key,5);	
//		}
//		break;
//
//		case MENU_CHN7_CAL:
//		{
//			CalibrationKey(key,6);	
//		}
//		break;
//
//		case MENU_CHN8_CAL:
//		{
//			CalibrationKey(key,7);	
//		}
//		break;
//
//		case MENU_CHN_DOL_CAL:
//		{
//			CalibrationKey(key,11);	
//		}
//		break;
//
//		case MENU_CHN_FREQ1_CAL:
//		{
//			CalibrationKey(key,8);	
//		}
//		break;
//
//		case MENU_CHN_FREQ2_CAL:
//		{
//			CalibrationKey(key,9);	
//		}
//		break;
//
//		case MENU_CHN_FREQ3_CAL:
//		{
//			CalibrationKey(key,10);	
//		}
//		break;
//
//		case MENU_CHN_FREQ_HI_CAL:
//		{
//			CalibrationKey(key,12);	
//		}
//		break;
	}	
	return 0;
}
//-----------------------------------------------------
unsigned char startMenu(void)
 {
	selectedMenuItem = &m_s0i1;

	dispMenu();
	PT_INIT(&pt_display);
	return 0;
}
//-------------------------------------------------------
//void CalibrationKey(unsigned char key,unsigned char channel)
//{
//		switch(key)
//		{
//			case 'F':
//			{				
//				//LCD_WriteCommand(LCD_CMD_CLEAR);
//
//				dynamic_disp= DYN_DISPALY_ON;
//
//				input_field_hi.char_count=INPUT_CHAR_BUF_LEN;
//				input_field_lo.char_count=INPUT_CHAR_BUF_LEN;
//
//				sprintf(input_field_hi.input_char_buf,"%5.4f",channels[channel].calibrate.cal.cal_hi);
//				sprintf(input_field_lo.input_char_buf,"%5.4f",channels[channel].calibrate.cal.cal_lo);	
//				
//				if(strchr (input_field_hi.input_char_buf,'.'))
//				{
//					input_field_hi.has_point=1;
//				}			
//				else
//				{
//					input_field_hi.has_point=0;
//				}
//
//				if(strchr (input_field_lo.input_char_buf,'.'))
//				{
//					input_field_lo.has_point=1;
//				}			
//				else
//				{
//					input_field_lo.has_point=0;
//				}
//
//				input_field_hi.input_char_buf[INPUT_CHAR_BUF_LEN]=0;
//				input_field_lo.input_char_buf[INPUT_CHAR_BUF_LEN]=0;
//
//				input_field_ptr=&input_field_lo;
//
//			}
//			break;
//
//			case '[':
//			{
//				input_field_ptr=&input_field_lo;
//			}
//			break;
//
//			case ']':
//			{
//				input_field_ptr=&input_field_hi;
//			}
//			break;
//
//			case '=':
//			{
//				if((input_field_ptr->input_char_buf[0]!=' ')/*&&(input_field_ptr->input_char_buf[0]!='-')*/)
//				{
//					if(input_field_ptr==&input_field_lo)
//					{	
//						sscanf(input_field_lo.input_char_buf,"%f",&channels[channel].calibrate.cal.cal_lo);
//						SetFirstPoint(channel,channels[channel].channel_data,channels[channel].calibrate.cal.cal_lo);
//					}
//					else
//					{
//						sscanf(input_field_hi.input_char_buf,"%f",&channels[channel].calibrate.cal.cal_hi);
//						SetSecondPoint(channel,channels[channel].channel_data,channels[channel].calibrate.cal.cal_hi);
//					}
//				}
//			}
//			break;
//
//			case '/':
//			{
//				if(input_field_ptr->char_count)
//				{
//					input_field_ptr->input_char_buf[input_field_ptr->char_count-1]=' ';
//					input_field_ptr->char_count--;
//				}
//				
//				if(strchr (input_field_ptr->input_char_buf,'.'))
//				{
//					input_field_ptr->has_point=1;
//				}			
//				else
//				{
//					input_field_ptr->has_point=0;
//				}				
//			}
//			break;
//		}
//
//		if(input_field_ptr->char_count<INPUT_CHAR_BUF_LEN)
//		{
//			if((key>='0') && (key<='9'))
//			{
//				   input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
//				   input_field_ptr->char_count++;
//			}
//			
//			if(key=='.')
//			{
//				   if((input_field_ptr->has_point==0)&&(input_field_ptr->char_count!=0)&&(input_field_ptr->char_count<(INPUT_CHAR_BUF_LEN)))
//				   {
//				   	   input_field_ptr->input_char_buf[input_field_ptr->char_count]='.';
//				   	   input_field_ptr->char_count++;
//					   input_field_ptr->has_point=1;
//				   }			
//			}
//	
//			if(key=='-')
//			{
//				   if(input_field_ptr->char_count==0)
//				   {
//					   input_field_ptr->input_char_buf[input_field_ptr->char_count]='-';
//					   input_field_ptr->char_count++;
//				   }			
//			}
//		}
//		else
//		{
//			 input_field_ptr->char_count=INPUT_CHAR_BUF_LEN/*-1*/;
//		}
//		CalibrationScreen(channel);		
//}
//
//
//void CalibrationScreen(unsigned char channel)//экран калибровки канала
//{
////   sprintf(&string_buf,"Chn. val.:% 8lu",channels[channel].channel_data);
////   //LCD_WriteAC(LCD_1_STR_ADDR);
////   //LCD_WriteString(&string_buf);
//
// 	if(input_field_ptr==&input_field_lo)
//	{
//			//LCD_WriteAC(LCD_1_STR_ADDR);
//			sprintf(&string_buf,"Low val [%6s]",input_field_lo.input_char_buf);
//			//LCD_WriteString(&string_buf);
//
//			//LCD_WriteAC(LCD_2_STR_ADDR);
//			sprintf(&string_buf,"High val %6s ",input_field_hi.input_char_buf);
//			//LCD_WriteString(&string_buf);
//
//			//LCD_WriteAC(LCD_4_STR_ADDR);
//			sprintf(&string_buf,"'='-Enter,'/'-Del");
//			//LCD_WriteString(&string_buf);
//
//			//LCD_WriteAC(LCD_1_STR_ADDR+9+input_field_lo.char_count);
//			//LCD_WriteCommand(LCD_CMD_CURSOR);
//	}
//
//	if(input_field_ptr==&input_field_hi)
//	{
//			//LCD_WriteAC(LCD_1_STR_ADDR);
//			sprintf(&string_buf,"Low val  %6s ",input_field_lo.input_char_buf);
//			//LCD_WriteString(&string_buf); 
//
//			//LCD_WriteAC(LCD_2_STR_ADDR);
//			sprintf(&string_buf,"High val[%6s]",input_field_hi.input_char_buf);
//			//LCD_WriteString(&string_buf);
//
//			//LCD_WriteAC(LCD_4_STR_ADDR);
//			sprintf(&string_buf,"'='-Enter,'/'-Del");
//			//LCD_WriteString(&string_buf);
//
//			//LCD_WriteAC(LCD_2_STR_ADDR+9+input_field_hi.char_count);
//			//LCD_WriteCommand(LCD_CMD_CURSOR);
//	}
//}
//
//
//void SettingsKey(unsigned char key)
//{
//		switch(key)
//		{
//			case 'F':
//			{				
//				//LCD_WriteCommand(LCD_CMD_CLEAR);
//
////				input_field_hi.char_count=2;
//				input_field_lo.char_count=2;
//
////				sprintf(input_field_hi.input_char_buf,"%02u",channels[channel].calibrate.cal.cal_hi);
//				sprintf(input_field_lo.input_char_buf,"%02bu",ADRESS_DEV);	
//				
//
////				input_field_hi.input_char_buf[2]=0;
//				input_field_lo.input_char_buf[2]=0;
//
//				input_field_ptr=&input_field_lo;
//
//			}
//			break;
//
////			case '[':
////			{
////				input_field_ptr=&input_field_lo;
////			}
////			break;
////
////			case ']':
////			{
////				input_field_ptr=&input_field_hi;
////			}
////			break;
//
//			case '=':
//			{
//				if((input_field_ptr->input_char_buf[0]!=' ')/*&&(input_field_ptr->input_char_buf[0]!='-')*/)
//				{
//					if(input_field_ptr==&input_field_lo)
//					{	
//						sscanf(input_field_lo.input_char_buf,"%bu",&ADRESS_DEV);
//						//SetFirstPoint(channel,channels[channel].channel_data,channels[channel].calibrate.cal.cal_lo);
//						Device_Save_Address(ADRESS_DEV);
//					}
//				}
//			}
//			break;
//
//			case '/':
//			{
//				if(input_field_ptr->char_count)
//				{
//					input_field_ptr->input_char_buf[input_field_ptr->char_count-1]=' ';
//					input_field_ptr->char_count--;
//				}				
//			}
//			break;
//		}
//
//		if(input_field_ptr->char_count<2)
//		{
//			if((key>='0') && (key<='9'))
//			{
//				   if(input_field_ptr->char_count==0)
//				   {
//				   		if((key=='0')||(key=='1'))
//						{
//						   input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
//						   input_field_ptr->char_count++;							
//						}
//				   }
//				   else
//				   {
//					   if(input_field_ptr->char_count==1)
//					   {
//					   		   if(input_field_ptr->input_char_buf[0]=='0')
//							   {
//								   		input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
//								   		input_field_ptr->char_count++;							   		
//							   }
//							   else
//							   {
//								   if(((key>='0') && (key<='5')))
//								   {
//								   		input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
//								   		input_field_ptr->char_count++;
//								   }
//							   }
//					   }
//				   }
//			}
//		}
//		else
//		{
//			 input_field_ptr->char_count=2;
//		}
//		SettingsScreen();		
//}
//
//void SettingsScreen(void)
//{
// 	if(input_field_ptr==&input_field_lo)
//	{
//			//LCD_WriteAC(LCD_1_STR_ADDR);
//			sprintf(&string_buf,"Address [%2s]",input_field_lo.input_char_buf);
//			//LCD_WriteString(&string_buf);
//
//			//LCD_WriteAC(LCD_4_STR_ADDR);
//			sprintf(&string_buf,"'='-Enter,'/'-Del");
//			//LCD_WriteString(&string_buf);
//
//			//LCD_WriteAC(LCD_1_STR_ADDR+9+input_field_lo.char_count);
//			//LCD_WriteCommand(LCD_CMD_CURSOR);
//	}	
//}

PT_THREAD(DisplayProcess(struct pt *pt))
{

  unsigned char i=0;
  float value;
  PT_BEGIN(pt);

  while(1) 
  {
	wdt_count[Display_Proc].process_state=IDLE;  
 	PT_YIELD_UNTIL(pt,dynamic_disp); //ждем команды на старт	
	 
  	wdt_count[Display_Proc].process_state=RUN;
  	PT_DELAY(pt,20);

   for(i=0;i<CHANNEL_NUMBER;i++)
   {
		//value=GetCalibrateVal(i,channels[i].channel_data);
		  value= (float)channels[i].channel_data/0xFFFFFF*10.225;
	 
		  if(_chkfloat_ (value)>1) 
		  {
			   sprintf(channels[i].string_buf,"Err");
		  }
		  else  
		  {
			   sprintf(channels[i].string_buf,"%3.2f",value);
		  }
	}

	Tablo_Output_Frame();

	wdt_count[Display_Proc].count++;
  }

  PT_END(pt);
 }
