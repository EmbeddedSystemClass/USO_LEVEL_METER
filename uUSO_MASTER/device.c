#include "device.h"
#include "eeprom/eeprom.h"
#include "preferences.h"
extern volatile unsigned char xdata ADRESS_DEV;
void Device_Save_Address(unsigned char address)
{
	ADRESS_DEV=address&0xF;
	EEPROM_Write(&address,1,DEVICE_ADDR_EEPROM);
}

unsigned char Device_Restore_Address(void)
{
	unsigned long temp_addr;
	EEPROM_Read(&temp_addr,1,DEVICE_ADDR_EEPROM);
	ADRESS_DEV=(unsigned char)((temp_addr>>24)&0xF);
}