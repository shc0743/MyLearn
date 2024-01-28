#ifndef __HEADER_FILE_Menu_hpp__
#define __HEADER_FILE_Menu_hpp__

#include "stm32f10x.h"                  // Device header



typedef struct {
	unsigned int id;
	char szText0[20];
	char szText1[20];
	char szText2[20];
} MenuData;


class MenuBase {
	public:
		MenuBase() {};
			
	public:
		static void AntiTik(GPIO_TypeDef* gpio, uint16_t pin); // 防抖
	
	protected:
		unsigned int menuItemCount;
		volatile MenuData* menuData;
	
		unsigned int nMenuState;
};


class MenuRoutine : public MenuBase {
	public:
		MenuRoutine() {
			this->menuData = 0;
			this->menuItemCount =
			this->m_isInited =
			this->nCachedMenuItem = 
			this->nMenuState = 
			this->ui_menuColor = 0;
			this->ui_ShowCloseButton = true;
		};
	
	public:
		bool InitMenu(MenuData* menuData, unsigned int itemCount);
		bool isInited() { return this->m_isInited; };
		void DrawMenu(bool repaint = false);
	
		/*
		@return a 8-bit data.
			If the operation is completed (not cancelled by user), high bit is set. otherwise user cancelled the request.
			If an error has occurred, low bit is reset. otherwise low bit is set.
		*/
		char GetRetValueFromMenu(unsigned int * value);
	
		virtual int user(int argument) { return 0; };
		
	public:
		const char* ui_Title;
		bool ui_ShowCloseButton;
		int ui_menuColor;
	
		//int DebugID;
	
	protected:
		bool m_isInited;
	
	private:
		unsigned nCachedMenuItem;
};












#endif
