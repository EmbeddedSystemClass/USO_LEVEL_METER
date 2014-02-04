#include "proto_tablo.h"

#include "calibrate\calibrate.h"
#include <stdio.h>
#include <intrins.h>
#include "crc_table.h"
#include "proto_uso\channels.h"
#include "watchdog.h"
//-----------------------------------------------------------------------------------

sbit DE_RE=P3^5;

//-----------------------------------------------------------------------------------

volatile unsigned char xdata ADRESS_DEV=0xF;//табло

volatile unsigned char xdata	transf_count;//счетчик передаваемых байтов	   
volatile unsigned char xdata	buf_len;//длина передаваемого буфера

//------------------------флаги ошибок--------------------------------
volatile unsigned char idata  CUT_OUT_NULL;//флаг-вырезаем 0 после 0xD7
volatile unsigned char xdata frame_len=0;//длина кадра, которую вытаскиваем из шестого байта кадра
//--------------------------------------------------------------------

volatile unsigned char xdata  TransferBuf[MAX_LENGTH_TR_BUF]={0} ; //буфер передаваемых данных
//--------------------------------------------------------------------
volatile unsigned char xdata  STATE_BYTE=0xC0;//байт состояния устройства
volatile unsigned char idata symbol=0xFF;//принятый символ

volatile struct pt pt_proto;
//-----------------------------------------------------------------------------------

void UART_ISR(void) interrupt 4 //using 1
{	
	EA=0;	//запрет прерывания

	if(TI)
	{
		TI=0;
		 
		if(transf_count<buf_len)
		{
			if(transf_count<3)//передаем заголовок
			{
				SBUF=TransferBuf[transf_count];			
				transf_count++;
			}
			else   //тело...   подставляем 0 после 0xD7
			{
					if(CUT_OUT_NULL==0)
					{
						if(TransferBuf[transf_count]==(unsigned char)0xD7)//проверим, это  ,0xD7 или другое
						{			
							CUT_OUT_NULL=0x1;	
						}
						SBUF=TransferBuf[transf_count];			
						transf_count++;
					}
					else
					{
						SBUF=(unsigned char)0x0;
						CUT_OUT_NULL=0;		
					}	
			}	
		}
		else
		{
			transf_count=0;		//обнуляем счетчик
			CUT_OUT_NULL=0;
//			PT_RESTART_OUT(pt_proto);  //внепроцессный рестарт			
		}					   
	}			
	EA=1;
	return;
}
//------------------------------------------------------------------------------
void Protocol_Init(void) //using 0
{
	TI=0;
	RI=0;
	

	transf_count=0x0;//счетчик передаваемых байтов
	buf_len=0x0;//длина передаваемого буфера
	DE_RE=1;//линия на на передачу
	CUT_OUT_NULL=0;
//	STATE_BYTE=0xC0;
//	PT_INIT(&pt_proto);
	return;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
const unsigned char tablo_channels[CHANNEL_NUMBER]={0,1,2,3,4,5};//таблица номеров каналов индикаторов табло, вставить правильные
unsigned char Tablo_Output_Frame(void)
{
   unsigned char i=0;
   unsigned char counter=0;//счетчик символов кадра
  // unsigned char chr_buffer[8];
  // float value;
  
   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // адрес узла
   TransferBuf[4]=CHANNEL_SET_PARAMETERS_REQ;  // код операции
  // TransferBuf[5]=counter-6;//длина оставшейся части кадра

   TransferBuf[6]=0x0;//номер канала
   TransferBuf[7]=0x8F;//формат канала
  // TransferBuf[8]=counter-9;//длина информации по каналу
   //формирование канала

   TransferBuf[9]=':';//заголовок кадра табло
  // TransferBuf[10]=counter-15;//длина кадра табло
   counter=11;
   for(i=0;i<CHANNEL_NUMBER;i++)
   {
   		TransferBuf[counter]='[';
		counter++;
		TransferBuf[counter]=tablo_channels[i];
		counter++;
		//value=GetCalibrateVal(i,channels[i].channel_data);
//		value= (float)channels[i].channel_data/0xFFFFFF*10.225;
	 
//	  if(_chkfloat_ (value)>1) 
//	  {
//		   counter+=sprintf(&TransferBuf[counter],"Err");
//	  }
//	  else  
//	  {
//		   counter+=sprintf(&TransferBuf[counter],"%3.2f",value);
//	  }

		counter+=sprintf(&TransferBuf[counter],"%s",channels[i].string_buf);	

		TransferBuf[counter]=']';
		counter++;		
   }

   		TransferBuf[counter]='[';
		counter++;

		TransferBuf[counter]='*';
		counter++;

   		TransferBuf[counter]=0xF;
		counter++;
		
		TransferBuf[counter]=']';
		counter++;	

   TransferBuf[counter]=0x1;//номер канала
   counter++;
   TransferBuf[counter]=0x41;//формат канала
   counter++;
   TransferBuf[counter]=0x0;//тип сигнала	-пока нет сигнала
   counter++;
   TransferBuf[counter]=CRC_Check(TransferBuf,counter);
   counter++;

   TransferBuf[5]=counter-6;//длина оставшейся части кадра
   TransferBuf[8]=counter-9;//длина информации по каналу
   TransferBuf[10]=counter-15;//длина кадра табло

	DE_RE=1; //переключаем RS485 в режим передачи
					
	REN=0;	//запрет приема-только передача
	transf_count=0;
	buf_len=counter;
	CUT_OUT_NULL=0;
	SBUF=TransferBuf[transf_count];//передача байта, остальным займется автомат
	transf_count++;//инкрементируем счетчик переданных
	ES=1; //включим прерывание уарт	
   return counter;
//	
}
//-----------------------------------------------------------------------------------
//#pragma OT(0,Speed) 
//PT_THREAD(ProtoProcess(struct pt *pt))
// {
//
//  PT_BEGIN(pt);
//
//  while(1) 
//  {
//  //----------restart------------
////		recieve_count=0x0;//??
//////		REN=1;//recieve enqble
////		DE_RE=1;//
////		ES=1;
//  //-----------------------------
//	  // PT_WAIT_UNTIL(pt,RECIEVED); //ждем команды на старт
//	   wdt_count[Proto_Proc].process_state=IDLE;
//
////	   PT_YIELD_UNTIL(pt,RECIEVED); //ждем команды на старт	
//	   wdt_count[Proto_Proc].count++;
//
//			buf_len=Tablo_Create_Frame();
//			
//			DE_RE=1; //переключаем RS485 в режим передачи
//							
//			REN=0;	//запрет приема-только передача
//			transf_count=0;
//			CUT_OUT_NULL=0;
//			SBUF=TransferBuf[transf_count];//передача байта, остальным займется автомат
//			transf_count++;//инкрементируем счетчик переданных
//			ES=1; //включим прерывание уарт	
//
//			PT_DELAY(pt,500);			
////		}
//
//		
//  //-----------------------------
//  }
//
// PT_END(pt);
//}

