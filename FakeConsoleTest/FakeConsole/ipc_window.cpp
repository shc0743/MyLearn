#include "ipc_window.hpp"
#include "console_window.hpp"
#include "basedef.hpp"
using namespace std;

void app::ipc::IPCWindow::requestCreateConsole(EventData& ev) {
	if (ev.wParam != 1885) {
		ev.returnValue(ERROR_WRONG_PASSWORD);
		return;
	}
	if (!w32oop::util::memory::MemoryChecker<sizeof(CreateConsoleRequestInfo)>().readable((PVOID)ev.lParam)) {
		ev.returnValue(STATUS_ACCESS_VIOLATION);
		return;
	}
	auto req = CreateConsoleRequestInfo(*((CreateConsoleRequestInfo*)ev.lParam));
	auto pWindow = shared_ptr<ui::ConsoleWindow>(new ui::ConsoleWindow(req.x, req.y, req.w, req.h));
	pWindow->create();
	if (pWindow->SpawnApplication(req.lpApplication, req.lpCommand) == false) {
		ev.returnValue(GetLastError());
		DestroyWindow(*pWindow);
		return;
	}
	pWindow->show(req.nCmdShow);
	app::windows.push_back(std::move(pWindow));
}
