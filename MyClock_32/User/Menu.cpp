extern "C" {
#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
}
#include "Menu.h"
#include "app.h"
#include "mygpio.h"



extern "C" {
	extern unsigned char DebugMode;
}



void MenuBase::AntiTik(GPIO_TypeDef* gpio, uint16_t pin) {
	delay_ms(100);
	while (GPIO_ReadInputDataBit(gpio, pin) == 0);
	delay_ms(10);
}




bool MenuRoutine::InitMenu(MenuData* menuData, unsigned int itemCount) {
	this->menuItemCount = itemCount;
	return (this->m_isInited = bool(this->menuData = menuData));
}


void MenuRoutine::DrawMenu(bool repaint) {
	if (repaint) {
		// 重绘菜单UI
		
		Lcd_Clear(GRAY0); // 清屏
		
		Gui_DrawFont_GBK16(10, 8, BLACK, GRAY0, (u8*)this->ui_Title); // 显示标题（标题不宜过长）
		Gui_DrawLine(0, 28, 160, 28, BLACK); // 分割线
		
		if (this->ui_ShowCloseButton) { // 关闭按钮
			//DisplayButtonUp(135, 5, 155, 25);
			Gui_DrawFont_GBK16(141, 6, RED, GRAY0, (u8*)"x");
		}
		
		Gui_DrawFont_ASC16Ex(0, 128-32, WHITE, BLACK, (u8*)"+- Switch Item        ", false); // 提示
		Gui_DrawFont_ASC16Ex(0, 128-16, WHITE, BLACK, (u8*)"[OK] Enter Item       ", false); // 提示
		
		nMenuState = 2; // 默认聚焦第一个菜单项
		
		
	}
	
//	if (DebugMode) Gui_DrawFont_Num32(20, 20, WHITE, GREEN, DebugID);
	
	if (this->ui_ShowCloseButton) DisplayButtonWithState(135, 5, 155, 25, nMenuState == 1); // 关闭按钮的边框
	else if (this->nMenuState == 1) this->nMenuState = 2;
	
	Gui_Fill(0, 29, 160, 67, GRAY1); // 菜单显示区域
	const u16 bgColor = RGB6toRGB565( // 菜单项背景颜色
		(ui_menuColor & 0xff0000) >> 16, // R
		(ui_menuColor & 0x00ff00) >> 8,  // G
		(ui_menuColor & 0x0000ff) >> 0); // B

	for (unsigned long i = 1; i < this->menuItemCount + 1; ++i) {
		const volatile MenuData& ref = this->menuData[i - 1];
		if (ref.id != nMenuState) continue;
		if (ref.szText0[0]) Gui_DrawFont_ASC16Ex(10, 40, bgColor, WHITE, (u8*)ref.szText0, true);
		if (ref.szText1[0]) Gui_DrawFont_ASC16Ex(10, 40+16, bgColor, WHITE, (u8*)ref.szText1, true);
		if (ref.szText2[0]) Gui_DrawFont_ASC16Ex(10, 40+32, bgColor, WHITE, (u8*)ref.szText2, true);
	}
	
	
	
	
}


char MenuRoutine::GetRetValueFromMenu(unsigned int * value) {
	uint8_t b14, b15, a8, a9, a10;
	while (1) {
		__WFI();
		delay_us(1);
		
		b14 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14);
		b15 = !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
		a8 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
		a9 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9);
		a10 = !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);
		
		if (a9) { // -
			if (--nMenuState < 1) nMenuState = this->menuItemCount + 1;
			this->DrawMenu(); AntiTik(GPIOA, GPIO_Pin_9);
			//break;
		}
		if (a8) { // +
			if (++nMenuState > this->menuItemCount + 1) nMenuState = 1;
			this->DrawMenu(); AntiTik(GPIOA, GPIO_Pin_8);
			//break;
		}
		if (b14) { // <
			if (--nMenuState < 1) nMenuState = this->menuItemCount + 1;
			this->DrawMenu(); AntiTik(GPIOB, GPIO_Pin_14);
			//break;
		}
		if (b15) { // >
			if (++nMenuState > this->menuItemCount + 1) nMenuState = 1;
			this->DrawMenu(); AntiTik(GPIOB, GPIO_Pin_15);
			//break;
		}
		if (a10) { // OK
			AntiTik(GPIOA, GPIO_Pin_10);
			
			if (nMenuState == 1) return 0x0f;
			*value = nMenuState;
			return 0xff;
			
			//break;
		}
	}
	
	return 0x00;
}





