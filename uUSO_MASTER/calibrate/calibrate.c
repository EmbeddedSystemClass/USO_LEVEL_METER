#include "calibrate.h"
#include "proto_uso/channels.h"
//------------------------------------------------------------
float GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code)   //преобразование значения АЦП в калиброванное значение	 ??? проверить && debug
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
//void SetFirstPoint(unsigned char channel_num,long ADC_Code,float val)	 //
//{	  
//
//
//		channels[channel_num].calibrate.cal.adc_lo=ADC_Code;
//		channels[channel_num].calibrate.cal.cal_lo=val;
//
//		EEPROM_Write(&channels[channel_num].calibrate.serialize,5,ADC_CALIBRATE_ADDR+channel_num*5);
//	
//		return;
//}
////------------------------------------------------------------
//void SetSecondPoint(unsigned char channel_num,long ADC_Code,float val) // 
//{		
//
//		channels[channel_num].calibrate.cal.adc_hi=ADC_Code;
//		channels[channel_num].calibrate.cal.cal_hi=val;
//	
//
//		EEPROM_Write(&channels[channel_num].calibrate.serialize,5,ADC_CALIBRATE_ADDR+channel_num*5);
//
//
//		return;
//}
//---------------------------------------------------------------
//void Calibrate(unsigned char channel_num,float K,float C) //общая функция калибровки
//{
//		unsigned long true_crc=0;
//		channels[channel_num].calibrate.cal.K= K;
//		channels[channel_num].calibrate.cal.C= C;
//		EEPROM_Write(&channels[channel_num].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel_num*3);
//
//		true_crc= (unsigned long)Calibrate_Get_CRC();//расчет текущей CRC калибровок
//		EEPROM_Write(&true_crc,1,CALIBRATE_DEVICE_CRC_ADDR);//запомним CRC 	
//
//		return;
//}
//---------------------------------------------------------------
//unsigned char Calibrate_Get_CRC(void)//расчет crc  в eerprom
//{
//	 unsigned char crc = 0x0;
//	 unsigned char i=0,j=0;
//
//     for(i=0;i<CHANNEL_NUMBER;i++)
//	 {
//		for(j=0;j<(3*sizeof(unsigned long));j++)
//		{
//			crc = Crc8Table[crc ^ (((unsigned char *)(&channels[i].calibrate.serialize))[j])];
//		}
//	 }
//
//     return crc;
//}
//---------------------------------------------------------------
//void Calibrate_Set_Flag(unsigned char channel,unsigned char flag)//установить/снять флаг калиброванности канала
//{
//    unsigned long true_crc=0;
//
//	if(flag==SET)
//	{
//			channels[channel].calibrate.cal.calibrate=1;//установим/снимем флаг калибровки
//			EEPROM_Write(&channels[channel].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel*3);		
//	}
//	else
//	{
//		if(flag==RESET)
//		{
//			channels[channel].calibrate.cal.calibrate=0;//установим/снимем флаг калибровки
//			EEPROM_Write(&channels[channel].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel*3);
//		}
//	}
//
//  	true_crc= (unsigned long)Calibrate_Get_CRC();//расчет текущей CRC калибровок
//	EEPROM_Write(&true_crc,1,CALIBRATE_DEVICE_CRC_ADDR);//запомним CRC 	
//}
////----------------------------------------------------------------
//void Calibrate_Set_Default(void)//установка калибровки по умолчанию(отключена)
//{
//		unsigned char i=0;
//	    unsigned long true_crc=0;		
//
//		for(i=0;i<CHANNEL_NUMBER;i++)
//		{
//			Calibrate_Set_Flag(i,RESET);
//		}
//		
//		true_crc= (unsigned long)Calibrate_Get_CRC();//расчет текущей CRC калибровок
//		EEPROM_Write(&true_crc,1,CALIBRATE_DEVICE_CRC_ADDR);//запомним CRC 		
//}
////----------------------------------------------------------------