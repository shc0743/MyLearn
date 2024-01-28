#include "MyAlarm.h"
extern "C" {
#include "delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "MyRTC.h"
#include "AT24Cxx.h"
#include "stdlib.h"
}
#include "app.h"
#include "MyDateTime.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>


extern unsigned int nUIState;
extern "C" { extern unsigned char DebugMode; }

MyAlarmItem myAlarms[20];
#define VALIDALARM 0x7f10c483


void MyAlarm_CheckAlarm();



MenuData SubMenu_AlarmManager[21] = {
	//id, text0, text1, text2
	{ 0x02, "// ", 0, 0, },
};
static void UpdateAlarmMenu() {
	for (unsigned int i = 0; i < 20; ++i) {
		MenuData& item = SubMenu_AlarmManager[i + 1];
		item.id = i + 3;
		sprintf(item.szText0, "Alarm %d.", i);
		if (myAlarms[i].valid != VALIDALARM) continue;
		sprintf(item.szText1, "[%s]", myAlarms[i].enabled ? "ENABLED" : "DISABLED");
	}
}



static void EditAlarmUI(signed short n = -1) {
	if (n != -1 && (n < 0 || n > 20)) return;
	
	Lcd_Clear(WHITE);
	Gui_DrawFont_GBK16(10, 8, BLACK, WHITE, (u8*)"Edit Alarm");
	Gui_DrawLine(0, 28, 160, 28, BLACK); // 分割线

	MyAlarmItem ald;
	if (n > -1) ald = myAlarms[n];
	
	signed short citem = 0, x = 1; char cchBuffer2[10] = ""; bool shouldBreak = false, changed = false;
	if (!ald.valid) {
		ald.time[0] = ald.time[1] = ald.time[2] = 0;
		ald.enabled = false;
		ald.days = 0x0fffffff;
		ald.ringLater = 0;
		ald.valid = VALIDALARM;
		changed = true;
	}
	sprintf(cchBuffer2, "%d", (int)n);
	Gui_DrawFont_GBK16(110, 8, BLACK, WHITE, (u8*)cchBuffer2);
	while (1) {
		if (shouldBreak) break;
		
		Gui_DrawFont_ASC16Ex(10, 40, BLACK, WHITE, (u8*)"Enabled:", 0);
		Gui_DrawFont_ASC16Ex(80, 40, BLACK, WHITE, (u8*)(ald.enabled ? "on " : "off"), citem == 0);
		
		Gui_DrawFont_ASC16Ex(10, 60, BLACK, WHITE, (u8*)"Time:", 0);
		
		sprintf(cchBuffer2, "%02d:%02d:%02d", ald.time[0], ald.time[1], ald.time[2]); // eg. 23:59:59
		Gui_DrawFont_ASC16Ex(60, 60, BLACK, WHITE, (u8*)cchBuffer2, 0);
		if (citem >= 1 && citem <= 3) {
			cchBuffer2[2] = cchBuffer2[5] = 0;
			switch (citem) {
				case 1:
					Gui_DrawFont_ASC16Ex(60, 60, BLACK, WHITE, (u8*)cchBuffer2, 1); break;
				case 2:
					Gui_DrawFont_ASC16Ex(60+24, 60, BLACK, WHITE, (u8*)(cchBuffer2 + 3), 1); break;
				case 3:
					Gui_DrawFont_ASC16Ex(60+48, 60, BLACK, WHITE, (u8*)(cchBuffer2 + 6), 1); break;
			}
			cchBuffer2[2] = cchBuffer2[5] = ':';
		}
		
		uint8_t b14, b15, a8, a9, a10;
		while (1) {
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
				if (--citem < 0) citem = 3; break;
			}
			if (b15) { // >
				if (++citem > 3) citem = 0; break;
			}
			if (a8 || a9) { // +-
				changed = true;
				if (citem == 0) { ald.enabled = !ald.enabled; break; }
				x = a8 ? 1 : -1;
				ald.time[citem - 1] += x;
				if (ald.time[citem - 1] > ((citem == 1) ? 23 : 59)) ald.time[citem - 1] = 0;
				else if (ald.time[citem - 1] < 0) ald.time[citem - 1] = (citem == 1) ? 23 : 59;
				break;
			}
			if (a10) { // OK
				MenuBase::AntiTik(GPIOA, GPIO_Pin_10);
				if (changed) {
					Lcd_Clear(BLACK);
					Gui_DrawFont_GBK16(10, 10, GREEN, BLACK, (u8*)"Saving changes");
					delay_ms(100);
					
					myAlarms[n] = ald;
					AT24CXX_Write(0x100, (u8*)myAlarms, sizeof(myAlarms));
					UpdateAlarmMenu();
				}
				delay_ms(50);
				Lcd_Clear(GRAY0);
				shouldBreak = true;
				break;
			}
			delay_us(10);
		}
	}
}



void DrawSubMenu_AlarmManager(char arg1) {
	//static unsigned short nMenuState = 0;
	static MenuRoutine myMenu;
	if (!myMenu.isInited()) {
		myMenu.ui_Title = "Alarm Manager";
		myMenu.ui_menuColor = 0x000000;
		myMenu.ui_ShowCloseButton = true;
	}
	MyAlarm_CheckAlarm();
	myMenu.InitMenu(SubMenu_AlarmManager, sizeof(SubMenu_AlarmManager)/sizeof(MenuData)); // 初始化菜单
	
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
				
				/*case 2: {
					EditAlarmUI();
					myMenu.DrawMenu(true);
					break;
				}*/
				case 2: break;
				
				default: {
					EditAlarmUI(state - 3);
					myMenu.InitMenu(SubMenu_AlarmManager, sizeof(SubMenu_AlarmManager)/sizeof(MenuData)); // 初始化菜单
					myMenu.DrawMenu(true);
					break;
				}
			}
			
			if (breakParent) break;
		}
		
		return;
	}
	
	
	
	if (arg1 == 1) {
		myMenu.DrawMenu(true);
	}
}



void DrawSubMenu_AlarmUI(char arg1) {
	Lcd_Clear(WHITE);
	Gui_DrawFont_GBK16(10, 10, RED, WHITE, (u8*)"ALARM");
	WaitKey(GPIOA, GPIO_Pin_10);
	nUIState = 0;
}


void MyAlarm_CheckAlarm() {
	unsigned int firstAlarm = 0xffffffff;
	RTC_SetAlarm(0);//清除下次闹钟
	
    RTC_WaitForLastTask();
	
	time_t time_cnt = 0, time_cnt2 = RTC_GetCounter();
	struct tm time_date;
	
	for (unsigned int i = 0; i < 20; ++i) {
		//MenuData& item = SubMenu_AlarmManager[i + 1];
		//item.id = i + 3;
		//sprintf(item.szText0, "Alarm %d.", i);
		
		if (myAlarms[i].valid != VALIDALARM) continue;
		if (!myAlarms[i].enabled) continue;
		time_date.tm_year = MyRTC_Time[0];
		time_date.tm_mon = MyRTC_Time[1] - 1;
		time_date.tm_mday = MyRTC_Time[2];
		time_date.tm_hour = myAlarms[i].time[0];
		time_date.tm_min = myAlarms[i].time[1];
		time_date.tm_sec = myAlarms[i].time[2];
		time_cnt = mktime(&time_date);
		if (time_cnt < time_cnt2) time_cnt += 86400;
		if (time_cnt < firstAlarm) firstAlarm = time_cnt - 3;
		if (1) {
			MenuData& item = SubMenu_AlarmManager[i + 1];
			sprintf(item.szText2, "%d %d-%d:%d", time_date.tm_hour, time_date.tm_min, time_date.tm_sec, firstAlarm);
		}
		//sprintf(item.szText1, "[%s]", myAlarms[i].enabled ? "ENABLED" : "DISABLED");
	}
	UpdateAlarmMenu();
	
	
	
	
	if (firstAlarm && firstAlarm != 0xffffffff) {
		RTC_SetAlarm(firstAlarm);//配置下次闹钟
	}
	
	
}



void MyAlarm_Init() {
	AT24CXX_Read(0x100, (u8*)myAlarms, sizeof(myAlarms));
	for (unsigned int i = 0; i < 20; ++i) {
		if (myAlarms[i].valid != VALIDALARM) {
			myAlarms[i].valid =
			myAlarms[i].days =
			myAlarms[i].enabled =
			myAlarms[i].ringLater =
			myAlarms[i].time[0] = myAlarms[i].time[1] = myAlarms[i].time[2] =
			0;
		}
	}
	
    RTC_WaitForSynchro();
    RTC_WaitForLastTask();
    RTC_ITConfig(RTC_IT_ALR, ENABLE);//使能秒钟断和闹钟中断
	RTC_WaitForLastTask();

	NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;//RTC全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn; //闹钟中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =  1; //比RTC全局中断的优先级高
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

	
	
	MyAlarm_CheckAlarm();
}


extern "C" void RTC_IRQHandler(void) {
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) { //秒钟中断
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);
    }
}




static void HandleAlarmEvent() {
	nUIState = 0x1A1;
}




extern "C" void RTCAlarm_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_ALR) != RESET) //ALR中断
    {
        EXTI_ClearITPendingBit(EXTI_Line17);
        RTC_WaitForLastTask();
		
		HandleAlarmEvent();
		
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }

    RTC_ClearITPendingBit(RTC_IT_OW);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();

}



