#ifndef _MENU_H_
#define _MENU_H_
#include "pt/pt.h"




enum {
    MENU_CANCEL=1,

    MENU_CHN_SET,
	    MENU_CHN1_SET,
	    MENU_CHN2_SET,
	    MENU_CHN3_SET,
		MENU_CHN4_SET,
		MENU_CHN5_SET,
		MENU_CHN6_SET,
		MENU_CHN7_SET,
		MENU_CHN8_SET,
		MENU_CHN_DOL_SET,
		MENU_CHN_FREQ1_SET,
		MENU_CHN_FREQ2_SET,
		MENU_CHN_FREQ3_SET,
		MENU_CHN_FREQ_HI_SET,

    MENU_CHN_CAL,
	    MENU_CHN1_CAL,
	    MENU_CHN2_CAL,
	    MENU_CHN3_CAL,
		MENU_CHN4_CAL,
		MENU_CHN5_CAL,
		MENU_CHN6_CAL,
		MENU_CHN7_CAL,
		MENU_CHN8_CAL,
		MENU_CHN_DOL_CAL,
		MENU_CHN_FREQ1_CAL,
		MENU_CHN_FREQ2_CAL,
		MENU_CHN_FREQ3_CAL,
		MENU_CHN_FREQ_HI_CAL,

    MENU_DEV_SET
};

#define LCD_1_STR_ADDR	0x00
#define LCD_2_STR_ADDR	0x40
#define LCD_3_STR_ADDR	0x14
#define LCD_4_STR_ADDR	0x54


#define P_MAX 999
#define F_MAX 9999
#define U_MAX 9999
#define I_MAX 40.0	

#define K_P	18.413	//коэффициенты F=K*P+B
#define B_P	-5.674

unsigned char startMenu(void);
void initMenu(void);
unsigned char dispMenu(void);
unsigned char* CopyToData(unsigned char code *mas); //копируем массив из code в data

void dispSetScroller(unsigned int num,unsigned int max);//установка значения с полосой
void DynamicDisplay(void);//динамическое отображение параметра
unsigned char menuKey(unsigned char key);
PT_THREAD(DisplayProcess(struct pt *pt));

#endif
