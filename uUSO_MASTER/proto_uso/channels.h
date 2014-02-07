#ifndef CHANNELS_H
#define CHANNELS_H
#include "eeprom/eeprom.h"
#include "preferences.h"
//---------------------------------------
#define CHANNEL_NUMBER	6//���������� �������
////---------------------------������---------------------------------
//#define CHNL_ADC	0//����� ���
//	#define CHNL_ADC_FIX_16	0
//	#define CHNL_ADC_SW_16	1
//	#define CHNL_ADC_FIX_24	3
//	#define CHNL_ADC_SW_24	2
//#define CHNL_DOL	1 //����� ���
//	#define CHNL_DOL_ENC	0
//	#define CHNL_DOL_FLOAT	1
//#define CHNL_FREQ	2//���������/�������
//	#define	 CHNL_FREQ_COUNT_T	0
//	#define	 CHNL_FREQ_256		1
//	#define  CHNL_FREQ_PERIOD	3
//	#define	 CHNL_FREQ_COUNT	4
//#define CHNL_UNKNOWN	0xF //����� �� ���������
//	#define CHNL_UNKNOWN_MOD	0


enum //�� ����� ������� �������� ������
{
	SIGNAL_NONE=0,
	SIGNAL_ALL,
	SIGNAL_HI,
	SIGNAL_LO
};

enum
{
	CAL_HI=0,
	CAL_LO,
	UST_HI,
	UST_LO
};
//------------------------------------------------------------------
struct Channel
{
//	unsigned char number ;	  	// ����� ������ 
	
//	union 
//	{
//		struct
//		{
//			unsigned char type;		  // ��� ������
//			unsigned char modific;	  // ����������� ������
//			unsigned char state_byte_1;	// ���� ��������� ������
//			unsigned char state_byte_2; 
//		}set;
//	
//		unsigned long serialize;//��������������� ����� ���������
//	} settings;
	
	unsigned long channel_data;		  // �������� ������
    float  channel_data_calibrate;//������������� �������� ������
	unsigned char string_buf[8];
	unsigned char string_mask[8];
	unsigned char blink;

	union
	{
		struct
		{
			unsigned long  adc_hi;
			float 		   cal_hi;
		
			unsigned long  adc_lo;
			float 		   cal_lo;
			
			float		   ust_hi;
			float		   ust_lo;

			unsigned char signal;//
		} cal;
		unsigned long serialize[7];//
	} calibrate;
	
} ;
//---------------------------------------
void ChannelsInit(void);//������������� ��������� �������
void Store_Channels_Data(void);//�������� ������ ������� � ����
void Restore_Channels_Data(void);//����������� ������ ������� �� ����
void Channels_Set_Default(void);//�������� �������� ������� �� ���������
unsigned char Channels_Get_Settings_CRC(void);//������ crc �������� � eerprom
//---------------------------------------
#endif