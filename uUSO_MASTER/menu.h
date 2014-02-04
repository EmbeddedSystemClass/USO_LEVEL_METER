#ifndef _MENU_H_
#define _MENU_H_
#include "pt/pt.h"




enum {
    MENU_CANCEL=1,

    MENU_CHN_SET,
	    MENU_CHN1_SET,
			MENU_CHN1_CAL_HI,
			MENU_CHN1_CAL_LO,
			MENU_CHN1_UST_HI,
			MENU_CHN1_UST_LO,
	    MENU_CHN2_SET,
			MENU_CHN2_CAL_HI,
			MENU_CHN2_CAL_LO,
			MENU_CHN2_UST_HI,
			MENU_CHN2_UST_LO,
	    MENU_CHN3_SET,
			MENU_CHN3_CAL_HI,
			MENU_CHN3_CAL_LO,
			MENU_CHN3_UST_HI,
			MENU_CHN3_UST_LO,
		MENU_CHN4_SET,
			MENU_CHN4_CAL_HI,
			MENU_CHN4_CAL_LO,
			MENU_CHN4_UST_HI,
			MENU_CHN4_UST_LO,
		MENU_CHN5_SET,
			MENU_CHN5_CAL_HI,
			MENU_CHN5_CAL_LO,
			MENU_CHN5_UST_HI,
			MENU_CHN5_UST_LO,
		MENU_CHN6_SET,
			MENU_CHN6_CAL_HI,
			MENU_CHN6_CAL_LO,
			MENU_CHN6_UST_HI,
			MENU_CHN6_UST_LO,


	MENU_SET_BRIGHTNESS
};



unsigned char startMenu(void);
void initMenu(void);
unsigned char dispMenu(void);

void DynamicDisplay(void);//������������ ����������� ���������
unsigned char menuKey(unsigned char key);
PT_THREAD(DisplayProcess(struct pt *pt));

#endif
