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

sbit LED=P0^6;


enum
{
	DYN_NOT_DISPLAY=0,
	DYN_DISPALY_ON =1
//	DYN_DISPLAY_CH1=1,
//	DYN_DISPLAY_CH2=2,
//	DYN_DISPALY_CH3=3
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
MAKE_MENU(m_s0i1,  m_s0i2,	  NULL_ENTRY,  NULL_ENTRY, m_s1i2,       0, 	"DATA SCREEN1");
MAKE_MENU(m_s0i2,  m_s0i3,	  m_s0i1,      NULL_ENTRY, m_s1i2,       0, 	"DATA SCREEN2");
MAKE_MENU(m_s0i3,  m_s0i4,	  m_s0i2,      NULL_ENTRY, m_s1i2,       0, 	"DATA SCREEN3");
MAKE_MENU(m_s0i4,  NULL_ENTRY,m_s0i3,      NULL_ENTRY, m_s1i2,       0, 	"DATA SCREEN4");

MAKE_MENU(m_s1i1,  m_s1i2,    NULL_ENTRY,  m_s0i1,     m_s2i1,       0, 		"Channel settings");
MAKE_MENU(m_s1i2,  m_s1i3,   /*m_s1i1*/NULL_ENTRY,      m_s0i1,     m_s3i1,       0, 		"Channel calibr.");
MAKE_MENU(m_s1i3,  NULL_ENTRY,m_s1i2,      m_s0i1,     NULL_ENTRY,   MENU_DEV_SET,"Device settings");

// подменю Настройка каналов
MAKE_MENU(m_s2i1,  m_s2i2,    NULL_ENTRY,  m_s1i1,     NULL_ENTRY,   MENU_CHN1_SET, 		"Channel 1 set.");
MAKE_MENU(m_s2i2,  m_s2i3,    m_s2i1,      m_s1i1,     NULL_ENTRY,   MENU_CHN2_SET, 		"Channel 2 set.");
MAKE_MENU(m_s2i3,  m_s2i4,	  m_s2i2,      m_s1i1,     NULL_ENTRY,   MENU_CHN3_SET, 		"Channel 3 set.");
MAKE_MENU(m_s2i4,  m_s2i5,	  m_s2i3,      m_s1i1,     NULL_ENTRY,   MENU_CHN4_SET, 		"Channel 4 set.");
MAKE_MENU(m_s2i5,  m_s2i6,	  m_s2i4,      m_s1i1,     NULL_ENTRY,   MENU_CHN5_SET, 		"Channel 5 set.");
MAKE_MENU(m_s2i6,  m_s2i7,	  m_s2i5,      m_s1i1,     NULL_ENTRY,   MENU_CHN6_SET, 		"Channel 6 set.");
MAKE_MENU(m_s2i7,  m_s2i8,	  m_s2i6,      m_s1i1,     NULL_ENTRY,   MENU_CHN7_SET, 		"Channel 7 set.");
MAKE_MENU(m_s2i8,  m_s2i9,	  m_s2i7,      m_s1i1,     NULL_ENTRY,   MENU_CHN8_SET, 		"Channel 8 set.");
MAKE_MENU(m_s2i9,  m_s2i10,	  m_s2i8,      m_s1i1,     NULL_ENTRY,   MENU_CHN_DOL_SET, 		"Channel dol set.");
MAKE_MENU(m_s2i10, m_s2i11,	  m_s2i9,      m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ1_SET, 	"Channel freq1 set.");
MAKE_MENU(m_s2i11, m_s2i12,	  m_s2i10,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ2_SET, 	"Channel freq2 set.");
MAKE_MENU(m_s2i12, m_s2i13,	  m_s2i11,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ3_SET, 	"Channel freq3 set.");
MAKE_MENU(m_s2i13, NULL_ENTRY,m_s2i12,     m_s1i1,     NULL_ENTRY,   MENU_CHN_FREQ_HI_SET, 	"Channel freqhi set.");

// подменю Калибровка каналов
MAKE_MENU(m_s3i1,  m_s3i2,    NULL_ENTRY,  m_s1i2,     NULL_ENTRY,   MENU_CHN1_CAL, 		"Channel 1 cal.");
MAKE_MENU(m_s3i2,  m_s3i3,    m_s3i1,      m_s1i2,     NULL_ENTRY,   MENU_CHN2_CAL, 		"Channel 2 cal.");
MAKE_MENU(m_s3i3,  m_s3i4,	  m_s3i2,      m_s1i2,     NULL_ENTRY,   MENU_CHN3_CAL, 		"Channel 3 cal.");
MAKE_MENU(m_s3i4,  m_s3i5,	  m_s3i3,      m_s1i2,     NULL_ENTRY,   MENU_CHN4_CAL, 		"Channel 4 cal.");
MAKE_MENU(m_s3i5,  m_s3i6,	  m_s3i4,      m_s1i2,     NULL_ENTRY,   MENU_CHN5_CAL, 		"Channel 5 cal.");
MAKE_MENU(m_s3i6,  m_s3i7,	  m_s3i5,      m_s1i2,     NULL_ENTRY,   MENU_CHN6_CAL, 		"Channel 6 cal.");
MAKE_MENU(m_s3i7,  m_s3i8,	  m_s3i6,      m_s1i2,     NULL_ENTRY,   MENU_CHN7_CAL, 		"Channel 7 cal.");
MAKE_MENU(m_s3i8,  /*m_s3i9*/NULL_ENTRY,	  m_s3i7,      m_s1i2,     NULL_ENTRY,   MENU_CHN8_CAL, 		"Channel 8 cal.");
MAKE_MENU(m_s3i9,  m_s3i10,	  m_s3i8,      m_s1i2,     NULL_ENTRY,   MENU_CHN_DOL_CAL, 		"Channel dol cal.");
MAKE_MENU(m_s3i10, m_s3i11,	  m_s3i9,      m_s1i2,     NULL_ENTRY,   MENU_CHN_FREQ1_CAL, 	"Channel freq1 cal.");
MAKE_MENU(m_s3i11, m_s3i12,	  m_s3i10,     m_s1i2,     NULL_ENTRY,   MENU_CHN_FREQ2_CAL, 	"Channel freq2 cal.");
MAKE_MENU(m_s3i12, m_s3i13,	  m_s3i11,     m_s1i2,     NULL_ENTRY,   MENU_CHN_FREQ3_CAL, 	"Channel freq3 cal.");
MAKE_MENU(m_s3i13, NULL_ENTRY,m_s3i12,     m_s1i2,     NULL_ENTRY,   MENU_CHN_FREQ_HI_CAL, 	"Channel freqhi cal.");


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

struct input_field
{
	unsigned char input_char_buf[2*INPUT_CHAR_BUF_LEN+1];
	unsigned char char_count;
	unsigned char has_point;
};

volatile struct input_field input_field_lo={"      ",0,0};
volatile struct input_field input_field_hi={"      ",0,0};

volatile struct input_field *input_field_ptr;

unsigned char cal_float;//верхнее или нижнее значение

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
xdata menuItem code * tempMenu, *tempMenu2;

	// первая строка - заголовок. Или пункт меню верхнего уровня


	tempMenu = selectedMenuItem;//->Parent;

	//LCD_WriteCommand(LCD_CMD_ON);
    //LCD_WriteCommand(LCD_CMD_CLEAR);

	if ((tempMenu == &m_s0i1)||(tempMenu == &m_s0i2)||(tempMenu == &m_s0i3)||(tempMenu == &m_s0i4)) 
	{ // мы на верхнем уровне
		dynamic_disp= DYN_DISPALY_ON;
	} 
	else 
	{
		dynamic_disp=DYN_NOT_DISPLAY;

		//LCD_WriteCommand(LCD_SET_ADDR|0x1);
		//LCD_WriteCommand(LCD_SET_ADDR|0x1);

		tempMenu2=tempMenu->Previous;
		if(tempMenu2!=&NULL_ENTRY)
		{
			//LCD_WriteString(tempMenu2->Text);
		}
		
		//LCD_WriteCommand(LCD_SET_ADDR|0x40);
		//LCD_WriteData(0xC9);

		//LCD_WriteCommand(LCD_SET_ADDR|0x41);
		//LCD_WriteString(tempMenu->Text);


		//LCD_WriteCommand(LCD_SET_ADDR|0x15);

		tempMenu2=tempMenu->Next;
		if(tempMenu2!=&NULL_ENTRY)
		{
			//LCD_WriteString(tempMenu2->Text);

			//LCD_WriteCommand(LCD_SET_ADDR|0x55);
	
			tempMenu2=tempMenu2->Next;
		//	if(tempMenu2!=&NULL_ENTRY)
		    //LCD_WriteString(tempMenu2->Text);
		}
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
		
			case '[': 
			{
				menuChange(PREVIOUS);
	
			}
			break;
		
			case ']': 
			{
				menuChange(NEXT);			
			}
			break;
		
			case 'x':
			{
			  _nop_();
			}
			break;
		
			case 'F':
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
	
			case 'C': 
			{ // отмена выбора (возврат)
				menuChange(PARENT);
//				//LCD_WriteCommand(LCD_CMD_ON);
			}
			break;
	
			default:
			{
	
			}		
		} 
	
		if(key!=0)
	   	{
			//LCD_WriteCommand(LCD_CMD_CLEAR);
		//	delay(10);
			dispMenu(); 
		}
	}
	else
	{
		if(key== 'C') 
		{ // отмена выбора (возврат)
	
			flag_menu_entry=0;
			dynamic_disp=DYN_NOT_DISPLAY;
			//LCD_WriteCommand(LCD_CMD_CLEAR);
//			delay(3);
			//LCD_WriteCommand(LCD_CMD_ON);
		//	delay(3);
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
		case MENU_DEV_SET:
		{	
			SettingsKey(key);
		}
		break; 

		case MENU_CHN1_CAL:
		{
			CalibrationKey(key,0);	
		}
		break;

		case MENU_CHN2_CAL:
		{
			CalibrationKey(key,1);	
		}
		break;

		case MENU_CHN3_CAL:
		{
			CalibrationKey(key,2);	
		}
		break;

		case MENU_CHN4_CAL:
		{
			CalibrationKey(key,3);	
		}
		break;

		case MENU_CHN5_CAL:
		{
			CalibrationKey(key,4);	
		}
		break;

		case MENU_CHN6_CAL:
		{
			CalibrationKey(key,5);	
		}
		break;

		case MENU_CHN7_CAL:
		{
			CalibrationKey(key,6);	
		}
		break;

		case MENU_CHN8_CAL:
		{
			CalibrationKey(key,7);	
		}
		break;

		case MENU_CHN_DOL_CAL:
		{
			CalibrationKey(key,11);	
		}
		break;

		case MENU_CHN_FREQ1_CAL:
		{
			CalibrationKey(key,8);	
		}
		break;

		case MENU_CHN_FREQ2_CAL:
		{
			CalibrationKey(key,9);	
		}
		break;

		case MENU_CHN_FREQ3_CAL:
		{
			CalibrationKey(key,10);	
		}
		break;

		case MENU_CHN_FREQ_HI_CAL:
		{
			CalibrationKey(key,12);	
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
	return 0;
}
//-------------------------------------------------------
void CalibrationKey(unsigned char key,unsigned char channel)
{
		switch(key)
		{
			case 'F':
			{				
				//LCD_WriteCommand(LCD_CMD_CLEAR);

				dynamic_disp= DYN_DISPALY_ON;

				input_field_hi.char_count=INPUT_CHAR_BUF_LEN;
				input_field_lo.char_count=INPUT_CHAR_BUF_LEN;

				sprintf(input_field_hi.input_char_buf,"%5.4f",channels[channel].calibrate.cal.cal_hi);
				sprintf(input_field_lo.input_char_buf,"%5.4f",channels[channel].calibrate.cal.cal_lo);	
				
				if(strchr (input_field_hi.input_char_buf,'.'))
				{
					input_field_hi.has_point=1;
				}			
				else
				{
					input_field_hi.has_point=0;
				}

				if(strchr (input_field_lo.input_char_buf,'.'))
				{
					input_field_lo.has_point=1;
				}			
				else
				{
					input_field_lo.has_point=0;
				}

				input_field_hi.input_char_buf[INPUT_CHAR_BUF_LEN]=0;
				input_field_lo.input_char_buf[INPUT_CHAR_BUF_LEN]=0;

				input_field_ptr=&input_field_lo;

			}
			break;

			case '[':
			{
				input_field_ptr=&input_field_lo;
			}
			break;

			case ']':
			{
				input_field_ptr=&input_field_hi;
			}
			break;

			case '=':
			{
				if((input_field_ptr->input_char_buf[0]!=' ')/*&&(input_field_ptr->input_char_buf[0]!='-')*/)
				{
					if(input_field_ptr==&input_field_lo)
					{	
						sscanf(input_field_lo.input_char_buf,"%f",&channels[channel].calibrate.cal.cal_lo);
						SetFirstPoint(channel,channels[channel].channel_data,channels[channel].calibrate.cal.cal_lo);
					}
					else
					{
						sscanf(input_field_hi.input_char_buf,"%f",&channels[channel].calibrate.cal.cal_hi);
						SetSecondPoint(channel,channels[channel].channel_data,channels[channel].calibrate.cal.cal_hi);
					}
				}
			}
			break;

			case '/':
			{
				if(input_field_ptr->char_count)
				{
					input_field_ptr->input_char_buf[input_field_ptr->char_count-1]=' ';
					input_field_ptr->char_count--;
				}
				
				if(strchr (input_field_ptr->input_char_buf,'.'))
				{
					input_field_ptr->has_point=1;
				}			
				else
				{
					input_field_ptr->has_point=0;
				}				
			}
			break;
		}

		if(input_field_ptr->char_count<INPUT_CHAR_BUF_LEN)
		{
			if((key>='0') && (key<='9'))
			{
				   input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
				   input_field_ptr->char_count++;
			}
			
			if(key=='.')
			{
				   if((input_field_ptr->has_point==0)&&(input_field_ptr->char_count!=0)&&(input_field_ptr->char_count<(INPUT_CHAR_BUF_LEN)))
				   {
				   	   input_field_ptr->input_char_buf[input_field_ptr->char_count]='.';
				   	   input_field_ptr->char_count++;
					   input_field_ptr->has_point=1;
				   }			
			}
	
			if(key=='-')
			{
				   if(input_field_ptr->char_count==0)
				   {
					   input_field_ptr->input_char_buf[input_field_ptr->char_count]='-';
					   input_field_ptr->char_count++;
				   }			
			}
		}
		else
		{
			 input_field_ptr->char_count=INPUT_CHAR_BUF_LEN/*-1*/;
		}
		CalibrationScreen(channel);		
}


void CalibrationScreen(unsigned char channel)//экран калибровки канала
{
//   sprintf(&string_buf,"Chn. val.:% 8lu",channels[channel].channel_data);
//   //LCD_WriteAC(LCD_1_STR_ADDR);
//   //LCD_WriteString(&string_buf);

 	if(input_field_ptr==&input_field_lo)
	{
			//LCD_WriteAC(LCD_1_STR_ADDR);
			sprintf(&string_buf,"Low val [%6s]",input_field_lo.input_char_buf);
			//LCD_WriteString(&string_buf);

			//LCD_WriteAC(LCD_2_STR_ADDR);
			sprintf(&string_buf,"High val %6s ",input_field_hi.input_char_buf);
			//LCD_WriteString(&string_buf);

			//LCD_WriteAC(LCD_4_STR_ADDR);
			sprintf(&string_buf,"'='-Enter,'/'-Del");
			//LCD_WriteString(&string_buf);

			//LCD_WriteAC(LCD_1_STR_ADDR+9+input_field_lo.char_count);
			//LCD_WriteCommand(LCD_CMD_CURSOR);
	}

	if(input_field_ptr==&input_field_hi)
	{
			//LCD_WriteAC(LCD_1_STR_ADDR);
			sprintf(&string_buf,"Low val  %6s ",input_field_lo.input_char_buf);
			//LCD_WriteString(&string_buf); 

			//LCD_WriteAC(LCD_2_STR_ADDR);
			sprintf(&string_buf,"High val[%6s]",input_field_hi.input_char_buf);
			//LCD_WriteString(&string_buf);

			//LCD_WriteAC(LCD_4_STR_ADDR);
			sprintf(&string_buf,"'='-Enter,'/'-Del");
			//LCD_WriteString(&string_buf);

			//LCD_WriteAC(LCD_2_STR_ADDR+9+input_field_hi.char_count);
			//LCD_WriteCommand(LCD_CMD_CURSOR);
	}
}


void SettingsKey(unsigned char key)
{
		switch(key)
		{
			case 'F':
			{				
				//LCD_WriteCommand(LCD_CMD_CLEAR);

//				input_field_hi.char_count=2;
				input_field_lo.char_count=2;

//				sprintf(input_field_hi.input_char_buf,"%02u",channels[channel].calibrate.cal.cal_hi);
				sprintf(input_field_lo.input_char_buf,"%02bu",ADRESS_DEV);	
				

//				input_field_hi.input_char_buf[2]=0;
				input_field_lo.input_char_buf[2]=0;

				input_field_ptr=&input_field_lo;

			}
			break;

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

			case '=':
			{
				if((input_field_ptr->input_char_buf[0]!=' ')/*&&(input_field_ptr->input_char_buf[0]!='-')*/)
				{
					if(input_field_ptr==&input_field_lo)
					{	
						sscanf(input_field_lo.input_char_buf,"%bu",&ADRESS_DEV);
						//SetFirstPoint(channel,channels[channel].channel_data,channels[channel].calibrate.cal.cal_lo);
						Device_Save_Address(ADRESS_DEV);
					}
				}
			}
			break;

			case '/':
			{
				if(input_field_ptr->char_count)
				{
					input_field_ptr->input_char_buf[input_field_ptr->char_count-1]=' ';
					input_field_ptr->char_count--;
				}				
			}
			break;
		}

		if(input_field_ptr->char_count<2)
		{
			if((key>='0') && (key<='9'))
			{
				   if(input_field_ptr->char_count==0)
				   {
				   		if((key=='0')||(key=='1'))
						{
						   input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
						   input_field_ptr->char_count++;							
						}
				   }
				   else
				   {
					   if(input_field_ptr->char_count==1)
					   {
					   		   if(input_field_ptr->input_char_buf[0]=='0')
							   {
								   		input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
								   		input_field_ptr->char_count++;							   		
							   }
							   else
							   {
								   if(((key>='0') && (key<='5')))
								   {
								   		input_field_ptr->input_char_buf[input_field_ptr->char_count]=key;
								   		input_field_ptr->char_count++;
								   }
							   }
					   }
				   }
			}
		}
		else
		{
			 input_field_ptr->char_count=2;
		}
		SettingsScreen();		
}

void SettingsScreen(void)
{
 	if(input_field_ptr==&input_field_lo)
	{
			//LCD_WriteAC(LCD_1_STR_ADDR);
			sprintf(&string_buf,"Address [%2s]",input_field_lo.input_char_buf);
			//LCD_WriteString(&string_buf);

			//LCD_WriteAC(LCD_4_STR_ADDR);
			sprintf(&string_buf,"'='-Enter,'/'-Del");
			//LCD_WriteString(&string_buf);

			//LCD_WriteAC(LCD_1_STR_ADDR+9+input_field_lo.char_count);
			//LCD_WriteCommand(LCD_CMD_CURSOR);
	}	
}

PT_THREAD(DisplayProcess(struct pt *pt))
{

static unsigned int P=200;
static int F=32;
static unsigned int U_ch2=756,U_ch3=375;
static float I_ch4=18.6;


  PT_BEGIN(pt);

  while(1) 
  {
	wdt_count[Display_Proc].process_state=IDLE;  
 	PT_YIELD_UNTIL(pt,dynamic_disp); //ждем команды на старт	
	 
  	wdt_count[Display_Proc].process_state=RUN;
  	PT_DELAY(pt,20);
	Channel_All_Get_Data_Request();
	PT_DELAY(pt,20);

	
	if(selectedMenuItem == &m_s0i1)
	{
		//P= (unsigned int)channels[0].channel_data;
		P=(unsigned int)GetCalibrateVal(0,channels[0].channel_data);
		if(P>P_MAX)
		{
			P=P_MAX;
		}
	
	
		F=(unsigned int)((float)P*K_P+(B_P));
	
		if(F<0)
		{
			F=0;
		}
	
		if(F>F_MAX)
		{
			F=F_MAX;	
		}
	
		sprintf(&string_buf,"P=%3dkg/cm F=%4dkgs",P,F);
		//LCD_WriteAC(LCD_1_STR_ADDR);
		//LCD_WriteString(&string_buf);
	   
	
		//U_ch2=(unsigned int)(channels[1].channel_data*10000/0xFFFF);
		U_ch2=(unsigned int)GetCalibrateVal(1,channels[1].channel_data);

		if(U_ch2>U_MAX)
		{
			U_ch2=U_MAX;
		}
	
		sprintf(&string_buf,"2=%4d  mV",U_ch2);
		//LCD_WriteAC(LCD_2_STR_ADDR);
	//	PT_YIELD(pt);//дадим другим процессам время
		//LCD_WriteString(&string_buf);
		
		//U_ch3=(unsigned int)(channels[2].channel_data*10000/0xFFFF);
		U_ch3=(unsigned int)GetCalibrateVal(2,channels[2].channel_data);
		if(U_ch3>U_MAX)
		{
			U_ch3=U_MAX;
		}
	
		sprintf(&string_buf,"3=%4d  mV",U_ch3);
		//LCD_WriteAC(LCD_3_STR_ADDR);
		//LCD_WriteString(&string_buf);


//	 	I_ch4=((float)channels[3].channel_data*20.0/0xFFFF);
//		if(I_ch4>I_MAX)
//		{
//			I_ch4=I_MAX;
//		}
		
//		sprintf(&string_buf,"Cal_3=%5.3f",GetCalibrateVal(2,channels[2].channel_data));
//		//LCD_WriteAC(LCD_3_STR_ADDR);
//	//	PT_YIELD(pt);//дадим другим процессам время
//		//LCD_WriteString(&string_buf);
		
			
		
	 	//I_ch4=((float)channels[3].channel_data*20.0/0xFFFF);
		I_ch4=GetCalibrateVal(3,channels[3].channel_data);
		if(I_ch4>I_MAX)
		{
			I_ch4=I_MAX;
		}
		
		sprintf(&string_buf,"4=%4.1f  mA",I_ch4);
		//LCD_WriteAC(LCD_4_STR_ADDR);
	//	PT_YIELD(pt);//дадим другим процессам время
		//LCD_WriteString(&string_buf);	
	}

	if(selectedMenuItem == &m_s0i2)
	{
		sprintf(&string_buf,"Chnl. 5=%9.3f",GetCalibrateVal(4,channels[4].channel_data));
		//LCD_WriteAC(LCD_1_STR_ADDR);
		//LCD_WriteString(&string_buf);

		sprintf(&string_buf,"Chnl. 6=%9.3f",GetCalibrateVal(5,channels[5].channel_data));
		//LCD_WriteAC(LCD_2_STR_ADDR);
		//LCD_WriteString(&string_buf);

		sprintf(&string_buf,"Chnl. 7=%9.3f",GetCalibrateVal(6,channels[6].channel_data));
		//LCD_WriteAC(LCD_3_STR_ADDR);
		//LCD_WriteString(&string_buf);

		sprintf(&string_buf,"Chnl. 8=%9.3f",GetCalibrateVal(7,channels[7].channel_data));
		//LCD_WriteAC(LCD_4_STR_ADDR);
		//LCD_WriteString(&string_buf);
	}

	if(selectedMenuItem == &m_s0i3)
	{
// 		sprintf(&string_buf,"Freq. 1=%5.1f",GetCalibrateVal(8,channels[8].channel_data));
//		//LCD_WriteAC(LCD_1_STR_ADDR);
//		//LCD_WriteString(&string_buf);
//
//		sprintf(&string_buf,"Freq. 2=%5.1f",GetCalibrateVal(9,channels[9].channel_data));
//		//LCD_WriteAC(LCD_2_STR_ADDR);
//		//LCD_WriteString(&string_buf);
//
//		sprintf(&string_buf,"Freq. 3=%5.1f",GetCalibrateVal(10,channels[10].channel_data));
//		//LCD_WriteAC(LCD_3_STR_ADDR);
//		//LCD_WriteString(&string_buf);
//
//		sprintf(&string_buf,"Freq. Hi=%5u",(unsigned int)GetCalibrateVal(12,channels[12].channel_data));
//		//LCD_WriteAC(LCD_4_STR_ADDR);
//		//LCD_WriteString(&string_buf);

 		sprintf(&string_buf,"Freq. 1=%5.1f Hz",(float)channels[8].channel_data/256);
		//LCD_WriteAC(LCD_1_STR_ADDR);
		//LCD_WriteString(&string_buf);

		sprintf(&string_buf,"Freq. 2=%5.1f Hz",(float)channels[9].channel_data/256);
		//LCD_WriteAC(LCD_2_STR_ADDR);
		//LCD_WriteString(&string_buf);

		sprintf(&string_buf,"Freq. 3=%5.1f Hz",(float)channels[10].channel_data/256);
		//LCD_WriteAC(LCD_3_STR_ADDR);
		//LCD_WriteString(&string_buf);

		sprintf(&string_buf,"Freq.Hi=%5u Hz",(unsigned int)channels[12].channel_data);
		//LCD_WriteAC(LCD_4_STR_ADDR);
		//LCD_WriteString(&string_buf);
	}

	if(selectedMenuItem == &m_s0i4)
	{
//		sprintf(&string_buf,"DOL =%08lu",(unsigned long)GetCalibrateVal(11,channels[11].channel_data));
//		//LCD_WriteAC(LCD_1_STR_ADDR);
//		//LCD_WriteString(&string_buf);
		sprintf(&string_buf,"Freq.Hi=%5u Hz",(unsigned int)channels[13].channel_data);
		//LCD_WriteAC(LCD_1_STR_ADDR);
		//LCD_WriteString(&string_buf);		

		sprintf(&string_buf,"DOL =%08lu",channels[11].channel_data);
		//LCD_WriteAC(LCD_2_STR_ADDR);
		//LCD_WriteString(&string_buf);
	}

	if(flag_menu_entry==1)
	{
	
//	if((selectedMenuItem == &m_s3i3) && (flag_menu_entry==1))	 //calibr 1 channel
//	{
//		CalibrationScreen(2);	
//	}

//		if(selectedMenuItem == &m_s3i1)	 //calibr 1 channel
//		{
//			CalibrationScreen(0);	
//		}
//
//		if(selectedMenuItem == &m_s3i2)	 //calibr 1 channel
//		{
//			CalibrationScreen(1);	
//		}
//
//		if(selectedMenuItem == &m_s3i3)	 //calibr 1 channel
//		{
//			CalibrationScreen(2);	
//		}
//
//		if(selectedMenuItem == &m_s3i4)	 //calibr 1 channel
//		{
//			CalibrationScreen(3);	
//		}
//
//		if(selectedMenuItem == &m_s3i5)	 //calibr 1 channel
//		{
//			CalibrationScreen(4);	
//		}
//
//		if(selectedMenuItem == &m_s3i6)	 //calibr 1 channel
//		{
//			CalibrationScreen(5);	
//		}
//
//		if(selectedMenuItem == &m_s3i7)	 //calibr 1 channel
//		{
//			CalibrationScreen(6);	
//		}
//
//		if(selectedMenuItem == &m_s3i8)	 //calibr 1 channel
//		{
//			CalibrationScreen(7);	
//		}
//
//		if(selectedMenuItem == &m_s3i9)	 //calibr 1 channel
//		{
//			CalibrationScreen(11);	
//		}
//
//		if(selectedMenuItem == &m_s3i10)	 //calibr 1 channel
//		{
//			CalibrationScreen(8);	
//		}
//
//		if(selectedMenuItem == &m_s3i11)	 //calibr 1 channel
//		{
//			CalibrationScreen(9);	
//		}
//
//		if(selectedMenuItem == &m_s3i12)	 //calibr 1 channel
//		{
//			CalibrationScreen(10);	
//		}
//
//		if(selectedMenuItem == &m_s3i13)	 //calibr 1 channel
//		{
//			CalibrationScreen(12);	
//		}
	}
	wdt_count[Display_Proc].count++;
  }

  PT_END(pt);
 }
