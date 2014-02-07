#include "calibrate.h"
#include "proto_uso/channels.h"
//------------------------------------------------------------
float GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code)   //преобразование значени€ ј÷ѕ в калиброванное значение	 ??? проверить && debug
{
	return 	((((float)((float)ADC_Code-(float)channels[channel_num].calibrate.cal.adc_lo)/(float)((float)channels[channel_num].calibrate.cal.adc_hi-(float)channels[channel_num].calibrate.cal.adc_lo))*(channels[channel_num].calibrate.cal.cal_hi-channels[channel_num].calibrate.cal.cal_lo)))+channels[channel_num].calibrate.cal.cal_lo; //
}
//------------------------------------------------------------
void RestoreCalibrate(void)		 //восстановление точек калибровки из EEPROM
{
	unsigned char i=0;

	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Read(&channels[i].calibrate.serialize[0],7,ADC_CALIBRATE_ADDR+i*7);
	}

	return;
}
//------------------------------------------------------------
void SetCalibration(unsigned char channel,unsigned char type)//запомним калибровку или уставку
{
	switch(type)
	{
		case CAL_HI:
		{
			channels[channel].calibrate.cal.adc_hi=channels[channel].channel_data;
			EEPROM_Write(&channels[channel].calibrate.cal.adc_hi,1,ADC_CALIBRATE_ADDR+channel*7);
			EEPROM_Write(&channels[channel].calibrate.cal.cal_hi,1,ADC_CALIBRATE_ADDR+channel*7+1);
		}
		break;

		case CAL_LO:
		{
			channels[channel].calibrate.cal.adc_lo=channels[channel].channel_data;
			EEPROM_Write(&channels[channel].calibrate.cal.adc_lo,1,ADC_CALIBRATE_ADDR+channel*7+2);
			EEPROM_Write(&channels[channel].calibrate.cal.cal_lo,1,ADC_CALIBRATE_ADDR+channel*7+3);
		}
		break;

		case UST_HI:
		{
			EEPROM_Write(&channels[channel].calibrate.cal.ust_hi,1,ADC_CALIBRATE_ADDR+channel*7+4);
		}
		break;

		case UST_LO:
		{
			EEPROM_Write(&channels[channel].calibrate.cal.ust_lo,1,ADC_CALIBRATE_ADDR+channel*7+5);
		}
		break;
	}
}
//------------------------------------------------------------
extern unsigned char brightness;
void RestoreSettings(void)
{
	unsigned long temp;
	EEPROM_Read(&temp,1,ADC_BRIGHTNESS_ADDR);
	brightness=(unsigned char)((temp>>24)&0xF);
}

void SetBrightness(unsigned char bri)
{
	bri=bri&0xF;
	EEPROM_Write(&bri,1,ADC_BRIGHTNESS_ADDR); 
}