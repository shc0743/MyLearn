#include "global.h"
#include "captchaAPI.h"
#include "../../resource/tool.h"
#include "captcha_errors.h"

using namespace CaptchaEmbedded;





HWND __stdcall CreateCaptcha(CAPTCHAWINDOWDATA* wd, CAPTCHADATA* data) {
	wd->__ownerData__ = data;
	HWND hwnd = CreateWindowExW(0, GetCaptchaWindowClassName(),
		wd->caption, WS_POPUPWINDOW | WS_MINIMIZEBOX
		| (wd->resizable ? WS_SIZEBOX : 0),
		0, 0, 1, 1, 0, 0, hInst, wd);
	if (!hwnd) return NULL;

	wd->_hwnd = hwnd;

	SetWindowPos(hwnd, 0, wd->pos.x, wd->pos.y, wd->size.cx, wd->size.cy, 0);
	if (wd->centerWindow) {
		CenterWindow(hwnd);
	}

	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);


	return hwnd;
}


bool __stdcall DestroyCaptcha(CAPTCHAWINDOWDATA* wd) {
	SendMessage(wd->_hwnd, WM_USER + WM_DESTROY, WM_DESTROY, 0);
	return DestroyWindow(wd->_hwnd);
}



bool __stdcall CheckCaptcha(CAPTCHAHANDLE captcha);
int __stdcall RunCaptcha(CAPTCHADATA* data) {
	if (!data) return EIPARAM;

	ShowWindow(data->wData->_hwnd, SW_NORMAL);



	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	Sleep(256);

	if (msg.wParam) return (int)(CheckCaptcha(data) ? msg.wParam : -1);
	return CheckCaptcha(data) ? 0 : 1;
}





void WINAPI RunCaptcha_RunDLL() {
	CmdLineW cl(::GetCommandLineW());


	CAPTCHAWINDOWDATA wd{};
	CAPTCHADATA d{};
	d.cb = sizeof(d);
	d.wData = &wd;
	d.data_type = CAPTCHADATA::T_data_type::OwnerDraw;
	d.data.pGenerateFunction = CaptchaUtil_generateMathProblem;
	d.data.pDrawFunction = CaptchaUtil_drawMathProblem;
	d.data.pValidateFunc = CaptchaUtil_validateMathProblem;

	wd.cb = sizeof wd;
	std::wstring wCaption;
	cl.getopt(L"title", wCaption);
	if (wCaption.empty()) wCaption = L"CAPTCHA";
	wcscpy_s(wd.caption, wCaption.c_str());
	wd.size.cx = 500; wd.size.cy = 300;
	wd.centerWindow = true;
	wd.resizable = false;
	wd.ctls_data.text0 = L"To prove you are a human, please solve the following challenge.";
	wd.ctls_data.text_question = L"Please type the answer above.";
	HWND hfFg = GetForegroundWindow();
	if (!CreateCaptcha(&wd, &d)) {
		MessageBoxW(0, LastErrorStrW().c_str(), L"Failed to load CAPTCHA", MB_ICONHAND);
		exit(-1);
	}
	
	if (cl.getopt(L"center-foreground-window")) {
		CenterWindow(wd._hwnd, hfFg);
	}


	exit(RunCaptcha(&d));




}






void __stdcall CaptchaUtil_generateMathProblem(CaptchaEmbedded::PCAPTCHADATA data) {
	std::wstring result;
	const static PCWSTR opers[]{
		L" + ", L" - ", L" * "
	};
	const static PCWSTR suffix{ L" = ?" };

	srand((unsigned int) (time(0) - rand()));
	short bit1 = rand() % 100;
	short bit2 = rand() % 100;

	srand((unsigned int) (time(0) - rand()));
	short oper = rand() % 3;
	if (data->difficulty != CAPTCHADATA::T_difficulty::Normal) {
		if (data->difficulty == CAPTCHADATA::T_difficulty::Easy) oper = oper % 2;
		if (data->difficulty == CAPTCHADATA::T_difficulty::Difficult) oper = 2;
	}


	result += std::to_wstring(bit1);
	result += opers[oper];
	result += std::to_wstring(bit2);
	result += suffix;


	size_t len = result.length();
	memcpy(data->__custom_data_for_CAPTCHA_provider__, &len, sizeof(len));
	memcpy(data->__custom_data_for_CAPTCHA_provider__ + sizeof(len), result.c_str(), len * sizeof(wchar_t));


	long long answer = -1;
	switch (opers[oper][1])
	{
	case '+':
		answer = (long long)bit1 + (long long)bit2; break;
	case '-':
		answer = (long long)bit1 - (long long)bit2; break;
	case '*':
		answer = (long long)bit1 * (long long)bit2; break;
#if 0
		/*
		* This will cause a crash if bit2==0.
		* So the '/' is deprecated and not be used now.
		* Also for the following reasons:
		* 
		* - I am lazy
		* - Division is too difficult for users
		*
		*/
	case '/':
		if (bit2 == 0) {  } // 
		answer = (unsigned long long)bit1 / (unsigned long long)bit2; break;
#endif
	default:; // exception
	}

	memcpy(data->__custom_data_for_CAPTCHA_provider__ + sizeof(len) + (len * sizeof(wchar_t)), &answer, sizeof(answer));



}

void __stdcall CaptchaUtil_drawMathProblem(HDC hdc, PCAPTCHADATA data) {
	RECT rc{ .left = 20,.top = 20,.right = 120,.bottom = 60 };
	Rectangle(hdc, 10, 10, 150, 80);

	DrawText(hdc, L"Please solve:", -1, &rc, DT_INTERNAL | DT_NOCLIP);

	rc.top = 40;
	size_t len = 0;
	memcpy(&len, data->__custom_data_for_CAPTCHA_provider__, sizeof(len));

	DrawText(hdc, PCWSTR(data->__custom_data_for_CAPTCHA_provider__ + sizeof(len)), (int)len, &rc, DT_INTERNAL | DT_NOCLIP);


}

bool __stdcall CaptchaUtil_validateMathProblem(PCAPTCHADATA data) {
	long long answer = -2;
	size_t len = 0;
	memcpy(&len, data->__custom_data_for_CAPTCHA_provider__, sizeof(len));
	memcpy(&answer, data->__custom_data_for_CAPTCHA_provider__ + sizeof(len) + (len * sizeof(wchar_t)), sizeof(answer));

	std::wstring wAns = std::to_wstring(answer);
	if (data->wData->user_answer == wAns) return true;

	return false;
}








