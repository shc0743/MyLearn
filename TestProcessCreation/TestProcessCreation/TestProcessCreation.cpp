// TestProcessCreation.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include "../../../win32-oop-util/Window.hpp"
#include "../../resource/tool.h"
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(linker,"/subsystem:windows /entry:wmainCRTStartup")

using namespace w32oop;
using namespace w32oop::foundation;

class MyTestProcessCreation : public Window {
public:
	MyTestProcessCreation() : Window(L"测试 Windows 进程的创建。", 320, 200) {}
	~MyTestProcessCreation() {
		for (auto& i : MyProcess) {
			CloseHandle(i);
		}
	}
private:
	Static current_count;
	Static max_count_tip;
	Edit max_count;
	Button start_button;
	Button kill_all_child_processes;
	CheckBox associate_with_job_object;

	vector<HANDLE> MyProcess;
	HANDLE hJob = NULL;
protected:
	void onCreated() override {
		add_style(WS_OVERLAPPEDWINDOW); // 添加窗口样式
		current_count.set_parent(this);
		current_count.create(L"当前进程数量: -", 1, 1, 0, 0);
		max_count_tip.set_parent(this);
		max_count_tip.create(L"创建进程数量:", 1, 1, 0, 0);
		max_count.set_parent(this);
		max_count.create(L"", 1, 1, 0, 0, ES_NUMBER | Edit::STYLE);
		max_count.text(L"100");
		start_button.set_parent(this);
		start_button.create(L"创建这些进程", 1, 1, 0, 0, Button::STYLE | BS_DEFPUSHBUTTON);
		kill_all_child_processes.set_parent(this);
		kill_all_child_processes.create(L"结束所有子进程", 1, 1, 0, 0);
		associate_with_job_object.set_parent(this);
		associate_with_job_object.create(L"关联到 Job 对象", 1, 1, 0, 0);
		current_count.text(L"当前进程数量: 0");

		// 设置事件处理程序
		associate_with_job_object.check();
		associate_with_job_object.onChanged([this](EventData& data) {
			if (!associate_with_job_object.checked()) {
				if (MessageBoxW(hwnd, L"温馨提示：如果不关联到 Job 对象，关闭窗口后您将被迫手动结束所有子进程！\n"
					L"您真的要这么做吗？", L"温馨提示", MB_YESNO | MB_ICONQUESTION) != IDYES) {
					associate_with_job_object.check();
					return;
				}
			}
		});

		kill_all_child_processes.onClick([this](EventData& data) {
			thread([this] {
				w32oop::util::WindowRAIIHelper _([this] {
					current_count.text(L"当前进程数量: " + to_wstring(MyProcess.size()));
					kill_all_child_processes.enable();
					start_button.enable();
				});
				for (auto& i : MyProcess) {
					TerminateProcess(i, 0);
					CloseHandle(i);
				}
				MyProcess.clear();
			}).detach();
			current_count.text(L"正在执行，请不要操作UI...");
			kill_all_child_processes.disable();
			start_button.disable();
		});

		hJob = CreateJobObjectW(NULL, NULL);
		if (hJob == NULL) {
			MessageBoxW(hwnd, L"无法创建任务对象。应用程序可能无法正常运行。", NULL, MB_ICONERROR);
			return;
		}
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo = { 0 };
		jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jobInfo, sizeof(jobInfo))) {
			MessageBoxW(hwnd, L"无法设置任务对象。应用程序可能无法正常运行。", NULL, MB_ICONERROR);
			CloseHandle(hJob);
			hJob = NULL;
		}

		start_button.onClick([this](EventData& data) {
			thread([this] {
				w32oop::util::WindowRAIIHelper _([this] {
					current_count.text(L"当前进程数量: " + to_wstring(MyProcess.size()));
					kill_all_child_processes.enable();
					start_button.enable();
				});
				long long n = atoll(ws2c(max_count.text()));
				if (n <= 0) {
					MessageBoxW(hwnd, L"请输入正确的数值", NULL, MB_ICONERROR);
					return;
				}

				if (!hJob) {
					MessageBoxW(hwnd, L"无效的 hJob", NULL, MB_ICONERROR);
					return;
				}

				// 创建进程
				STARTUPINFOW si{ .cb = sizeof(si) };
				PROCESS_INFORMATION pi{};
				wchar_t cmd[] = L"Process -";
				wstring executable = GetProgramDirW();
				auto szExecutable = executable.c_str();
				bool withJob = (associate_with_job_object.checked());
				for (long long i = 0; i < n; ++i) {
					AutoZeroMemory(pi);

					if (!CreateProcessW(szExecutable, cmd, 0, 0, 0, CREATE_SUSPENDED, 0, 0, &si, &pi)) continue;
					if (pi.hThread) {
						CloseHandle(pi.hThread);
					}
					if (withJob) if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
						TerminateProcess(pi.hProcess, 0);
						CloseHandle(pi.hProcess);
						continue;
					}
					MyProcess.push_back(pi.hProcess);
				}
			}).detach();
			current_count.text(L"正在执行，请不要操作UI...");
			kill_all_child_processes.disable();
			start_button.disable();
		});
	}
	
	void onSize(EventData& data) {
		RECT rc{}; GetClientRect(hwnd, &rc);
		current_count.resize(10, 10, rc.right - 20, 25);
		max_count_tip.resize(10, 45, 100, 25);
		max_count.resize(120, 45, rc.right - 130, 25);
		start_button.resize(10, 80, (rc.right - 20 - 5) / 2, 30);
		kill_all_child_processes.resize((rc.right - 20) / 2 + 15, 80, (rc.right - 20 - 5) / 2, 30);
		associate_with_job_object.resize(10, 125, rc.right - 20, 25);
	}
	virtual void setup_event_handlers() override {
		WINDOW_add_handler(WM_SIZE, onSize);
		WINDOW_add_handler(WM_SIZING, onSize);
		WINDOW_add_handler(WM_SETFOCUS, [this](EventData& data) {
			SetFocus(max_count);
		});
	}
};

int wmain(int argc, wchar_t* argv[]) {
	if (argc > 1 && L"-"s == argv[1]) return 0;
	if (true) {
		Window::set_global_option(Window::Option_DebugMode, true);
	}
	MyTestProcessCreation app;
	app.create();
	app.set_accelerator(LoadAcceleratorsW(GetModuleHandleW(NULL), MAKEINTRESOURCE(1000)));
	app.set_main_window();
	app.show();
	app.center();
	app.focus();
	return app.run();
}

