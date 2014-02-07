#ifndef CALIBRATE_H
#define CALIBRATE_H
#include "eeprom/eeprom.h"
#include "proto_uso/channels.h"
#include "adc.h"
#include "crc_table.h"

enum 
{
	RESET=0,
	SET=1
} ;


void RestoreCalibrate(void);		 //�������������� ����� ���������� �� EEPROM
float GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code);   //�������������� �������� ��� � �������� ��������� � ��\�^3
void SetCalibration(unsigned char channel,unsigned char type);//�������� ���������� ��� �������
void RestoreSettings(void);
void SetBrightness(unsigned char bri);
//------------------------------------------------------------
#endif