//#pragma once
#ifndef ab92e2d5fd584de899060fe3ecefbbf3
#define ab92e2d5fd584de899060fe3ecefbbf3


#define MyGPIO_Init(N, Port, Mode) \
GPIO_InitTypeDef  GPIO_InitStructure_##N##Port; \
GPIO_InitStructure_##N##Port.GPIO_Pin = GPIO_Pin_##Port; \
GPIO_InitStructure_##N##Port.GPIO_Speed = GPIO_Speed_50MHz; \
GPIO_InitStructure_##N##Port.GPIO_Mode = Mode; \
GPIO_Init(GPIO##N, &GPIO_InitStructure_##N##Port); \
GPIO_SetBits(GPIO##N, GPIO_Pin_##Port);


#endif

