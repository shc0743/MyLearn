#include "targetver.h"
#include <ConsoleApi.h>
#include "../../../w32oop/w32use.hpp"

namespace app::ui {
	class ConsoleWindow : public Window {
	public:
		ConsoleWindow(int x, int y, int wc, int hc) :
			fontSize(defaultFontSize),
			wc(wc), hc(hc),
			Window(L"Console", (defaultFontSize / 2) * wc, defaultFontSize * hc, x, y, WS_OVERLAPPEDWINDOW, WS_EX_LAYERED) {
			font_name = L"Consolas";
		}
		~ConsoleWindow() {
			if (hHostedProcess) CloseHandle(hHostedProcess);
			if (hConsole) ClosePseudoConsole(hConsole);
		}
		
	protected:
		static int defaultFontSize;
		int fontSize = 14;
		std::wstring font_name;
		int wc, hc;

		COLORREF get_window_background_color() override {
			return RgbColor("#000");
		}

		void onCreated() override;
		void onDestroy() override;

		void doPaint(EventData& ev);

		void setup_event_handlers() override {
			WINDOW_add_handler(WM_PAINT, doPaint);
		}

	public:
		int getFontSize() const { return fontSize; };
		void setFontSize(int _) { fontSize = _; };
		static int getDefaultFontSize() { return defaultFontSize; }

	protected:
		bool _hosted = false;
		HANDLE hHostedProcess{};
		HPCON hConsole{};
		w32FileHandle outputReadSide, inputWriteSide;
		HFONT myFont;

		void worker();
		std::thread workerThread;
		
	public:
		bool SpawnApplication(_In_opt_ PCWSTR app, _In_opt_ PCWSTR cmd);
	};
}
