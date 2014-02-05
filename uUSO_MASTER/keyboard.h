#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <ADuC845.h>
#include "pt/pt.h"



//#define KEY_UP				0xF1D7//(бо)
//#define KEY_DOWN			0x1D17//(X)
//#define KEY_RIGHT			0x170F//()])
//#define KEY_LEFT			0x1717//([()
//#define KEY_OK				0xF17E//(F)
//#define KEY_ESC				0xF1EE//(C)
//#define KEY_0				0x1E1E
//#define KEY_1				0x1D1E
//#define KEY_2				0x1D1D
//#define KEY_3				0x1D1B
//#define KEY_4				0x1B1E
//#define KEY_5				0x1B1D
//#define KEY_6				0x1B1B
//#define KEY_7				0x171E
//#define KEY_8				0x171D
//#define KEY_9				0x171B
//#define KEY_POINT			0x1E1D


//#define KB_MASK 0xE0 // 1110 0000

//sfr KB_PO = 0x80; // P0
//sfr KB_PI = 0x90; // P1
//
//#define KB_ROW		5
//#define	KB_COLUMN	5
//
//#define KB_MASK		0x1F
//
//sbit KB_Out0 = KB_PO^0;
//sbit KB_Out1 = KB_PO^1;
//sbit KB_Out2 = KB_PO^2;
//sbit KB_Out3 = KB_PO^3;
//sbit KB_Out4 = KB_PO^4;
//
//sbit KB_In0 = KB_PI^0;
//sbit KB_In1 = KB_PI^1;
//sbit KB_In2 = KB_PI^2;
//sbit KB_In3 = KB_PI^3;
//sbit KB_In4 = KB_PI^4;

sbit KEY_1=P3^2;
sbit KEY_2=P3^3;
sbit KEY_3=P3^4;
sbit KEY_4=P3^6;

void KB_Initialize();
unsigned char KB_ReadKey();

void  KBD_init();
PT_THREAD(KeyboardProcess(struct pt *pt));


#endif
