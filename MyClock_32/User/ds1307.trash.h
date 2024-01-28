extern "C" {
#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "MyI2C.h"
}

struct DS1307DateTime {
	uint8_t sec,min,hour,day,mon,week;
	unsigned int year;
};


uint8_t DS1307_Read(uint8_t address);
void DS1307_Wirte(uint8_t address , uint8_t dat);
void DS1307_Set_Time(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec,uint8_t week);
void DS1307_Init_Time(void);
void Get_Now_Time(DS1307DateTime* date_time);




