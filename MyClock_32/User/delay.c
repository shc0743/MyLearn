#include "stm32f10x.h"

#if 0
/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//设置定时器重装值
	SysTick->VAL = 0x00;					//清空当前计数值
	SysTick->CTRL = 0x00000005;				//设置时钟源为HCLK，启动定时器
	while(!(SysTick->CTRL & 0x00010000));	//等待计数到0
	SysTick->CTRL = 0x00000004;				//关闭定时器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 



void delay_us(uint32_t us) {
	Delay_us(us);
}
void delay_ms(uint32_t ms) {
	Delay_ms(ms);
}
void delay_s(uint32_t s) {
	Delay_s(s);
}
#else
#include "delay.h" 
static u8  fac_us=0;//usÑÓÊ±±¶³ËÊý
static u16 fac_ms=0;//msÑÓÊ±±¶³ËÊý
//³õÊ¼»¯ÑÓ³Ùº¯Êý
//SYSTICKµÄÊ±ÖÓ¹Ì¶¨ÎªHCLKÊ±ÖÓµÄ1/8
//SYSCLK:ÏµÍ³Ê±ÖÓ
void delay_init(u8 SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;//bit2Çå¿Õ,Ñ¡ÔñÍâ²¿Ê±ÖÓ  HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(u16)fac_us*1000;
}								    
//ÑÓÊ±nms
//×¢ÒânmsµÄ·¶Î§
//SysTick->LOADÎª24Î»¼Ä´æÆ÷,ËùÒÔ,×î´óÑÓÊ±Îª:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLKµ¥Î»ÎªHz,nmsµ¥Î»Îªms
//¶Ô72MÌõ¼þÏÂ,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//Ê±¼ä¼ÓÔØ(SysTick->LOADÎª24bit)
	SysTick->VAL =0x00;           //Çå¿Õ¼ÆÊýÆ÷
	SysTick->CTRL=0x01 ;          //¿ªÊ¼µ¹Êý  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//µÈ´ýÊ±¼äµ½´ï   
	SysTick->CTRL=0x00;       //¹Ø±Õ¼ÆÊýÆ÷
	SysTick->VAL =0X00;       //Çå¿Õ¼ÆÊýÆ÷	  	    
}   
//ÑÓÊ±nus
//nusÎªÒªÑÓÊ±µÄusÊý.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //Ê±¼ä¼ÓÔØ	  		 
	SysTick->VAL=0x00;        //Çå¿Õ¼ÆÊýÆ÷
	SysTick->CTRL=0x01 ;      //¿ªÊ¼µ¹Êý 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//µÈ´ýÊ±¼äµ½´ï   
	SysTick->CTRL=0x00;       //¹Ø±Õ¼ÆÊýÆ÷
	SysTick->VAL =0X00;       //Çå¿Õ¼ÆÊýÆ÷	 
}


#endif
