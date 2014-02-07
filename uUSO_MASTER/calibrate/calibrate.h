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


void RestoreCalibrate(void);		 //восстановление точек калибровки из EEPROM
float GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code);   //преобразование значения АЦП в значение плотности в кг\м^3
void SetCalibration(unsigned char channel,unsigned char type);//запомним калибровку или уставку
void RestoreSettings(void);
void SetBrightness(unsigned char bri);
//------------------------------------------------------------
#endif