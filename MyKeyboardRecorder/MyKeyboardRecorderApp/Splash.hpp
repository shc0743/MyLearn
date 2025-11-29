#pragma once
#include "../../../w32oop/w32use.hpp"

namespace app {
	class Splash : public Window {
	public:
		Splash() : Window(L"Splash", 300, 200, 0, 0, WS_POPUPWINDOW) {}
	protected:
		Static SplashText;

		void onCreated() {
			SplashText = Static(hwnd, L"Please wait...", 1, 1, 0, 0, Static::STYLE | SS_CENTER | SS_CENTERIMAGE);
			SplashText.create();
			dispatchEvent(EventData(hwnd, WM_SIZING, 0, 0));
		}
		void doLayout(const EventData& ev) {
			RECT rc{}; GetClientRect(hwnd, &rc);
			SplashText.resize(rc);
		}

		virtual void setup_event_handlers() override {
			WINDOW_add_handler(WM_SIZING, doLayout);
			WINDOW_add_handler(WM_SIZE, doLayout);
			WINDOW_add_handler(WM_CLOSE, [](EventData& ev) { ev.preventDefault(); });
			WINDOW_add_handler(WM_USER + WM_CLOSE, [this](EventData&) { destroy(); });
		}
	public:
		void setSplashText(std::wstring text) {
			SplashText.text(text);
		}
		void finish() {
			post(WM_USER + WM_CLOSE);
		}
	};

}