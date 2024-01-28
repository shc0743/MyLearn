#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "Lcd_Driver.h"
#include "GUI.h"
#include "app.h"


extern unsigned char DebugMode;

void ExtiScreenLightChecker(void);


void EXTI15_10_IRQHandler(void)
{
	//Gui_DrawFont_GBK16(10,30,GREEN,GRAY0, (u8*)"IRQ Entered");
	
	if (EXTI_GetITStatus(EXTI_Line10) == SET) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == 0) {
			if (DebugMode) Gui_DrawFont_GBK16(20, 0, BLACK, YELLOW, (u8*)"EXTI10");
			DrawAppMenu(1);
			ExtiScreenLightChecker();
		}
		EXTI_ClearITPendingBit(EXTI_Line10);
		//while (EXTI_GetITStatus(EXTI_Line10) == SET);
	}

	if (EXTI_GetITStatus(EXTI_Line14) == SET) {
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0) {
			
			ExtiScreenLightChecker();
		}
		EXTI_ClearITPendingBit(EXTI_Line14);
		//while (EXTI_GetITStatus(EXTI_Line14) == SET);
	}
	
	if (EXTI_GetITStatus(EXTI_Line15) == SET) {
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0) {
			
			ExtiScreenLightChecker();
			
		}
		EXTI_ClearITPendingBit(EXTI_Line15);
		//while (EXTI_GetITStatus(EXTI_Line15) == SET);
	}
	
	//Gui_DrawFont_GBK16(10,50,GREEN,GRAY0, (u8*)"IRQ Exited");
}
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line8) == SET) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0) {
			
			
			ExtiScreenLightChecker();
		}
		EXTI_ClearITPendingBit(EXTI_Line8);
		//while (EXTI_GetITStatus(EXTI_Line8) == SET);
	}
	if (EXTI_GetITStatus(EXTI_Line9) == SET) {
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_9) == 0) {
			
			
			ExtiScreenLightChecker();
		}
		EXTI_ClearITPendingBit(EXTI_Line9);
		//while (EXTI_GetITStatus(EXTI_Line9) == SET);
	}
}


