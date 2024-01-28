extern "C" {
#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "MyRTC.h"
#include "AT24Cxx.h"
#include "Serial.h"
}
#include "app.h"
#include "mygpio.h"
#include "MyDateTime.h"
#include "MyAlarm.h"
#include "Menu.h"
#include "apps_ext.h"

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>


#define DEV 0 /*开发模式,最大程度减少文件体积*/


volatile int my_lcd_mode = 2;

static bool isSafedEnv = false;
static int nUiTimeShowType = 0;
int my_ext_gpsTimeUpdateService = -1;
short my_TimeZone = +8;

extern "C" unsigned char DebugMode = 0;


MyDateTime rtcDate;

void KeyboardInit();




void WaitKey(GPIO_TypeDef * gpio, uint16_t pin, bool useSTOPMode) { // A10 = OK
	while (1) {
		if (useSTOPMode) {
			PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
			SystemInit();
		} else __wfi();
		delay_us(1);
		
		uint8_t key = !GPIO_ReadInputDataBit(gpio, pin);
		if (key) {
			MenuBase::AntiTik(gpio, pin);
			break;
		}
	}
}



void RunApplication() {
	delay_ms(500);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	
	Lcd_Init(my_lcd_mode);
	delay_ms(500);
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	
	Lcd_Clear(GRAY0);
	LCD_LED_SET;
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	Gui_DrawFont_GBK16(10,10,RED,GRAY0, (u8*)"Initializing...");
	Gui_DrawLine(0, 28, 160, 28, BLACK);
	Gui_DrawFont_GBK16(10,30,BLUE,GRAY0, (u8*)"Startup Log:");
	delay_ms(100);
	//GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	
	/*int bit = 0;
	while (1) {
		delay_ms(100);
		if (bit) GPIO_SetBits(GPIOC, GPIO_Pin_13);
		else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		if (bit) bit = 0; else bit = 1;
	}*/
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	MyRTC_Init();
	Gui_DrawFont_GBK16(10,50,WHITE,GREEN, (u8*)"[OK] RTC");
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	KeyboardInit();
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	AT24CXX_Init();
	Gui_DrawFont_GBK16(10,90,WHITE,GREEN, (u8*)"[OK] AT24Cxx");
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	MyAlarm_Init();
	Gui_DrawFont_GBK16(10,110,WHITE,GREEN, (u8*)"[OK] MyAlarm");
	#if 0
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	Serial_Init();
	Gui_DrawFont_GBK16(10,110,WHITE,GREEN, (u8*)"[OK] Serial usart");
	#endif
	
	nUiTimeShowType = AT24CXX_ReadOneByte(0x11);
	
	
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	
	for (unsigned int n = 3; n; --n) {
		//char ccs[] = "2147483647";
		//sprintf(ccs, "%u", n);
		//Gui_DrawFont_GBK16(150,30,BLUE,WHITE, (u8*)ccs);
		Gui_DrawFont_Num32(94, 10, BLUE, WHITE, (u16)(n / 10));
		Gui_DrawFont_Num32(120, 10, BLUE, WHITE, (u16)(n % 10));
		delay_ms(1000); 
	}
	Lcd_Clear(WHITE);
	
	while (1) {
		/*Lcd_Init(mode); 
		if (mode == 1) mode = 2;
		else mode = 1;
		LCD_LED_SET;//ͨ��IO���Ʊ�����
		Lcd_Clear(GRAY0);
		Font_Test();
		delay_ms(1200);
		LCD_LED_CLR;//IO���Ʊ�����
		delay_ms(1200);*/
		
		ShowAppMenu();
		delay_s(1);
	
	}
}



static bool shouldPauseMainMenu = false;
extern unsigned int nUIState;
void RunUIHandler();


void ShowAppMenu() {
	
	//delay_s(2);
	Lcd_Clear(WHITE);
	
	
	char timestr[] = "00:00:00";
	char datestr[] = "1970-01-01";
	Lcd_Clear(BLACK);
	while (true) {
		Delay_ms(50);
		//__wfi();
		if (shouldPauseMainMenu || nUIState != 0) {
			RunUIHandler();
			Lcd_Clear(BLACK);
			continue;
		}

		
		MyRTC_ReadTime();
		rtcDate.year = MyRTC_Time[0], rtcDate.mon = MyRTC_Time[1], rtcDate.day = MyRTC_Time[2];
		rtcDate.hour = MyRTC_Time[3], rtcDate.min = MyRTC_Time[4], rtcDate.sec = MyRTC_Time[5];

		//Gui_DrawFont_Num32 (10,10,BLACK,GRAY0, (++num) % 10);
		sprintf(timestr, "%02d:%02d:%02d", rtcDate.hour, rtcDate.min, rtcDate.sec);
		if (nUiTimeShowType == 1) {
			Gui_DrawFont_Num32(10+5, 40, WHITE, BLACK, u16(rtcDate.hour / 10));
			Gui_DrawFont_Num32(42+5, 40, WHITE, BLACK, u16(rtcDate.hour % 10));
			Gui_DrawFont_Num32(80+5, 40, WHITE, BLACK, u16(rtcDate.min  / 10));
			Gui_DrawFont_Num32(112+5,40, WHITE, BLACK, u16(rtcDate.min  % 10));
		} else {
			Gui_DrawFont_GBK16(45, 40, WHITE, BLACK, (u8*)timestr);
		}
		sprintf(datestr, "%04d-%02d-%02d", rtcDate.year, rtcDate.mon, rtcDate.day);
		Gui_DrawFont_GBK16(37, 90, WHITE, BLACK, (u8*)datestr);
		
		//Gui_DrawFont_GBK16(10, 30, BLUE, WHITE, (u8*)"A0=");
		//Gui_DrawFont_GBK16(50, 30, RED, WHITE, (u8*)((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) ? "1" : "0"));
		
		
		if (my_ext_gpsTimeUpdateService > 0) {
			GpsTimeUpdateService();
		}
	}
}



void KeyboardInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	
	//Gui_DrawFont_GBK16(10,30,RED,GRAY0, (u8*)"Hello 2026!");
	
	// K1 =  B14
	
	MyGPIO_Init(B, 14, GPIO_Mode_IPU); /* IPD bu guan zen mo xie dou mei you yong  fuck u IPD */
	MyGPIO_Init(B, 15, GPIO_Mode_IPU);
	MyGPIO_Init(A, 8, GPIO_Mode_IPU);
	MyGPIO_Init(A, 9, GPIO_Mode_IPU);
	MyGPIO_Init(A, 10, GPIO_Mode_IPU);
	MyGPIO_Init(A, 0, GPIO_Mode_IPU);
	
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource9);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line8;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_Init(&EXTI_InitStructure);
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	
	Gui_DrawFont_GBK16(10,70,WHITE,GREEN, (u8*)"[OK] Keyboard");
	
}





unsigned int nUIState = 0;


void DrawAppMenu(char arg1) {
	static unsigned short nMenuState = 0;
	if (DebugMode) Gui_DrawFont_Num32(0, 0, YELLOW, WHITE, nMenuState);
	
	if (arg1 == 2) {
		uint8_t b14, b15, a8, a9, a10;
		while (1) {
			__WFI();
			delay_us(1);
			
			if (nMenuState == 0) nMenuState = 1;
			
			b14 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
			b15 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
			a8 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
			a9 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);
			a10 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);
			
			if (b14) { // <
				if (--nMenuState < 1) nMenuState = 4;
				DrawAppMenu(0);
				delay_ms(100);
				while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
				break;
			}
			if (b15) { // >
				if (++nMenuState > 4) nMenuState = 1;
				DrawAppMenu(0);
				delay_ms(100);
				while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);
				break;
			}
			if (a10) { // OK
				bool shouldBreak = true;
				switch (nMenuState) {
					case 1:
						nUIState = 0x31; break;
					case 2:
						nUIState = 0x32; break;
					case 3:
						nUIState = 0x33; break;
					case 4:
						a8 = a9 = 1; shouldBreak = false; break;
					default:;
				}
					
				
				delay_ms(100);
				while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0);
				
				if (shouldBreak) nMenuState = 0;
				if (shouldBreak) break;
			}
			if (a8 || a9) { // +/-
				shouldPauseMainMenu = false;
				nUIState = nMenuState = 0;
				break;
			}
		}
		
		return;
	}
	
	if (arg1 == 1) {
		if (nUIState != 0) return;
	
		shouldPauseMainMenu = true;
		nUIState = 0x10;
		//nMenuState = 0;
		
		return;
	}
	
	if (arg1 == 3) {
		nMenuState = 0;
	}
	
	switch (nMenuState) {
		case 0: {
			Lcd_Clear(GRAY0);
			
			Gui_DrawFont_GBK16(10, 8, BLACK, GRAY0, (u8*)"Clock");
			Gui_DrawLine(0, 28, 160, 28, BLACK);
			
			DisplayButtonUp(10, 40, 150, 80);
			Gui_DrawFont_GBK16(35, 53, BLUE, GRAY0, (u8*)"Application");
			
			DisplayButtonUp(10, 90, 50, 120);
			Gui_DrawFont_GBK16(15, 97, BLUE, GRAY0, (u8*)"Sync");
			
			DisplayButtonUp(70, 90, 150, 120);
			Gui_DrawFont_GBK16(80, 97, BLUE, GRAY0, (u8*)"Settings");
			
			DisplayButtonUp(135, 5, 155, 25);
			Gui_DrawFont_GBK16(141, 6, RED, GRAY0, (u8*)"x");
			
			nMenuState = 1;
			DrawAppMenu(0);
		}
		
		#if 0
		case 1: {
			DisplayButtonDown(10, 40, 150, 80);
			DisplayButtonUp(10, 90, 50, 120);
			DisplayButtonUp(70, 90, 150, 120);
			DisplayButtonUp(135, 5, 155, 25);
			break;
		}
		
		case 2: {
			DisplayButtonUp(10, 40, 150, 80);
			DisplayButtonDown(10, 90, 50, 120);
			DisplayButtonUp(70, 90, 150, 120);
			DisplayButtonUp(135, 5, 155, 25);
			break;
		}
		
		case 3: {
			DisplayButtonUp(10, 40, 150, 80);
			DisplayButtonUp(10, 90, 50, 120);
			DisplayButtonDown(70, 90, 150, 120);
			DisplayButtonUp(135, 5, 155, 25);
			break;
		}
		
		case 4: {
			DisplayButtonUp(10, 40, 150, 80);
			DisplayButtonUp(10, 90, 50, 120);
			DisplayButtonUp(70, 90, 150, 120);
			DisplayButtonDown(135, 5, 155, 25);
			break;
		}
	
		default:;
		#else
		default: {
			DisplayButtonWithState(10, 40, 150, 80, nMenuState == 1);
			DisplayButtonWithState(10, 90, 50, 120, nMenuState == 2);
			DisplayButtonWithState(70, 90, 150, 120, nMenuState == 3);
			DisplayButtonWithState(135, 5, 155, 25, nMenuState == 4);
		}
		#endif
	}
	
	
}


MenuData SubMenu_Application[] = {
	//id, text0, text1, text2
	{ 0x02, "Alarm Manager", 0, 0, },
	{ 0x03, "In-flash", "Text Viewer", 0, },
	{ 0x04, "In-flash", "Picture Viewer", 0, },
	{ 0x05, "Timer", 0, 0, },
	{ 0x06, "Time Countdown", 0, 0, },
	{ 0x07, "Development", "Toolkit", 0, },
	{ 0x08, "Factory Options", 0, 0, },
	{ 0x09, "About,", "License,", "Copyright", },
	{ 0x0A, "Debug Mode:   off", 0, 0, },
};
#include "Furina1.h"
#include "Furina2.h"
void DrawSubMenu_Application(char arg1) {
	//static unsigned short nMenuState = 0;
	static MenuRoutine myMenu;
	if (!myMenu.isInited()) {
		myMenu.InitMenu(SubMenu_Application, sizeof(SubMenu_Application)/sizeof(MenuData)); // 初始化菜单
		myMenu.ui_Title = "Application";
		myMenu.ui_menuColor = 0x1A237E;
		myMenu.ui_ShowCloseButton = true;
	}
	
	if (arg1 == 2) {
		//uint8_t b14, b15, a8, a9, a10;
		unsigned state = 0; char ret = 0; bool breakParent = false;
		while (1) {
			#if 0
			__WFI();
			delay_us(1);
			
			b14 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
			b15 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
			a8 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
			a9 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);
			a10 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);
			
			if (b14) { // <
				if (--nMenuState < 1) nMenuState = 0xA;
				DrawSubMenu_Application(0);
				delay_ms(100);
				while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0);
				break;
			}
			if (b15) { // >
				if (++nMenuState > 0xA) nMenuState = 1;
				DrawSubMenu_Application(0);
				delay_ms(100);
				while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0);
				break;
			}
			if (a10) { // OK
				bool shouldBreak = true;
				switch (nMenuState) {
					case 1:
						a8 = a9 = 1; shouldBreak = false; break;
					case 0xA:
						DebugMode = !DebugMode; DrawSubMenu_Application(0); break;
					default:;
				}
				
				delay_ms(100);
				while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0);
				
				if (shouldBreak) break;
			}
			if (a8 || a9) { // +/-
				//if (DebugMode) Gui_Circle(40, 40, 10, GREEN);
				nUIState = 0x10;
				break;
			}
			#endif
			
			#if 1
			ret = myMenu.GetRetValueFromMenu(&state);
			if (0 == (ret & 0x0f)) break;
			if (0 == (ret & 0xf0)) state = 1;
			
			breakParent = false;
			switch (state) {
				case 1: {
					nUIState = 0x10;
					breakParent = true;
					break;
				}
				
				case 2: {
					nUIState = 0x101;
					breakParent = true;
					break;
				}
				
				case 3: {
					if (!isSafedEnv) break;
					Lcd_Clear(GRAY0);
					Gui_DrawFont_GBK16_Spec(10, 10, BLUE, GRAY0, (u8*)"\x6c\x38\x8f\xdc\x55\x9c\x6b\x22\x82\x99\x5b\x81\x5a\x1c\x59\x27\x4e\xba");//永远喜欢芙宁娜大人
					Gui_DrawFont_GBK16(10, 30, BLUE, GRAY0, (u8*)"What a cute Hydro");
					Gui_DrawFont_GBK16(10, 46, BLUE, GRAY0, (u8*)"Archon Furina!!!!");
					Gui_DrawFont_GBK16(10, 70, BLUE, GRAY0, (u8*)"I love Focalors");
					Gui_DrawFont_GBK16(40, 86, BLUE, GRAY0, (u8*)"forever");
					Gui_DrawFont_GBK16(10, 112, WHITE, BLUE, (u8*)"4.0 Fontaine");
					WaitKey(GPIOA, GPIO_Pin_10); // ok
					Lcd_Clear(GRAY0);
					Gui_DrawFont_GBK16(12, 30, BLUE, GRAY0, (u8*)" Genshin Impact ");
					Gui_DrawFont_GBK16(10, 46, WHITE, BLUE, (u8*)"Step Into a Vast");
					Gui_DrawFont_GBK16(10, 62, WHITE, BLUE, (u8*)"Magical World of");
					Gui_DrawFont_GBK16(10, 78, WHITE, BLUE, (u8*)"   Adventure    ");
					WaitKey(GPIOA, GPIO_Pin_10); // ok
					Lcd_Clear(GRAY0);
					Gui_DrawFont_GBK16(10, 30, WHITE, BLUE, (u8*)"GenshinImpact4.0");
					Gui_DrawFont_GBK16(10, 46, BLUE, GRAY0, (u8*)"  New Nation:");
					Gui_DrawFont_GBK16(10, 62, BLUE, GRAY0, (u8*)"   Fontaine");
					WaitKey(GPIOA, GPIO_Pin_10); // ok
					Lcd_Clear(GRAY0);
					Gui_DrawFont_GBK16(10, 30, WHITE, BLUE, (u8*)"Focalors (Furina)");
					Gui_DrawFont_GBK16(10, 46, BLUE, GRAY0, (u8*)"  Hydro Archon");
					Gui_DrawFont_GBK16(10, 62, BLUE, GRAY0, (u8*)"The god of justice");
					WaitKey(GPIOA, GPIO_Pin_10); // ok
					Lcd_Clear(GRAY0);
					Gui_DrawFont_GBK16(12, 30, WHITE, BLUE, (u8*)"      v4.0      ");
					Gui_DrawFont_GBK16(5, 46, BLUE, GRAY0, (u8*)"As Light Rain Falls");
					Gui_DrawFont_GBK16(10, 62, BLUE, GRAY0, (u8*)" Without Reason");
					WaitKey(GPIOA, GPIO_Pin_10); // ok
					Lcd_Clear(GRAY0);
					Gui_DrawFont_GBK16(10, 30, WHITE, BLUE, (u8*)"https://genshin.");
					Gui_DrawFont_GBK16(10, 46, WHITE, BLUE, (u8*)"hoyoverse.com/  ");
					
					WaitKey(GPIOA, GPIO_Pin_10); // ok
					myMenu.DrawMenu(true);
					break;
				}
				
				case 4: {
					if (!isSafedEnv) break;
					//取模方式 水平扫描 从左到右 低位在前
					const unsigned char *p = 
					#if DEV
					0
					#else
					gImage_Furina1
					#endif
					;
					//显示160*128 图片  140x112
					int i,j,k; 
					unsigned char picH,picL;
					Lcd_Clear(WHITE); //清屏  
	
					for (k = 0; k < 1; ++k) {
						for (j = 0; j < 1; ++j) {
							//Lcd_SetRegion(40*j+2,40*k,40*j+39,40*k+39);		//坐标设置
							//Lcd_SetRegion(10, 6, 150, 122);
							Lcd_SetRegion(0, 0, 160, 128);
							for(i = 0; i < (160*128); ++i) {	
								picL=*(p+i*2);	//数据低位在前
								picH=*(p+i*2+1);				
								LCD_WriteData_16Bit(picH<<8|picL);  						
							}	
						}
					}
					
					WaitKey(GPIOA, GPIO_Pin_10, true); // ok
					
					myMenu.DrawMenu(true);
					break;
				}
				
				case 5: {
					app_Timer();
					myMenu.DrawMenu(true);
					break;
				}
				
				case 8: {
					nUIState = 0x2001;
					breakParent = true;
					break;
				}
				
				case 0xa: {
					DebugMode = !DebugMode;
					sprintf(SubMenu_Application[0xA - 2].szText0, "Debug Mode:   o%s", DebugMode ? "n" : "ff");
					myMenu.DrawMenu();
					break;
				}
				
				default:;
			}
			
			if (breakParent) break;
			#endif
		}
		
		return;
	}
	
	
	
	if (arg1 == 1) {
		myMenu.DrawMenu(true);
		//nMenuState = 2;
	}
	
	
	
	return;
	
	
	
	
	#if 0
	
	if (arg1 == 1) {
		Lcd_Clear(GRAY0);
		
		Gui_DrawFont_GBK16(10, 8, BLACK, GRAY0, (u8*)"Application");
		Gui_DrawLine(0, 28, 160, 28, BLACK);
		
		DisplayButtonUp(135, 5, 155, 25);
		Gui_DrawFont_GBK16(141, 6, RED, GRAY0, (u8*)"x");
		
		Gui_DrawFont_ASC16Ex(0, 128-32, WHITE, BLACK, (u8*)"<> Switch Item        ", false);
		Gui_DrawFont_ASC16Ex(0, 128-16, WHITE, BLACK, (u8*)"[OK] Enter Item       ", false);
		
		nMenuState = 2;
	}
	
	DisplayButtonWithState(135, 5, 155, 25, nMenuState == 1);
	
	#if 0
	u16 offset = std::max(0, nMenuState - 4) * (-40);
	
	if (nMenuState < 0x5 && nMenuState > 0x1) Gui_DrawFont_ASC16Ex(10, offset + 40, BLACK, WHITE, (u8*)"Alarm Manager", nMenuState == 2);
	if (nMenuState < 0x6 && nMenuState > 0x2) Gui_DrawFont_ASC16Ex(10, offset + 70, BLACK, WHITE, (u8*)"In-flash text", nMenuState == 3);
	if (nMenuState < 0x7 && nMenuState > 0x3) Gui_DrawFont_ASC16Ex(10, offset + 100, BLACK, WHITE, (u8*)"In-flash image", nMenuState == 4);
	if (nMenuState < 0x8 && nMenuState > 0x4) Gui_DrawFont_ASC16Ex(10, offset + 130, BLACK, WHITE, (u8*)"Timer", nMenuState == 5);
	if (nMenuState < 0x9 && nMenuState > 0x5) Gui_DrawFont_ASC16Ex(10, offset + 160, BLACK, WHITE, (u8*)"Time Countdown", nMenuState == 6);
	if (nMenuState < 0xA && nMenuState > 0x6) Gui_DrawFont_ASC16Ex(10, offset + 190, BLACK, WHITE, (u8*)"About&Copyright", nMenuState == 7);
	#endif
	
	Gui_Fill(0, 29, 160, 67, GRAY1);
	const u16 bgColor = RGB6toRGB565(63, 81, 181);
	switch (nMenuState) {
		case 0x2:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"Alarm Manager", true);
			break;
		
		case 0x3:
		case 0x4:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"In-flash", true);
			Gui_DrawFont_ASC16Ex(10, 56, bgColor, WHITE, (u8*)(nMenuState == 0x3 ? "Text Viewer" : "Picture Viewer"), true);
			break;
		
		case 0x5:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"Timer", true);
			break;
		
		case 0x6:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"Time Countdown", true);
			break;
		
		case 0x7:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"Development", true);
			Gui_DrawFont_ASC16Ex(10, 56, bgColor, WHITE, (u8*)"Toolkit", true);
			break;
		
		case 0x8:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"Factory Options", true);
			break;
		
		case 0x9:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"About,", true);
			Gui_DrawFont_ASC16Ex(10, 56, bgColor, WHITE, (u8*)"License,", true);
			Gui_DrawFont_ASC16Ex(10, 56+16, bgColor, WHITE, (u8*)"Copyleft", true);
			break;
		
		case 0xA:
			Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)"Debug Mode:    ", true);
			Gui_DrawFont_ASC16Ex(110, 40, bgColor, WHITE, (u8*)(DebugMode ? "on" : "off"), true);
			break;
	}
	#endif
}


MenuData SubMenu_Factory[] = {
	//id, text0, text1, text2
	{ 0x02, "1. Test:", "Sound Test", 0, },
	{ 0x03, "2. Test:", "Option Test", "(0x03)", },
	{ 0x04, "3. Factory:", "Clear AT24C64", 0, },
};
void DrawSubMenu_Factory(char arg1) {
	//static unsigned short nMenuState = 0;
	static MenuRoutine myMenu;
	if (!myMenu.isInited()) {
		myMenu.InitMenu(SubMenu_Factory, sizeof(SubMenu_Factory)/sizeof(MenuData)); // 初始化菜单
		myMenu.ui_Title = "Factory Options";
		myMenu.ui_menuColor = 0x000000;
		myMenu.ui_ShowCloseButton = true;
		//myMenu.DebugID = 2;
	}
	
	if (arg1 == 2) {
		//uint8_t b14, b15, a8, a9, a10;
		unsigned state = 0; char ret = 0; bool breakParent = false;
		while (1) {
			ret = myMenu.GetRetValueFromMenu(&state);
			if (0 == (ret & 0x0f)) break;
			if (0 == (ret & 0xf0)) state = 1;
			
			breakParent = false;
			switch (state) {
				case 1: {
					nUIState = 0x31;
					breakParent = true;
					break;
				}
				
				case 2: {
					Lcd_Clear(WHITE);
					Gui_DrawFont_GBK16(10, 10, RED, WHITE, (u8*)"Running");
					
					delay_ms(100);
					
					for (uint16_t i = 0; i < 10; ++i) {
						GPIO_SetBits(GPIOA, GPIO_Pin_11);
						delay_ms(1000);
						GPIO_ResetBits(GPIOA, GPIO_Pin_11);
						delay_ms(1000);
					}
					Gui_DrawFont_GBK16(10, 30, BLUE, WHITE, (u8*)"Finished");
					delay_ms(1000);
					
					myMenu.DrawMenu(true);
					break;
				}
				
				case 4: {
					Lcd_Clear(WHITE);
					Gui_DrawFont_GBK16(10, 10, RED, WHITE, (u8*)"Running");
					
					delay_ms(10);
					
					AT24CXX_WriteLenByte(0x0, 0x00, 8192);
					
					Gui_DrawFont_GBK16(10, 30, BLUE, WHITE, (u8*)"Finished");
					delay_ms(1000);
					
					myMenu.DrawMenu(true);
					break;
				}
				
				default:;
			}
			
			if (breakParent) break;
		}
		
		return;
	}
	
	
	
	if (arg1 == 1) {
		myMenu.DrawMenu(true);
	}
}


MenuData SubMenu_Settings[] = {
	//id, text0, text1, text2
	{ 0x02, "General Settings", 0, 0, },
	{ 0x03, "Time Settings", 0, 0, },
	{ 0x04, "Update Settings", 0, 0, },
	{ 0x05, "Rotate Screen", 0, 0, },
	{ 0x06, "Enter STOP Mode", 0, 0, },
	{ 0x07, "Change Style", "of Clock", 0, },
	{ 0x08, "Hyperdemo", "Optlist", "Manage Center", },
	{ 0x09, "--", 0, 0, },
	{ 0x0a, "Advanced Settings", 0, 0, },
};
void DrawSubMenu_Settings(char arg1) {
	//static unsigned short nMenuState = 0;
	static MenuRoutine myMenu;
	if (!myMenu.isInited()) {
		myMenu.InitMenu(SubMenu_Settings, sizeof(SubMenu_Settings)/sizeof(MenuData)); // 初始化菜单
		myMenu.ui_Title = "Settings";
		myMenu.ui_menuColor = 0xabcdef;
		myMenu.ui_ShowCloseButton = true;
		//myMenu.DebugID = 3;
	}
	
	if (arg1 == 2) {
		//uint8_t b14, b15, a8, a9, a10;
		unsigned state = 0; char ret = 0; bool breakParent = false;
		while (1) {
			ret = myMenu.GetRetValueFromMenu(&state);
			if (0 == (ret & 0x0f)) break;
			if (0 == (ret & 0xf0)) state = 1;
			
			breakParent = false;
			switch (state) {
				case 1: {
					nUIState = 0x10;
					breakParent = true;
					break;
				}
				
				case 3: {
					Lcd_Clear(WHITE);
					
					Gui_DrawFont_GBK16(10, 10, BLACK, WHITE, (u8*)"Set Date Time");
					MyRTC_ReadTime();
					Gui_DrawFont_GBK16(10, 50, BLACK, WHITE, (u8*)"Date:");
					Gui_DrawFont_GBK16(10, 80, BLACK, WHITE, (u8*)"Time:");
					
					char cchBuffer[16] = "", cchBuffer2[10] = ""; signed char currentLine = 0, currentNum = 0;
					
					uint8_t b14, b15, a8, a9, a10;
					while (1) {
						sprintf(cchBuffer , "%04d-%02d-%02d", MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2]); // eg. 2000-01-01
						sprintf(cchBuffer2, "%02d:%02d:%02d", MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]); // eg. 23:59:59
						Gui_DrawFont_GBK16(60, 50, BLACK, WHITE, (u8*)cchBuffer);
						Gui_DrawFont_GBK16(60, 80, BLACK, WHITE, (u8*)cchBuffer2);
						if (currentLine == 0) {
							cchBuffer[4] = cchBuffer[7] = 0;
							switch (currentNum) {
								case 0:
									Gui_DrawFont_ASC16Ex(60, 50, BLACK, WHITE, (u8*)cchBuffer, 1); break;
								case 1:
									Gui_DrawFont_ASC16Ex(60+40, 50, BLACK, WHITE, (u8*)(cchBuffer + 5), 1); break;
								case 2:
									Gui_DrawFont_ASC16Ex(60+64, 50, BLACK, WHITE, (u8*)(cchBuffer + 8), 1); break;
							}
							cchBuffer[4] = cchBuffer[7] = '-';
						}
						if (currentLine == 1) {
							cchBuffer2[2] = cchBuffer2[5] = 0;
							switch (currentNum) {
								case 0:
									Gui_DrawFont_ASC16Ex(60, 80, BLACK, WHITE, (u8*)cchBuffer2, 1); break;
								case 1:
									Gui_DrawFont_ASC16Ex(60+24, 80, BLACK, WHITE, (u8*)(cchBuffer2 + 3), 1); break;
								case 2:
									Gui_DrawFont_ASC16Ex(60+48, 80, BLACK, WHITE, (u8*)(cchBuffer2 + 6), 1); break;
							}
							cchBuffer2[2] = cchBuffer2[5] = ':';
						}
						
						__WFI();
						delay_us(1);
						
						b14 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
						b15 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
						a8 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
						a9 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);
						a10 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);
						
						MenuBase::AntiTik((a8||a9||a10)?GPIOA:GPIOB, a8?GPIO_Pin_8:(
						a9?GPIO_Pin_9:(a10?GPIO_Pin_10:(b14?GPIO_Pin_14:GPIO_Pin_15))));
						
						if (b14) { // <
							if (--currentNum < 0) {
								currentNum = 2;
								currentLine = currentLine == 1 ? 0 : 1;
							}
						}
						if (b15) { // >
							if (++currentNum > 2) {
								currentNum = 0;
								currentLine = currentLine == 1 ? 0 : 1;
							}
						}
						if (a8 || a9) { // +-
							short n = a8 ? 1 : -1;
							MyRTC_Time[currentNum + (currentLine == 1 ? 3 : 0)] += n;
							MyRTC_SetTime();
						}
						if (a10) { // OK
							MenuBase::AntiTik(GPIOA, GPIO_Pin_10);
							break;
						}
						delay_us(10);
					}
					
					myMenu.DrawMenu(true);
					break;
				}
				
				case 5: {
					my_lcd_mode = (my_lcd_mode == 2) ? 1 : 2;
					GPIO_SetBits(GPIOB, GPIO_Pin_12);
					delay_ms(500);
					GPIO_ResetBits(GPIOB, GPIO_Pin_12);
					Lcd_Init(my_lcd_mode);
					myMenu.DrawMenu(true);

					break;
				}
				
				case 6: {
					Lcd_Reset();
					LCD_LED_CLR;
					GPIO_SetBits(GPIOB, GPIO_Pin_12);
					delay_ms(1000);
					PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
					SystemInit();
					Lcd_Init(my_lcd_mode);
					LCD_LED_SET;
					myMenu.DrawMenu(true);
					break;
				}
				
				case 7: {
					nUiTimeShowType = nUiTimeShowType == 1 ? 0 : 1;
					AT24CXX_WriteOneByte(0x11, nUiTimeShowType);
					Gui_DrawFont_GBK16(10, 40, BLACK, WHITE, (u8*)"The operation");
					Gui_DrawFont_GBK16(10, 40+16, BLACK, WHITE, (u8*)"completed");
					Gui_DrawFont_GBK16(10, 40+32, BLACK, WHITE, (u8*)"successfully.");
					delay_s(2);
					myMenu.DrawMenu();
					break;
				}
				
				case 8: {
					Lcd_Clear(BLACK);
					Gui_DrawFont_GBK16(10, 10, WHITE, BLACK, (u8*)"Press a key");
					Gui_DrawFont_GBK16(40, 30, WHITE, BLACK, (u8*)"[011010]");
					uint8_t b14, b15, a8, a9, a10; int state = 0;
					while (1) {
						__WFI();
						delay_us(1);
						Gui_DrawFont_Num32(10, 60, WHITE, BLACK, state);
						Gui_DrawFont_Num32(50, 60, GREEN, BLACK, isSafedEnv ? 1 : 0);
						
						b14 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
						b15 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
						a8 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
						a9 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);
						a10 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);
						
						if (a9) { // -
							MenuBase::AntiTik(GPIOA, GPIO_Pin_9);
							if (state == 0 || state == 1 || state == 3) {
								++state;
							} else break;
						}
						if (a8) { // +
							MenuBase::AntiTik(GPIOA, GPIO_Pin_8);
							if (state == 2) ++state;
							else if (state == 4) {
								isSafedEnv = !isSafedEnv;
								break;
							} else break;
						}
						if (b14 || b15 || a10) {
							break;
						}
					}
					myMenu.DrawMenu(true);
					break;
				}
				
				default:;
			}
			
			if (breakParent) break;
		}
		
		return;
	}
	
	
	
	if (arg1 == 1) {
		myMenu.DrawMenu(true);
	}
}








extern "C" void ExtiScreenLightChecker() {
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_12)) {
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		shouldPauseMainMenu = false;
		nUIState = 0;
	}
}







#if 0
#endif
void RunUIHandler() {
	switch (nUIState) {
		case 0x10: {
			DrawAppMenu(3);
			DrawAppMenu(0);
			
			while (1) {
				DrawAppMenu(2);
				if (nUIState != 0x10) break;
			}
			break;
		}
		
		case 0x31: {
			DrawSubMenu_Application(1);
			while (1) {
				DrawSubMenu_Application(2);
				if (nUIState != 0x31) break;
			}
			break;
		}
		
		case 0x32: {
			/*DrawSubMenu_Mode(1);
			while (1) {
				DrawSubMenu_Mode(2);
				if (nUIState != 0x32) break;
			}*/nUIState = 0x10;
			break;
		}
		
		case 0x33: {
			DrawSubMenu_Settings(1);
			while (1) {
				DrawSubMenu_Settings(2);
				if (nUIState != 0x33) break;
			}
			break;
		}
		
		case 0x101: {
			DrawSubMenu_AlarmManager(1);
			while (1) {
				DrawSubMenu_AlarmManager(2);
				if (nUIState != 0x101) break;
			}
			break;
		}
		case 0x1A1: {
			DrawSubMenu_AlarmUI(1);
			break;
		}
		
		case 0x2001: {
			DrawSubMenu_Factory(1);
			while (1) {
				DrawSubMenu_Factory(2);
				if (nUIState != 0x2001) break;
			}
			break;
		}
			
		default:;
	}
}






