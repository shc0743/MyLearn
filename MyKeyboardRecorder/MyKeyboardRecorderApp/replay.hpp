#pragma once
#include "../../../w32oop/w32use.hpp"
#include <thread>

namespace app {
	class Replay : public Window {
	public:
		Replay() : Window(L"Replay Record", 320, 200, 0, 0, WS_OVERLAPPEDWINDOW) {}

		Window* runner() {
			create();
			set_main_window();
			center();
			show();
			return this;
		}
	protected:
		Static textFile; Edit editFile; Button btnChooseFile;
		Static textComPort; Edit editComPort;
		Button btnStartReplay; // 开始之后按钮文本变成“Stop”，复用控件

		void onCreated();
		void onDestroy();
		void doLayout(EventData& ev);
		void onFile(EventData& ev);
		void onClose(EventData& ev);

		virtual void setup_event_handlers() override {
			WINDOW_add_handler(WM_SIZING, doLayout);
			WINDOW_add_handler(WM_SIZE, doLayout);
			WINDOW_add_handler(WM_DROPFILES, onFile);
			WINDOW_add_handler(WM_CLOSE, onClose);
		}

		bool isRunning = false;
		std::thread worker;
		void Worker();
	};

}