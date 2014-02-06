//#include "globals.h"
#include "menu.h"
#include "keyboard.h"
#include <stdio.h>
#include <intrins.h>

#include "proto_tablo.h"
#include "proto_uso/channels.h"
#include <string.h>
#include "calibrate/calibrate.h"
#include "watchdog.h"
#include "device.h"

#define DISPLAY_WIDTH 	20
#define DISPLAY_HEIGHT	4

extern struct Channel xdata channels[CHANNEL_NUMBER];//обобщенная структура каналов
extern unsigned char brightness	;

volatile struct pt pt_display;




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

//char buf[20];
unsigned char dynamic_disp=0;//номер отображаемого динамического экрана


unsigned //char num_buf[8];

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
MAKE_MENU(m_s1i2,  NULL_ENTRY,m_s1i1,      m_s0i1,     NULL_ENTRY,   MENU_SET_BRIGHTNESS,   "brI");

// подменю Настройка каналов
MAKE_MENU(m_s2i1,  m_s2i2,    NULL_ENTRY,  m_s1i1,     m_s3i1,       /*MENU_CHN1_SET*/0, 		"C_1");
MAKE_MENU(m_s2i2,  m_s2i3,    m_s2i1,      m_s1i1,     m_s4i1,   	 /*MENU_CHN2_SET*/0, 		"C_2");
MAKE_MENU(m_s2i3,  m_s2i4,	  m_s2i2,      m_s1i1,     m_s5i1,   	 /*MENU_CHN3_SET*/0, 		"C_3");
MAKE_MENU(m_s2i4,  m_s2i5,	  m_s2i3,      m_s1i1,     m_s6i1,   	 /*MENU_CHN4_SET*/0, 		"C_4");
MAKE_MENU(m_s2i5,  m_s2i6,	  m_s2i4,      m_s1i1,     m_s7i1,   	 /*MENU_CHN5_SET*/0, 		"C_5");
MAKE_MENU(m_s2i6,  NULL_ENTRY,m_s2i5,      m_s1i1,     m_s8i1,   	 /*MENU_CHN6_SET*/0, 		"C_6");


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



enum
{
	CAL_HI=0,
	CAL_LO,
	UST_HI,
	UST_LO
};

enum
{
	BLINK_ALL=0xF,
	BLINK_NONE=0xFF
};

static unsigned char enter_flag=0; //зашли в поле ввода


unsigned char menuHandler(menuItem* currentMenuItem,unsigned char key);	 //обработка меню

void CalibrationKey(unsigned char key,unsigned char channel,unsigned char type);
void CalibrationScreen(unsigned char channel);//экран калибровки канала

void SetBrightnessKey(unsigned char key);
void SetBrightnessScreen(void);

void Set_Blink_Sym(struct Channel *chn,unsigned char sym_position);

void menuChange(menuItem code* NewMenu)
{
	if (NewMenu == &NULL_ENTRY)
	  return;

	selectedMenuItem = NewMenu;
}

void menuChange_NEXT(void)
{
	xdata menuItem code * tempMenu;

	if((selectedMenuItem->Next==&NULL_ENTRY)&&(selectedMenuItem->Parent!=&NULL_ENTRY))
	{
		tempMenu = selectedMenuItem->Parent;
		menuChange(tempMenu->Child);	
	}
	else
	{
		menuChange(selectedMenuItem->Next);	
	}	
}



unsigned char dispMenu(void)
{
	unsigned char i=0;

	if (selectedMenuItem == &m_s0i1) 
	{ // мы на верхнем уровне
		dynamic_disp= DYN_DISPALY_ON;
		Set_Blink_Sym(&channels[0],0);
		Set_Blink_Sym(&channels[4],3);
	} 
	else 
	{
		dynamic_disp=DYN_NOT_DISPLAY;

		Set_Blink_Sym(&channels[0],BLINK_NONE);
		Set_Blink_Sym(&channels[1],BLINK_NONE);
		Set_Blink_Sym(&channels[2],BLINK_NONE);
		Set_Blink_Sym(&channels[3],BLINK_NONE);
		Set_Blink_Sym(&channels[4],BLINK_NONE);
		Set_Blink_Sym(&channels[5],BLINK_NONE);

	    for(i=0;i<CHANNEL_NUMBER;i++)
	    {
			sprintf(channels[i].string_buf,"   ");
		}
		memcpy(channels[0].string_buf,selectedMenuItem->Text,4);
	}
	enter_flag=0;
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
//				menuChange(PREVIOUS);	
			}
			break;
		
			case '>': 
			{
			//	menuChange(NEXT);
				menuChange_NEXT();		
			}
			break;
		
		
			case 'E':
			{ // выбор пункта	
//			LED1=~LED1;				
//					sel = selectedMenuItem->Select;//SELECT;
					if (selectedMenuItem->Select != 0) 
					{
//						flag_menu_entry=1;
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
	    dispMenu(); 
	}
	else
	{
		if(key== 'Q') 
		{ // отмена выбора (возврат)
			flag_menu_entry=0;
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

unsigned char menuHandler(menuItem* currentMenuItem,unsigned char key)	 //обработка меню
{
	flag_menu_entry=1;
	
	switch (currentMenuItem->Select) 
	{
		case MENU_SET_BRIGHTNESS:
		{	
			SetBrightnessKey(key);
		}
		break;

		
		case MENU_CHN1_CAL_HI:
		{
			CalibrationKey(key,0,CAL_HI);
		}
		break; 

		case MENU_CHN1_CAL_LO:
		{
			CalibrationKey(key,0,CAL_LO);
		}
		break; 

		case MENU_CHN1_UST_HI:
		{
			CalibrationKey(key,0,UST_HI);
		}
		break; 

		case MENU_CHN1_UST_LO:
		{
			CalibrationKey(key,0,UST_LO);
		}
		break; 

		
		
		case MENU_CHN2_CAL_HI:
		{
			CalibrationKey(key,1,CAL_HI);
		}
		break; 

		case MENU_CHN2_CAL_LO:
		{
			CalibrationKey(key,1,CAL_LO);
		}
		break; 

		case MENU_CHN2_UST_HI:
		{
			CalibrationKey(key,1,UST_HI);
		}
		break; 

		case MENU_CHN2_UST_LO:
		{
			CalibrationKey(key,1,UST_LO);
		}
		break;


		case MENU_CHN3_CAL_HI:
		{
			CalibrationKey(key,2,CAL_HI);
		}
		break; 

		case MENU_CHN3_CAL_LO:
		{
			CalibrationKey(key,2,CAL_LO);
		}
		break; 

		case MENU_CHN3_UST_HI:
		{
			CalibrationKey(key,2,UST_HI);
		}
		break; 

		case MENU_CHN3_UST_LO:
		{
			CalibrationKey(key,2,UST_LO);
		}
		break;


		case MENU_CHN4_CAL_HI:
		{
			CalibrationKey(key,3,CAL_HI);
		}
		break; 

		case MENU_CHN4_CAL_LO:
		{
			CalibrationKey(key,3,CAL_LO);
		}
		break; 

		case MENU_CHN4_UST_HI:
		{
			CalibrationKey(key,3,UST_HI);
		}
		break; 

		case MENU_CHN4_UST_LO:
		{
			CalibrationKey(key,3,UST_LO);
		}
		break; 


		case MENU_CHN5_CAL_HI:
		{
			CalibrationKey(key,4,CAL_HI);
		}
		break; 

		case MENU_CHN5_CAL_LO:
		{
			CalibrationKey(key,4,CAL_LO);
		}
		break; 

		case MENU_CHN5_UST_HI:
		{
			CalibrationKey(key,4,UST_HI);
		}
		break; 

		case MENU_CHN5_UST_LO:
		{
			CalibrationKey(key,4,UST_LO);
		}
		break; 


		case MENU_CHN6_CAL_HI:
		{
			CalibrationKey(key,5,CAL_HI);
		}
		break; 

		case MENU_CHN6_CAL_LO:
		{
			CalibrationKey(key,5,CAL_LO);
		}
		break; 

		case MENU_CHN6_UST_HI:
		{
			CalibrationKey(key,5,UST_HI);
		}
		break; 

		case MENU_CHN6_UST_LO:
		{
			CalibrationKey(key,5,UST_LO);
		}
		break; 
	}	
	return 0;
}
//-----------------------------------------------------
unsigned char startMenu(void)
 {
	selectedMenuItem = &m_s0i1;

	dispMenu();
	PT_INIT(&pt_display);
		Set_Blink_Sym(&channels[0],BLINK_NONE);
		Set_Blink_Sym(&channels[1],BLINK_NONE);
		Set_Blink_Sym(&channels[2],BLINK_NONE);
		Set_Blink_Sym(&channels[3],BLINK_NONE);
		Set_Blink_Sym(&channels[4],BLINK_NONE);
		Set_Blink_Sym(&channels[5],BLINK_NONE);
	return 0;
}
//-------------------------------------------------------
void SetBrightnessKey(unsigned char key)
{
		switch(key)
		{
			case 'E'://enter
			{
				if(!enter_flag)
				{
				   //отобразить параметр
				   enter_flag=1;
				}
				else
				{
					//сохранить параметр в eeprom
					enter_flag=0;
				    flag_menu_entry=0;
					dispMenu(); 
				}
			}
			break;

//			case 'Q'//quit
//			{
//			}
//			break;

			case '>'://shift
			{
				
			}
			break;

			case '+'://increment
			{
				brightness=(brightness+1)&0xF;
			}
			break;
		}
		
		if(enter_flag)
		{
			SetBrightnessScreen();
		}
}

void SetBrightnessScreen(void)
{
		unsigned char i;
		dynamic_disp=DYN_NOT_DISPLAY;
	    for(i=0;i<CHANNEL_NUMBER;i++)
	    {
			memset(channels[i].string_buf,' ',3);
			channels[i].string_buf[3]=0;
		}
		sprintf(channels[0].string_buf," % 3bu",brightness);
}


#define CAL_ENTER_FIELD	2
//-------------------------------------------------------
void CalibrationKey(unsigned char key,unsigned char channel,unsigned char type)
{
		float value;
		static unsigned char  current_char=0;		
		switch(key)
		{
			case 'E'://enter
			{
				if(!enter_flag)
				{
					current_char=0;
		
					switch(type)
					{
						case CAL_HI:
						{
							value=channels[channel].calibrate.cal.cal_hi;	
						}
						break;
	
						case CAL_LO:
						{
							value=channels[channel].calibrate.cal.cal_lo;
						}
						break;
	
						case UST_HI:
						{
							value=channels[channel].calibrate.cal.ust_hi;
						}
						break;
	
						case UST_LO:
						{
							value=channels[channel].calibrate.cal.ust_lo;
						}
						break;
					}
					
				  if((_chkfloat_ (value)==0)&&(value>0.00)&&(value<9.99)) 
				  {
					   
					   sprintf(channels[CAL_ENTER_FIELD].string_buf,"%3.2f",value);
				  }
				  else  
				  {
					  sprintf(channels[CAL_ENTER_FIELD].string_buf,"0.00");
				  }	

				  enter_flag=1;
				  Set_Blink_Sym(&channels[CAL_ENTER_FIELD],current_char);
			    }
				else
				{
					  sscanf(channels[CAL_ENTER_FIELD].string_buf,"%f",&value);
				   	  if((_chkfloat_ (value)==0)&&(value>0.00)&&(value<9.99)) 
					  {
							switch(type)
							{
								case CAL_HI:
								{
									channels[channel].calibrate.cal.cal_hi=value;
									//сохранить параметр в eeprom	
								}
								break;
			
								case CAL_LO:
								{
									channels[channel].calibrate.cal.cal_lo=value;
								}
								break;
			
								case UST_HI:
								{
									channels[channel].calibrate.cal.ust_hi=value;
								}
								break;
			
								case UST_LO:
								{
									channels[channel].calibrate.cal.ust_lo=value;
								}
								break;
							}
					 }
					
					enter_flag=0;
				    flag_menu_entry=0;
					dispMenu(); 
					
				}
			}
			break;

//			case 'Q'//quit
//			{
//			}
//			break;

			case '>'://shift
			{
			 	current_char++;
				if(current_char==1)
				{
					current_char=2;
				}

				if(current_char>3)
				{
					current_char=0;
				}

				Set_Blink_Sym(&channels[CAL_ENTER_FIELD],current_char);
			}
			break;

			case '+'://increment
			{	
				channels[CAL_ENTER_FIELD].string_buf[current_char]++;
				if((channels[CAL_ENTER_FIELD].string_buf[current_char]<'0')||(channels[CAL_ENTER_FIELD].string_buf[current_char]>'9'))
				{
					channels[CAL_ENTER_FIELD].string_buf[current_char]='0';
				}
				channels[CAL_ENTER_FIELD].string_buf[1]='.';	
			}
			break;
		}

		if(enter_flag)
		{
			CalibrationScreen(channel);
		}		
}

void CalibrationScreen(unsigned char channel)//экран калибровки канала
{
		unsigned char i;
		xdata menuItem code * tempMenu;

		dynamic_disp=DYN_NOT_DISPLAY;
	    for(i=3;i<CHANNEL_NUMBER;i++)
	    {
			sprintf(channels[i].string_buf,"   ");
//			memset(channels[i].string_buf,' ',3);
//			channels[i].string_buf[3]=0;
		}
		tempMenu= selectedMenuItem->Parent;
		memcpy(channels[0].string_buf,tempMenu->Text,4);
		memcpy(channels[1].string_buf,selectedMenuItem->Text,4);
	//	sprintf(channels[0].string_buf," % 3bu",brightness);
}


void Set_Blink_Sym(struct Channel *chn,unsigned char sym_position)
{
		if(sym_position<4)
		{
				sprintf(chn->string_mask,"XXXX");
				chn->string_mask[sym_position]=' ';
		}
		else
		{
			if(sym_position==0xF)
			{
				sprintf(chn->string_mask,"    ");
			}
			else
			{
				sprintf(chn->string_mask,"XXXX");
			}
		}	
}
//--------------------------------------------------------
PT_THREAD(DisplayProcess(struct pt *pt))
{

  static unsigned char i=0;
  static float value;
  PT_BEGIN(pt);

  while(1) 
  {
//	wdt_count[Display_Proc].process_state=IDLE;  

  	PT_DELAY(pt,50);
 //	PT_YIELD_UNTIL(pt,dynamic_disp); //ждем команды на старт	
	 
   wdt_count[Display_Proc].process_state=RUN;
   if(/*dynamic_disp*/selectedMenuItem == &m_s0i1)
   {
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

	}

	Tablo_Output_Frame();
	wdt_count[Display_Proc].count++;
  }

  PT_END(pt);
 }
