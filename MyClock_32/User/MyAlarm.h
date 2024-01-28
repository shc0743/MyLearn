#include "stm32f10x.h"                  // Device header

#include "Menu.h"



typedef struct {
	int valid;
	signed char time[3];
	bool enabled;
	int days; // 0x0fffffff
	          //    1234567
	unsigned int ringLater;
} MyAlarmItem;

extern MyAlarmItem myAlarms[20];



void MyAlarm_Init();
void DrawSubMenu_AlarmManager(char arg1);
void DrawSubMenu_AlarmUI(char arg1);





