#pragma once
#include <Windows.h>


namespace CaptchaEmbedded {


// 



PCWSTR GetCaptchaWindowClassName();


struct CAPTCHADATA;



struct CAPTCHAWINDOWDATA {
	DWORD cb;

	HWND _hwnd;
	CAPTCHADATA* __ownerData__;

	bool centerWindow;
	POINT pos;
	SIZE size;
	WCHAR caption[256];

	bool resizable;


	struct CTLS_DATA {
		PCWSTR
			text0,
			text_question;
	} ctls_data;


	WCHAR user_answer[1024]{};


};
	
	
typedef struct CAPTCHADATA {
	DWORD cb;

	DWORD version;

	enum class T_data_type { Unknown = 0, WndEmbed = 1, OwnerDraw = 2 } data_type;
	struct T_data {
		HWND hCaptchaWnd;
		UINT nCustomRefreshMessage;
		void(WINAPI*pGenerateFunction)(CAPTCHADATA*); // also redraw
		void(WINAPI*pDrawFunction)(HDC, CAPTCHADATA*); // also redraw

		bool(WINAPI* pValidateFunc)(CAPTCHADATA*);
	} data;

	CAPTCHAWINDOWDATA* wData;

	// v4.2.0 Add: [[property]] difficulty
	enum class T_difficulty { Normal = 0, Easy = 1, Difficult = 2 } difficulty;

	signed char __custom_data_for_CAPTCHA_provider__ [1024];


} *PCAPTCHADATA;


class CAPTCHAHANDLE {
public:
	CAPTCHAHANDLE() = default;
	~CAPTCHAHANDLE() {};

	CAPTCHAHANDLE(CAPTCHADATA& data) { this->m_handle = &data; };
	CAPTCHAHANDLE(CAPTCHADATA* data) { this->m_handle = data; };


	bool isValid() const { return bool(this->m_handle); }
	operator PCAPTCHADATA() const { return this->m_handle; }


private:
	PCAPTCHADATA m_handle;


};



}



void WINAPI CaptchaUtil_generateMathProblem(CaptchaEmbedded::PCAPTCHADATA data);
void WINAPI CaptchaUtil_drawMathProblem(HDC hdc, CaptchaEmbedded::PCAPTCHADATA data);
bool WINAPI CaptchaUtil_validateMathProblem(CaptchaEmbedded::PCAPTCHADATA data);



