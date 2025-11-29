#pragma once
#include "../../../w32oop/w32use.hpp"

namespace app {
	class Select : public Window {
	public:
		Select() : Window(L"Select Application Mode", 480, 320, 0, 0, WS_OVERLAPPEDWINDOW) {}

		Window* runner() {
			create();
			set_main_window();
			center();
			show();
			return this;
		}
	protected:
		Button btn1, btn2;

		void onCreated();
		void doLayout(EventData& ev);

		virtual void setup_event_handlers() override {
			WINDOW_add_handler(WM_SIZING, doLayout);
			WINDOW_add_handler(WM_SIZE, doLayout);
		}
	};

}