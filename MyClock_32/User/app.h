
#ifndef __HEADERFILE_app_H__
#define __HEADERFILE_app_H__


#ifdef __cplusplus
void RunApplication();
void ShowAppMenu();



#ifdef __STM32F10x_H
void WaitKey(GPIO_TypeDef * gpio, uint16_t pin, bool useSTOPMode = false);
#endif



extern "C" {
#endif


void DrawAppMenu(char arg1);


	
#ifdef __cplusplus
}
#endif


#define LightLED13(time) GPIO_ResetBits(GPIOC, GPIO_Pin_13); \
delay_ms(time);GPIO_SetBits(GPIOC, GPIO_Pin_13);




#endif

