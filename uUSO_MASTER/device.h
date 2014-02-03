/*
структура устройства
*/

#ifndef DEVICE_H
#define DEVICE_H

struct device
{
	unsigned char address;
	unsigned char channel_number;

	//struct *channels;
	//unsigned char name[10];
	//unsigned char version[5];

};

void Device_Save_Address(unsigned char address);
unsigned char Device_Restore_Address(void);

#endif