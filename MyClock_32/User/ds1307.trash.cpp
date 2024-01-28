#include "ds1307.h"



/*
*********************************************************************************************************
*	函 数 名: uint8_t DS1307_Read(uint8_t address)
*	功能说明: 向DS1307寄存器读取数据
*	形    参：寄存器地址
*	返 回 值: 读出的数据
*********************************************************************************************************
*/
uint8_t DS1307_Read(uint8_t address)
{
	uint8_t dat;	
	MyI2C_Start();
	MyI2C_SendByte(0xD0);//发送设备地址，写指令
	MyI2C_ReceiveAck();
	MyI2C_SendByte(address);//发送寄存器地址
	MyI2C_ReceiveAck();
	MyI2C_Stop();
	MyI2C_Start();//重新启动IIC总线
	MyI2C_SendByte(0xD1);	//发送设备地址，读指令
	MyI2C_ReceiveAck();
	dat=MyI2C_ReceiveByte(); //读一个字节数据
	//MyI2C_NAck();	
	MyI2C_Stop();
	return dat;
}

/*
*********************************************************************************************************
*	函 数 名: DS1307_Wirte(uint8_t address , uint8_t dat)
*	功能说明: 向DS1307寄存器写数据
*	形    参：address：寄存器地址，dat：要写入的数据
*	返 回 值: 无
*********************************************************************************************************
*/
void DS1307_Wirte(uint8_t address , uint8_t dat)
{ 
	MyI2C_Start();
	MyI2C_SendByte(0xD0);//发送设备地址，写指令
	MyI2C_ReceiveAck();
	MyI2C_SendByte(address);//发送寄存器地址
	MyI2C_ReceiveAck();
	MyI2C_SendByte(dat);//写一个字节数据
	MyI2C_ReceiveAck();
	MyI2C_Stop();	
}

//时间初始化设置
void DS1307_Set_Time(uint16_t year,uint8_t mon,uint8_t day,uint8_t hour,uint8_t min,uint8_t sec,uint8_t week)
{
	year-=2000;//年默认2000年开始
	if(year > 100) year = 0;
	
	//十进制转为BCD码
	year = ((year /10) << 4) + year %10;
	mon  = ((mon  /10) << 4) + mon  %10;
	day  = ((day  /10) << 4) + day  %10;
	week = ((week /10) << 4) + week  %10;
	hour = ((hour /10) << 4) + hour %10;
	min  = ((min  /10) << 4) + min  %10;
	sec  = ((sec  /10) << 4) + sec  %10;
	
	//写入寄存器，同时标记一个地址	
	DS1307_Wirte(0x00,sec);    //设置秒
    DS1307_Wirte(0x01,min);    //设置分
    DS1307_Wirte(0x02,hour);   //设置时
    DS1307_Wirte(0x03,week);   //设置周
    DS1307_Wirte(0x04,day);    //设置日
    DS1307_Wirte(0x05,mon);    //设置月	
    DS1307_Wirte(0x06,year);   //设置年		
	DS1307_Wirte(0x10,0x1);   //写入已经设置时间标志	
}
/*
*********************************************************************************************************
*	函 数 名: void DS1307_Init_Time(void)
*	功能说明: 第一次上电时，需要初始化时间，初始化一次后就不必重复初始化
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void DS1307_Init_Time(void)
{
	if(DS1307_Read(0x10)!=0x1)//如果未初始化，执行初始化时间 // 0x10 可以自定义，大于0x06就行
	{
		DS1307_Set_Time(2023,2,16,19,02,00,4);//2023年2月16日 17:50:00 星期四
		//DS1307_Set_Time(2023,2,28,23,58,58,2);//2023年2月28日 23:58:58 星期2
		Gui_DrawFont_GBK16(10,70,WHITE,GREEN, (u8*)"[OK] DS1307: INIT1");
		//printf("时间初始化完成,flag=%d\r\n",DS1307_Read(0x10));
	}	else{
		Gui_DrawFont_GBK16(10,70,WHITE,GREEN, (u8*)"[OK] DS1307: INIT0");
		//printf("时间已初始化过,flag=%d\r\n",DS1307_Read(0x10));
	}
}


//获取当前时间
void Get_Now_Time(DS1307DateTime* date_time)
{
	uint8_t sec,min,hour,week,day,mon,year;
	//uint8_t i;
	
	sec  = DS1307_Read(0x00);
	min  = DS1307_Read(0x01);
	hour = DS1307_Read(0x02);
	week = DS1307_Read(0x03);	
	day  = DS1307_Read(0x04);
	mon  = DS1307_Read(0x05);
	year = DS1307_Read(0x06);
	
	//BCD码转十进制
	date_time->sec  = (sec/16)*10 + sec%16;
	date_time->min  = (min/16)*10 + min%16;
	date_time->hour = (hour/16)*10 + hour%16;
	date_time->day  = (day/16)*10  + day%16;
	date_time->mon  = (mon/16)*10  + mon%16;
	date_time->year = (year/16)*10 + year%16 + 2000;
	date_time->week = (week/16)*10 + week%16;   			
}



