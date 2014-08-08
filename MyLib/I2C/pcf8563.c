#include "includes.h"

#define PCF8563_I2C_PORT 0

#define BIN_TO_BCD(val) (((val/10) << 4) | (val % 10))
#define BCD_TO_BIN(val) (((val)&15) + ((val)>>4)*10)
#define RTC_START() do{ INT8U temp = 0x00; ISendStr(PCF8563_I2C_PORT, 0xA2, 0, 1, &temp, 1); }while(0)

INT8S pcf8563_read(TIME* ptime)
{
	INT8U time_ram[10];
	INT8U sub_addr, mode, year;

	if(NULL == ptime){
		return -1;
	}

	mode = 1;
	sub_addr = 0x02;
	
	if(IRcvStr(PCF8563_I2C_PORT, 0xA2, sub_addr, mode, time_ram, 7) == false){
		return 	-1;
	}
	
	ptime->second = BCD_TO_BIN(time_ram[0] & 0x7F);
	ptime->minute = BCD_TO_BIN(time_ram[1] & 0x7F);
	ptime->hour   = BCD_TO_BIN(time_ram[2] & 0x3F);
	ptime->day    = BCD_TO_BIN(time_ram[3] & 0x3F);
	ptime->week   = BCD_TO_BIN(time_ram[4] & 0x07);
	ptime->month  = BCD_TO_BIN(time_ram[5] & 0x1F);
	
	// 世纪判断
	if(time_ram[5]&(0x80)){
		ptime->year = 1900;
	}else{
		ptime->year = 2000;
	}
	year = BCD_TO_BIN(time_ram[6]);
	ptime->year += year; 

	return 0;
}

INT8S pcf8563_write(TIME* ptime)
{
	INT8U setbuf[7];
	INT8U mode,sub_addr,year;
	
	if(NULL == ptime){
		return -1;
	}

	setbuf[0] = BIN_TO_BCD(ptime->second);
	setbuf[1] = BIN_TO_BCD(ptime->minute);
	setbuf[2] = BIN_TO_BCD(ptime->hour);
	setbuf[3] = BIN_TO_BCD(ptime->day);
	setbuf[4] = BIN_TO_BCD(ptime->week);

	// 判断世纪
	if(ptime->year >= 2000)
	{
		year =  ptime->year - 2000;
		setbuf[5] = BIN_TO_BCD(ptime->month);
		setbuf[6] = BIN_TO_BCD(year);
	}
	else
	{
		year = 	ptime->year - 1900;
		setbuf[5] = BIN_TO_BCD(ptime->month) | 0x80;
		setbuf[6] = BIN_TO_BCD(year);
	}

	mode = 1;
	sub_addr = 0x02;
	if(ISendStr(PCF8563_I2C_PORT, 0xA2, sub_addr, mode, (INT8U *)setbuf, 7) == false){
		return -1;	
	}

	RTC_START();

	return 0;
}

INT8S pcf8563_init(void)
{
	INT8U mode,dat,sub_addr;

	mode = 1;
	sub_addr = 0x02;
	if(IRcvStr(PCF8563_I2C_PORT, 0xA2, sub_addr, mode, &dat, 1) == false){
		return -1;
	}

	return 0;
}
