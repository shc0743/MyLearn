#include "apps_ext.h"
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
#include "Menu.h"


#include <stdlib.h>
#include <stdio.h>



extern unsigned int nUIState;
extern int my_ext_gpsTimeUpdateService;



void app_Timer() {
	Lcd_Clear(BLACK);
	
	
	unsigned long time = 0, crtcTime = 0, splitdTime = 0;
	char timestr[] = "0000";
	Gui_DrawFont_GBK16(75, 45, WHITE, BLACK, (u8*)":");
	bool shouldStart = false, shouldUpdate = true;
	
	while (1) {
		//Gui_DrawFont_Num32(10+5, 40, GREEN, BLACK, time);
		if (time % 20 == 0 || shouldUpdate) {
			unsigned long t1 = 0, t2 = 0;
			t1 = splitdTime / 60;
			t2 = splitdTime % 60;
			if (t1 == 99 && t2 == 59) shouldStart = false;
			sprintf(timestr, "%02ld%02ld", t1, t2);
			
			Gui_DrawFont_Num32(10+5, 40, shouldStart ? WHITE : GREEN, BLACK, u16(timestr[0] - 48));
			Gui_DrawFont_Num32(42+5, 40, shouldStart ? WHITE : GREEN, BLACK, u16(timestr[1] - 48));
			Gui_DrawFont_Num32(80+5, 40, shouldStart ? WHITE : GREEN, BLACK, u16(timestr[2] - 48));
			Gui_DrawFont_Num32(112+5, 40, shouldStart ? WHITE : GREEN, BLACK, u16(timestr[3] - 48));
			if (shouldUpdate) shouldUpdate = false;
		}
		
		if (!shouldStart) __wfi();
		if ((!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10))) { // OK
			break;
		}
		
		if ((!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8))) { // +
			shouldStart = !shouldStart;
			MenuBase::AntiTik(GPIOA, GPIO_Pin_8);
		}
		
		delay_ms(25);
		if (shouldStart) {
			crtcTime = RTC_GetCounter();
			if (crtcTime != time) {
				++splitdTime;
				time = crtcTime;
				shouldUpdate = true;
			}
		}
	}
	
	
	
}


void app_TimeCountDown() {
	
}


static unsigned int nTUSExecCount = 0, nTUSUpdCount = 0;


/*MenuData SubMenu_Mode[] = {
	//id, text0, text1, text2
	{ 0x02, "Sync Time", 0, 0, },
};*/
void DrawSubMenu_Mode(char arg1) {
	#if 0
	//static unsigned short nMenuState = 0;
	static MenuRoutine myMenu;
	if (!myMenu.isInited()) {
		myMenu.ui_Title = "Mode";
		myMenu.ui_menuColor = 0x000000;
		myMenu.ui_ShowCloseButton = true;
	}
	myMenu.InitMenu(SubMenu_Mode, sizeof(SubMenu_Mode)/sizeof(MenuData));
	
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
				
				case 2: {
				#endif
					return;Lcd_Clear(BLACK);
					Gui_DrawFont_GBK16(10, 10, WHITE, BLACK, (u8*)"Please wait,");
					Gui_DrawFont_GBK16(10, 26, WHITE, BLACK, (u8*)"task is running...");
					delay_ms(300);
						
					GPIO_SetBits(GPIOB, GPIO_Pin_13); // ??
					nTUSExecCount = nTUSUpdCount = 0;
					my_ext_gpsTimeUpdateService = 1;
					
					//WaitKey(GPIOA, GPIO_Pin_10);
					
					Lcd_Clear(BLACK);
					Gui_DrawFont_GBK16(10, 10, GREEN, BLACK, (u8*)"Request sent");
					Gui_DrawFont_GBK16(10, 26, GREEN, BLACK, (u8*)"successfully");
					delay_ms(500);
					
					nUIState = 0x10;
					//breakParent = true;
					#if 0
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
	#endif
}



#if 0
static bool myStrStartsWith(const char* str, const char* starter) {
	unsigned int n = 0;
	do {
		if (starter[n] != str[n]) return false;
	} while (starter[++n] != 0);
	return true;
}
#endif


#include <time.h>
extern short my_TimeZone;
bool GTUS_internal0(const char** cch, char* mystr, unsigned char* mystr_ptr, unsigned char* mystate, int* data) {
	if ((*cch)[*mystr_ptr] == ',' || (*cch)[*mystr_ptr] == '*') {
		*data = atoi(mystr);
		*cch = *cch + (1+(*mystr_ptr));
		mystr[0] = mystr[1] = mystr[2] = mystr[3] = mystr[4] = 0;
		mystr_ptr = 0;
		*mystate = *mystate + 1;
		return true;
	}
	mystr[*mystr_ptr] = (*cch)[*mystr_ptr];
	++mystr_ptr;
	return false;
}
void GpsTimeUpdateService() {
	time_t time_cnt = 0;
	struct tm time_date;
	
	//unsigned char mystate = 0; char mystr[6] = ""; unsigned char mystr_ptr = 0;
	Gui_DrawFont_GBK16(95, 0, GREEN, GRAY0, (u8*)"Updating");
	if ((++nTUSExecCount) % 50 == 0) {
		//Serial_SendString("$PGKC279*23\r\n");
		Gui_DrawFont_GBK16(95, 0, GREEN, GRAY0, (u8*)"Loading ");
	}
	
	if (Serial_RxFlag) {
		//Serial_RxPacket[11] = 0;
		Gui_DrawFont_GBK16(0, 0, BLACK, YELLOW, (u8*)Serial_RxPacket);
		const char* cch = Serial_RxPacket;
		//if (myStrStartsWith(cch, "PGKC280,")) {
		if (cch[2] == 'G' && cch[3] == 'G' && cch[4] == 'A' && 1) {
			#if 0
			cch += 8;
			for (unsigned int __ANTILOOP__ = 0; __ANTILOOP__ < 0xffff; ++__ANTILOOP__) {
				if (mystate == 0) {
					#if 0
					if (cch[0+mystr_ptr] == ',') {
						time_date.tm_year = atoi(mystr);
						cch += (1+mystr_ptr);
						mystr[0] = mystr[1] = mystr[2] = mystr[3] = mystr[4] = 0;
						mystr_ptr = 0;
						mystate = 1;
						continue;
					}
					mystr[mystr_ptr] = cch[0+mystr_ptr];
					++mystr_ptr;
					#endif
					if (GTUS_internal0(&cch, mystr, &mystr_ptr, &mystate, &time_date.tm_year)) continue;
				}
				if (mystate == 1) {
					if (GTUS_internal0(&cch, mystr, &mystr_ptr, &mystate, &time_date.tm_mon)) continue;
				}
				if (mystate == 2) {
					if (GTUS_internal0(&cch, mystr, &mystr_ptr, &mystate, &time_date.tm_mday)) continue;
				}
				if (mystate == 3) {
					if (GTUS_internal0(&cch, mystr, &mystr_ptr, &mystate, &time_date.tm_hour)) continue;
				}
				if (mystate == 4) {
					if (GTUS_internal0(&cch, mystr, &mystr_ptr, &mystate, &time_date.tm_min)) continue;
				}
				if (mystate == 5) {
					if (GTUS_internal0(&cch, mystr, &mystr_ptr, &mystate, &time_date.tm_sec)) continue;
				}
				if (mystate == 6) break;
			}
			#else
			char hh[3] = { cch[6], cch[7], 0}, mm[3] = { cch[8], cch[9], 0 }, ss[3] = {cch[10],cch[11],0};
			MyRTC_ReadTime();
			time_date.tm_year = MyRTC_Time[0], time_date.tm_mon = MyRTC_Time[1], time_date.tm_mday = MyRTC_Time[2];
			time_date.tm_hour = atoi(hh), time_date.tm_min = atoi(mm), time_date.tm_sec = atoi(ss);
			#endif
			
			
			time_cnt = mktime(&time_date);
			time_cnt += (my_TimeZone) * (60 * 60);
			
			struct tm time_date2;
			time_date2 = *localtime(&time_cnt);
			MyRTC_Time[0] = time_date2.tm_year, 
			MyRTC_Time[1] = time_date2.tm_mon, 
			MyRTC_Time[2] = time_date2.tm_mday, 
			MyRTC_Time[3] = time_date2.tm_hour, 
			MyRTC_Time[4] = time_date2.tm_min, 
			MyRTC_Time[5] = time_date2.tm_sec;
			MyRTC_SetTime();
					
			Gui_DrawFont_GBK16(0, 0, GREEN, GRAY0, (u8*)"Done");
			if (++nTUSUpdCount > 100) {
				my_ext_gpsTimeUpdateService = nTUSUpdCount = 0;
				GPIO_ResetBits(GPIOB, GPIO_Pin_13); // ??
				Lcd_Clear(GRAY0);
			}
		}
		
		Serial_RxFlag = 0;
	}
}




