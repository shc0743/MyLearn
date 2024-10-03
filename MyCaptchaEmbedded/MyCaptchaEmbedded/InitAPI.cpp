#include <Windows.h>
#include "global.h"
#include "InitAPI.h"
#include "captcha_ui.h"




bool __stdcall Initialize(void* data) {

	// UI
	UI_Init();


	return true;
}

bool __stdcall Initialize_Thread(void* data) {

	return true;
}

bool __stdcall UnInitialize_Thread(void* data) {

	return true;
}

bool __stdcall UnInitialize(void* data) {

	// UI
	UI_UnInit();


	return true;
}




