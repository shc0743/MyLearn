#include "global.h"
#include "captcha_ui.h"
#include "captchaAPI.h"
#include "captcha_errors.h"
#include "resource.h"
#include "../../resource/tool.h"


using namespace CaptchaEmbedded;


static WCHAR szCaptchaClass[256];
static WCHAR szEmbeddedClass[256];
static HBRUSH hBrush_F0F0F0, hBrush_FFFFFF;
static HFONT hFontNormal0, hFontB0, hFontI0, hFontU0, hFontS0, hFontLarge1, hFontLarge2;
static LRESULT CALLBACK WndProc_Captcha(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WndProc_Embedded(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



namespace CaptchaEmbedded {
	PCWSTR GetCaptchaWindowClassName() {
		return szCaptchaClass;
	}


	struct _internal_CAPTCHAWINDOWDATA {
		CAPTCHAWINDOWDATA* userData;

		HWND
			hS0,
			hEmbeddedView,
			hQuestion,
			hInputBox,
			hRefreshLink,
			hSubmitButton,
			hCancelButton,
			hEnd;


	};

}


bool UI_Init() {
	
	ATOM uatom = NULL;

	hBrush_F0F0F0 = CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));
	hBrush_FFFFFF = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	
	if (LoadStringW(hInst, IDS_STRING_UI_CAPTCHAWNDCLASS, szCaptchaClass, 256))
		uatom = s7::MyRegisterClassW(szCaptchaClass, WndProc_Captcha, WNDCLASSEXW{
		.hInstance = hInst,
		.hbrBackground = hBrush_FFFFFF,
	});

	if (LoadStringW(hInst, IDS_STRING_UI_CEMBEDWNDCLASS, szEmbeddedClass, 256))
		uatom = s7::MyRegisterClassW(szEmbeddedClass, WndProc_Embedded, WNDCLASSEXW{
		.hInstance = hInst,
		.hCursor = LoadCursor(NULL, IDC_CROSS),
		.hbrBackground = hBrush_F0F0F0,
	});


	hFontNormal0 = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));
	hFontB0 = CreateFont(-14, -7, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));
	hFontI0 = CreateFont(-14, -7, 0, 0, FW_NORMAL, TRUE, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));
	hFontU0 = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, TRUE, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));
	hFontS0 = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, TRUE, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));
	hFontLarge1 = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));
	hFontLarge2 = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));

	
#pragma comment(lib, "comctl32.lib")
	INITCOMMONCONTROLSEX icce{};
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_ALL_CLASSES;
	InitCommonControlsEx(&icce);


	return (uatom) ? true : false;
}



bool UI_UnInit() {
	UnregisterClassW(szCaptchaClass, hInst);
	UnregisterClassW(szEmbeddedClass, hInst);
	DeleteObject(hFontNormal0);
	DeleteObject(hFontB0);
	DeleteObject(hFontI0);
	DeleteObject(hFontU0);
	DeleteObject(hFontS0);
	DeleteObject(hFontLarge1);
	DeleteObject(hFontLarge2);
	DeleteObject(hBrush_F0F0F0);
	DeleteObject(hBrush_FFFFFF);

	return false;
}



static DWORD CALLBACK Thread_UIValidate(PVOID uiData) {
	_internal_CAPTCHAWINDOWDATA* data =
		(_internal_CAPTCHAWINDOWDATA*)uiData;
	if (!data) return ERROR_INVALID_PARAMETER;

	if (data->userData->__ownerData__->data.pValidateFunc) {
		if (data->userData->__ownerData__->data.pValidateFunc(data->userData->__ownerData__)) {
			goto passed;
		}
		else goto failed;
	}



	goto failed;


passed:
	{
		Sleep(rand() % 500 + 500);
		//EnableWindow(data->hSubmitButton, TRUE);
		//EnableWindow(data->hInputBox, TRUE);
		SendMessage(data->userData->_hwnd, WM_USER + 0xff, 1, 0);
		return 0;
	}


failed:
	WCHAR oldText[256]{};
	GetWindowTextW(data->hSubmitButton, oldText, 256);
	SetWindowTextW(data->hSubmitButton, L"Validation Failed");
	Sleep(1000);
	SetWindowTextW(data->hSubmitButton, oldText);
	EnableWindow(data->hSubmitButton, TRUE);
	EnableWindow(data->hInputBox, TRUE);
	return 1;
}


#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")



LRESULT WndProc_Captcha(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	_internal_CAPTCHAWINDOWDATA* data =
		(_internal_CAPTCHAWINDOWDATA*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	switch (message) {

	case WM_CREATE: {
#define MYCTLS_VAR_HWND hWnd
#define MYCTLS_VAR_HINST hInst
#define MYCTLS_VAR_HFONT hFontNormal0
#include "ctls.h"


		data = (_internal_CAPTCHAWINDOWDATA*)calloc(1, sizeof(_internal_CAPTCHAWINDOWDATA));
		if (!data) {
			// failed to initialize
			return -1;
		}
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data);

		CREATESTRUCT* cs = (CREATESTRUCT*)(void*)lParam;
		data->userData = (CAPTCHAWINDOWDATA*)cs->lpCreateParams;


		data->hS0 = text(data->userData->ctls_data.text0, 0, 0, 1, 1, SS_CENTER);

		data->hQuestion = text(data->userData->ctls_data.text_question, 0, 0, 1, 1, SS_CENTERIMAGE);
		SendMessage(data->hQuestion, WM_SETFONT, (WPARAM)hFontB0, TRUE);

		data->hInputBox = edit();
		data->hRefreshLink = custom(L"<a id=\"refresh\">Reload</a>", WC_LINK, 0, 0, 1, 1, WS_TABSTOP | SS_CENTER);
		SendMessage(data->hInputBox, WM_SETFONT, (WPARAM)hFontLarge1, TRUE);

		data->hSubmitButton = button(L"&Submit", IDOK);
		data->hCancelButton = button(L"&Cancel", IDCANCEL);
		SendMessage(data->hSubmitButton, WM_SETFONT, (WPARAM)hFontLarge1, TRUE);
		SendMessage(data->hCancelButton, WM_SETFONT, (WPARAM)hFontLarge1, TRUE);


		data->hEmbeddedView = custom(L"", szEmbeddedClass, 0, 0, 1, 1, WS_BORDER);
		SendMessage(data->hEmbeddedView, WM_USER + 12, 0, (LPARAM)data->userData);

		SendMessage(data->hEmbeddedView, WM_USER + WM_PAINT, 0, 0);



#undef MYCTLS_VAR_HWND
#undef MYCTLS_VAR_HINST
#undef MYCTLS_VAR_HFONT
	}
		break;

	case WM_SIZING:
	case WM_SIZE:
	{
		RECT crc{}; GetClientRect(hWnd, &crc);
		SIZE csz{ .cx = crc.right - crc.left,.cy = crc.bottom - crc.top };

		SetWindowPos(data->hS0, 0, 10, 10, csz.cx -20, 20, 0);

		SetWindowPos(data->hEmbeddedView, 0, 10, 40, csz.cx -20, csz.cy - 170, 0);

		SetWindowPos(data->hQuestion, 0, 10, crc.bottom - 120, csz.cx -20, 25, 0);

		SetWindowPos(data->hInputBox, 0, 10, crc.bottom - 90, csz.cx -20 -70, 25, 0);
		SetWindowPos(data->hRefreshLink, 0, csz.cx-10 -60, crc.bottom -90 +3, 60, 22 , 0);
		
		SetWindowPos(data->hSubmitButton, 0, 10, crc.bottom-50, (csz.cx-20)/2-10, 40, 0);
		SetWindowPos(data->hCancelButton, 0, (csz.cx-20)/2+10, crc.bottom-50, (csz.cx-20)/2, 40, 0);

	}
		break;

	case WM_MOVING:
	case WM_MOVE:
	{
		RECT rc{}; GetWindowRect(hWnd, &rc);
		auto sx = GetSystemMetrics(SM_CXSCREEN), sy = GetSystemMetrics(SM_CYSCREEN);
		long _w = 0, _h = 0;
		if (rc.left < 0) { _w = rc.right - rc.left; rc.left = 0; rc.right = _w; }
		if (rc.top < 0) { _h = rc.bottom - rc.top; rc.top = 0; rc.bottom = _h; }
		if (rc.right > sx) { _w = rc.right - rc.left; rc.right = sx; rc.left = sx - _w; }
		if (rc.bottom > sy) { _h = rc.bottom - rc.top; rc.bottom = sy; rc.top = sy - _h; }
		SetWindowPos(hWnd, 0, rc.left, rc.top,
			rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	case WM_NCHITTEST:
	{
		auto $ = DefWindowProc(hWnd, message, wParam, lParam);
		if ($ == HTCLIENT) $ = HTCAPTION;
		return $;
	}
		break;


	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{

		case NM_CLICK:          // Fall through to the next case.
		case NM_RETURN:
		{
			PNMLINK pNMLink = (PNMLINK)lParam;
			LITEM   item = pNMLink->item;

			if (wcscmp(item.szID, L"refresh") == 0) {
				SendMessage(data->hEmbeddedView, WM_USER + WM_PAINT, 0, 0);
			}

			break;
		}

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		break;


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case IDOK:
		{
			EnableWindow(data->hInputBox, FALSE);
			EnableWindow(data->hSubmitButton, FALSE);

			GetWindowTextW(data->hInputBox, data->userData->user_answer,
				sizeof(data->userData->user_answer) / sizeof(data->userData->user_answer[0]));

			if (HANDLE _ = CreateThread(0, 0, Thread_UIValidate, data, CREATE_SUSPENDED, 0)) {
				ResumeThread(_); CloseHandle(_);
			}
		}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;


	case WM_USER+0xff:
		DestroyWindow(hWnd);
		break;


	case WM_CLOSE:
		if (IDOK != MessageBoxW(hWnd, L"Interrupt validation?", L"CAPTCHA",
			MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION)) break;
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		if (data) { free(data); data = nullptr; }
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



LRESULT WndProc_Embedded(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_USER+WM_PAINT: {
		CAPTCHAWINDOWDATA* data = (CAPTCHAWINDOWDATA*)
			GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!data) break;

		switch (data->__ownerData__->data_type) {
		case CAPTCHADATA::T_data_type::OwnerDraw: {
			data->__ownerData__->data.pGenerateFunction(data->__ownerData__);
			HDC hdc = GetDC(hWnd);
			data->__ownerData__->data.pDrawFunction(hdc, data->__ownerData__);
			if (hdc) ReleaseDC(hWnd, hdc);
		}
			break;
		case CAPTCHADATA::T_data_type::WndEmbed: {
			SendMessage(data->__ownerData__->data.hCaptchaWnd,
				data->__ownerData__->data.nCustomRefreshMessage,
				wParam, lParam);
			SendMessage(hWnd, WM_SIZE, 0, 0);
		}
			break;
		default:
			break;
		}
		return TRUE;
	}
		break;
	case WM_SIZE:
	case WM_SIZING:
	{
		CAPTCHAWINDOWDATA* data = (CAPTCHAWINDOWDATA*)
			GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (data && data->__ownerData__->data_type ==
			CAPTCHADATA::T_data_type::WndEmbed) {
			RECT rc{}; GetClientRect(hWnd, &rc);
			SetWindowPos(data->__ownerData__->data.hCaptchaWnd, 0,
				rc.left, rc.top, rc.bottom - rc.top, rc.right - rc.left, 0);
		}
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		do {
			CAPTCHAWINDOWDATA* data = (CAPTCHAWINDOWDATA*)
				GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (!data) break;

			if (data->__ownerData__->data_type == CAPTCHADATA::T_data_type::OwnerDraw)
				data->__ownerData__->data.pDrawFunction(hdc, data->__ownerData__);

		} while (0);
		EndPaint(hWnd, &ps);
	}
		break;
	case WM_USER+12:
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)lParam);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



