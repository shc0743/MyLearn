#include "console_window.hpp"
#include "ipc_window.hpp"
#include "basedef.hpp"
using namespace std;

int app::ui::ConsoleWindow::defaultFontSize = 14;

void app::ui::ConsoleWindow::onCreated() {
	SetLayeredWindowAttributes(hwnd, 0, (BYTE)0xF7, LWA_ALPHA);

	float factor = get_dpi_scale_factor();
	myFont = CreateFontW(
		(int)(-(float(fontSize)) * factor - 0.5f), (int)(-(float(fontSize / 2)) * factor - 0.5f),
		0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE,
		font_name.c_str());
}

void app::ui::ConsoleWindow::onDestroy() {
	if (workerThread.joinable()) {
		workerThread.join();
	}

	if (myFont) DeleteFont(myFont);

	for (auto& w : app::windows) {
		if (w && w.get() != this && w->is_alive()) return;
	}
	DestroyWindow(*ipcWindow);
}

bool app::ui::ConsoleWindow::SpawnApplication(_In_opt_ PCWSTR app, _In_opt_ PCWSTR cmd) {
	// https://learn.microsoft.com/zh-cn/windows/console/creating-a-pseudoconsole-session
	if (_hosted) throw runtime_error("This console window already hosted an application.");

	// - Close these after CreateProcess of child application with pseudoconsole object.
	HANDLE inputReadSide{}, outputWriteSide{};
	// - Hold onto these and use them for communication with the child through the pseudoconsole.
	HANDLE outputReadSide{}, inputWriteSide{};

	if (!CreatePipe(&inputReadSide, &inputWriteSide, NULL, 0)) {
		return false;
	}
	if (!CreatePipe(&outputReadSide, &outputWriteSide, NULL, 0)) {
		return false;
	}

	if (FAILED(CreatePseudoConsole(COORD{ .X = (SHORT)wc,.Y = (SHORT)hc }, inputReadSide, outputWriteSide, 0, &hConsole))) {
		CloseHandle(inputReadSide);
		CloseHandle(outputWriteSide);
		CloseHandle(outputReadSide);
		CloseHandle(inputWriteSide);
		return false;
	}

	STARTUPINFOEXW si{ sizeof(si) };
	PROCESS_INFORMATION pi{};
	std::unique_ptr<uint8_t[]> attributeList;
	DWORD flags = CREATE_SUSPENDED | CREATE_BREAKAWAY_FROM_JOB | EXTENDED_STARTUPINFO_PRESENT;
	SIZE_T need{};
	bool ok = false;
	InitializeProcThreadAttributeList(0, 1, 0, &need);
	if (need && need < 32768) {
		attributeList = make_unique<uint8_t[]>(need);
		if (InitializeProcThreadAttributeList((PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get(),
			1, 0, &need)) {
			if (UpdateProcThreadAttribute(
				(PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get(), 0,
				PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
				hConsole,
				sizeof(hConsole),
				NULL, NULL
			)) {
				ok = true;
			}
		}
	}
	if (!ok) {
		CloseHandle(inputReadSide);
		CloseHandle(outputWriteSide);
		CloseHandle(outputReadSide);
		CloseHandle(inputWriteSide);
		return false;
	}
	si.StartupInfo.cb = sizeof(STARTUPINFOEXW);
	si.StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	si.StartupInfo.wShowWindow = SW_SHOWNORMAL;
	si.lpAttributeList = PPROC_THREAD_ATTRIBUTE_LIST(attributeList ? attributeList.get() : nullptr);
	wstring c = cmd ? cmd : L"";
	WCHAR COMSPEC[260]{};
	GetEnvironmentVariableW(L"COMSPEC", COMSPEC, 260);
	if (!CreateProcessW((app || cmd) ? (app[0] ? app : NULL) : COMSPEC, cmd ? c.data() : COMSPEC,
		NULL, NULL, FALSE, flags, NULL, NULL, (LPSTARTUPINFOW)&si, &pi)) {
		if (attributeList) DeleteProcThreadAttributeList((PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get());
		CloseHandle(inputReadSide);
		CloseHandle(outputWriteSide);
		CloseHandle(outputReadSide);
		CloseHandle(inputWriteSide);
		return false;
	}
	if (attributeList) DeleteProcThreadAttributeList((PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get());
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	CloseHandle(inputReadSide);
	CloseHandle(outputWriteSide);
	this->outputReadSide = outputReadSide;
	this->inputWriteSide = inputWriteSide;

	workerThread = std::thread(std::bind(&app::ui::ConsoleWindow::worker, this));

	return true;
}

void app::ui::ConsoleWindow::worker() {

}

void app::ui::ConsoleWindow::doPaint(EventData& ev) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	// TODO: 在此处添加使用 hdc 的任何绘图代码...
	{

	}
	EndPaint(hwnd, &ps);
}


