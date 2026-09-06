#include "targetver.h"
#include "../../../w32oop/w32use.hpp"

namespace app::ipc {
	constexpr UINT IPC_RequestCreateConsole = WM_USER + 0x11;

	class IPCWindow : public Window {
	public:
		IPCWindow() :Window(L"IPC Window", 1, 1, 0, 0, WS_OVERLAPPED, 0) {}

	protected:
		void requestCreateConsole(EventData& ev);

		void setup_event_handlers() override {
			WINDOW_add_handler(IPC_RequestCreateConsole, requestCreateConsole);
		}
	};

	struct CreateConsoleRequestInfo {
		LPCWSTR lpApplication, lpCommand;
		int x, y, w, h;
		int nCmdShow;
	};
}
