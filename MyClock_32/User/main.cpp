extern "C" {
#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
}
#include "app.h"
#include "mygpio.h"


void Font_Test(void);


int main(void)
{
	SystemInit();
	delay_init(72);
	delay_ms(1000);
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC ,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_WakeUpPinCmd(ENABLE);//使能WKUP引脚的唤醒功能
	
	MyGPIO_Init(C, 13, GPIO_Mode_Out_PP); // PC13 蓝色LED
	
	MyGPIO_Init(B, 12, GPIO_Mode_Out_OD); // LCD TFT 负极
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	
	MyGPIO_Init(A, 11, GPIO_Mode_Out_PP); // 输出
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	
	MyGPIO_Init(B, 13, GPIO_Mode_Out_PP); // GPS
	GPIO_ResetBits(GPIOB, GPIO_Pin_13); // 关闭
	
	RunApplication();
}



void Font_Test(void)
{
	/*Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(16,10,BLUE,GRAY0,"文字显示测试");

	delay_ms(1000);*/
	Lcd_Clear(GRAY0);
	Gui_DrawFont_GBK16(10,10,RED,GRAY0, (u8*)"This is my test!");	
	delay_ms(1800);
	Gui_DrawFont_GBK16(10,40,BLUE,GRAY0, (u8*)"Hello World");	
	delay_ms(1800);	
}


