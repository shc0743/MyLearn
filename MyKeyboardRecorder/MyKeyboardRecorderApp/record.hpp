#pragma once
#include "../../../w32oop/w32use.hpp"

namespace app {
	constexpr uint8_t RecordMagic[] = {
		0xf3, 0x1a, 0x9e, 0xc1, 0x3d, 0xb3, 0x46, 0xe1, 0xb0, 0x6f, 0x4a, 0x55, 0x93, 0xb5, 0x22, 0x06
	}; // random bytes, use as file magic

	struct RecordItem {
		uint64_t t;
		uint8_t modifier;
		uint8_t reserved;
		uint8_t keycode[6];
	};

	class Record : public Window {
	public:
		Record() : Window(L"Record Keyboard", 500, 200, 0, 0, WS_OVERLAPPEDWINDOW) {}

		Window* runner() {
			create();
			set_main_window();
			center();
			show();
			return this;
		}
	protected:
		//class RecordHook : public w32oop::system::Hook {
		//	virtual LRESULT callback(int nCode, WPARAM wParam, LPARAM lParam);
		//public:
		//	Record* owner = NULL;
		//};

	protected:
		Button btnElevate, btnStart, btnStop, btnChooseFile;
		Static staticFile;
		Edit editFilesave;
		w32FileHandle hFile;

		//RecordHook hook;

		void StartOrPause();
		void Stop();

		bool isStarted = false, isRunning = false;
		uint64_t startTime = 0;
		std::vector<RecordItem> records;

		void onCreated();
		void onDestroy();
		void doLayout(EventData& ev);
		void onClose(EventData& ev);

		bool rawInputRegistered = false;
		void registerRawInput();
		void unregisterRawInput();
		void onRawInput(EventData& ev);

		virtual void setup_event_handlers() override {
			WINDOW_add_handler(WM_SIZING, doLayout);
			WINDOW_add_handler(WM_SIZE, doLayout);
			WINDOW_add_handler(WM_CLOSE, onClose);
			WINDOW_add_handler(WM_INPUT, onRawInput);
		}

	public:
		void addRecord(RecordItem item);
	};

}