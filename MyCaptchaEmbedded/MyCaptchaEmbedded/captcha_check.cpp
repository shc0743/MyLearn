#include "global.h"
#include "captchaAPI.h"
using namespace CaptchaEmbedded;



bool __stdcall CheckCaptcha(CAPTCHAHANDLE captcha) {
	PCAPTCHADATA data = captcha;
	if (data->data.pValidateFunc) return data->data.pValidateFunc(data);

	return false;
}


void WINAPI CheckCaptcha_RunDLL() {

	ExitProcess(1);



}








