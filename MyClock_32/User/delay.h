#ifndef __DELAY_H
#define __DELAY_H
#include "stm32f10x.h"                  // Device header

#if 0
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);

void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay_s(uint32_t s);
#else
void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 nus);
#define delay_s(s) delay_ms((s) * 1000)
#define Delay_us delay_us
#define Delay_ms delay_ms
#define Delay_s delay_s
#endif

#endif
