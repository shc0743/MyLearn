extern "C" {
#include "stm32f10x.h"                  // Device header
}

struct MyDateTime {
	uint8_t sec,min,hour,day,mon,week;
	unsigned int year;
};

