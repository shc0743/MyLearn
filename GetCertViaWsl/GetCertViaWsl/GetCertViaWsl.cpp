// GetCertViaWsl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <w32use.hpp>
#include <commctrl.h>
#include <shlobj.h>
#include <shobjidl.h>
#include "resource.h"
using namespace std;

#pragma comment(linker, "\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

HINSTANCE hInst;

DWORD exec_app(std::wstring command, int show) {
	STARTUPINFOW si{}; PROCESS_INFORMATION pi{};

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = show;

	auto cl = std::make_unique<WCHAR[]>(command.size() + 1);
	memcpy(cl.get(), command.c_str(), (command.size() + 1) * sizeof(WCHAR));

	if (!CreateProcessW(NULL, cl.get(), NULL, NULL, FALSE,
		CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		return -1;
	}

	DWORD code = 0;
	ResumeThread(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &code);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return code;
}


class GetCertViaWslWindow : public Window {
public:
	GetCertViaWslWindow() :Window(L"Get Certificate via Windows Subsystem for Linux", 640, 480, 0, 0, WS_OVERLAPPEDWINDOW) {};
protected:
	virtual const HICON get_window_icon() const override {
		return LoadIconW(hInst, MAKEINTRESOURCEW(IDI_ICON_APP));
	}
	virtual void setup_event_handlers() override {
		WINDOW_add_handler(WM_SIZING, doLayout);
		WINDOW_add_handler(WM_SIZE, doLayout);
		WINDOW_add_handler(WM_CLOSE, [this](EventData& ev) {
			if (!canClose) ev.preventDefault();
		});
	}
protected:
	Static textList; Button adjSplit;
	Edit editList;
	Static saveToDir; Edit editSaveToDir; Button btnBrowse;
	Button btnExec, btnShutdownWsl;
	wstring splitChar = L",";
	bool initialized = false;
	bool canClose = true;
	void onCreated() override {
		textList = Static(*this, L"Enter domains you need:", 1, 1, 1, 1, Static::STYLE | SS_CENTER);
		textList.create();
		adjSplit = Button(*this, L"…", 1, 1);
		adjSplit.create();
		adjSplit.onClick([this](EventData& ev) {
			InputDialog i(L"Set split character");
			auto r = i.getInput<wstring>(L"Split character", splitChar);
			if (r.has_value()) {
				splitChar = r.value();
			}
		});
		editList = Edit(*this, L"", 1, 1, 1, 1, ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | WS_HSCROLL | ES_WANTRETURN | Edit::STYLE);
		editList.create();
		saveToDir = Static(*this, L"Save to:", 1, 1);
		saveToDir.create();
		editSaveToDir = Edit(*this, L"", 1, 1);
		editSaveToDir.create();
		btnBrowse = Button(*this, L"Browse...", 1, 1);
		btnBrowse.create();
		btnBrowse.onClick([this](EventData& ev) {
			IFileDialog* pFileDialog = NULL;
			HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileDialog, reinterpret_cast<void**>(&pFileDialog));
			if (!SUCCEEDED(hr)) return;
			// 设置为文件夹选择模式
			DWORD dwOptions;
			pFileDialog->GetOptions(&dwOptions);
			pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
			// 设置对话框标题
			pFileDialog->SetTitle(L"Choose a Folder");
			// 显示对话框
			hr = pFileDialog->Show(hwnd);
			if (SUCCEEDED(hr)) {
				// 获取选择的文件夹
				IShellItem* pItem;
				hr = pFileDialog->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
					if (SUCCEEDED(hr)) {
						// 将路径设置到编辑框
						editSaveToDir.text(pszFilePath);
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileDialog->Release();
		});
		btnExec = Button(*this, L"Execute", 1, 1);
		btnExec.create();
		btnExec.onClick([this](EventData& ev) {
			exec();
		});
		btnShutdownWsl = Button(*this, L"Shutdown WSL", 1, 1);
		btnShutdownWsl.create();
		btnShutdownWsl.onClick([this](EventData& ev) {
			int cf = 0;
			TaskDialog(hwnd, hInst, L"Confirm Shutdown",
				L"This will shutdown all WSL distributions. Are you sure?",
				L"wsl --shutdown", TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON, TD_WARNING_ICON, &cf);
			if (cf != IDYES) return;
			DWORD r = exec_app(L"wsl --shutdown", SW_HIDE);
			if (r != 0) {
				wstring msg = L"WSL shutdown command exited with code " + to_wstring(r) + L".";
				MessageBoxW(hwnd, msg.c_str(), L"Error", MB_ICONERROR);
			}
			else {
				MessageBoxW(hwnd, L"WSL has been shutdown successfully.", L"Success", MB_ICONINFORMATION);
			}
		});
		initialized = true;
		post(WM_SIZE);
	}
	void doLayout(EventData& ev) {
		if (!initialized) return;
		RECT rc{}; GetClientRect(hwnd, &rc);
		// Layout controls
		const int margin = 10;  // 边距
		const int spacing = 10;  // 控件间距
		int clientWidth = rc.right - rc.left;
		int clientHeight = rc.bottom - rc.top;
		int availableWidth = clientWidth - 2 * margin;
		int yPos = margin;
		textList.resize(margin, yPos, availableWidth - 20, 20);
		adjSplit.resize(margin + availableWidth - 20, yPos, 20, 20);
		yPos += 20 + spacing;
		int editListHeight = clientHeight - yPos - 60 - margin - 2 * spacing; // 计算剩余高度
		if (editListHeight < 50) editListHeight = 50; // 最小高度
		editList.resize(margin, yPos, availableWidth, editListHeight);
		yPos += editListHeight + spacing;
		saveToDir.resize(margin, yPos, 60, 25);
		int browseBtnWidth = 80;  // 浏览按钮宽度
		int pathEditWidth = availableWidth - 60 - browseBtnWidth - 2 * spacing;
		editSaveToDir.resize(margin + 60 + spacing, yPos, pathEditWidth, 25);
		btnBrowse.resize(margin + 60 + spacing + pathEditWidth + spacing, yPos, browseBtnWidth, 25);
		yPos += 25 + spacing;
		btnExec.resize(margin, yPos, availableWidth - 120, 30);
		btnShutdownWsl.resize(margin + availableWidth - 120 + spacing, yPos, 120 - spacing, 30);
	}
	void exec() {
		wstring domains = editList.text();
		if (domains.empty()) {
			MessageBoxW(hwnd, L"Please enter at least one domain.", L"Error", MB_ICONERROR);
			return;
		}
		wstring saveTo = editSaveToDir.text();
		if (saveTo.empty()) {
			MessageBoxW(hwnd, L"Please select a directory to save certificates.", L"Error", MB_ICONERROR);
			return;
		}
		// Check if directory exists
		DWORD ftyp = GetFileAttributesW(saveTo.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
			MessageBoxW(hwnd, L"The specified directory does not exist.", L"Error", MB_ICONERROR);
			return;
		}
		// Split domains by splitChar
		vector<wstring> domainList;
		if (domains.find(L"\n") != wstring::npos) {
			w32oop::util::str::operations::replace(domains, L"\r\n", L"\n");
			w32oop::util::str::operations::replace(domains, L"\n", splitChar);
		}
		w32oop::util::str::operations::split(domains, splitChar, domainList);
		// Trim spaces
		for (auto& d : domainList) {
			w32oop::util::str::operations::trim(d);
		}
		// Remove empty entries
		domainList.erase(remove_if(domainList.begin(), domainList.end(),
			[](const wstring& s) { return s.empty(); }), domainList.end());
		// Check if empty
		if (domainList.empty()) {
			MessageBoxW(hwnd, L"No valid domains found.", L"Error", MB_ICONERROR);
			return;
		}
		// ------
		// Prepare WSL command
		DWORD testEnv = exec_app(L"wsl -- ~/.acme.sh/acme.sh --help", 0);
		if (testEnv == -1) {
			MessageBoxW(hwnd, L"No WSL Distribution found. Please install by using `wsl.exe --install <Distro>`.", L"Error", MB_ICONERROR);
			return;
		}
		else if (testEnv != 0) {
			MessageBoxW(hwnd, L"WSL Distribution found, but acme.sh not installed. Please install by using `wsl -- curl https://get.acme.sh | sh`.", L"Error", MB_ICONERROR);
			return;
		}
		// Build domain arguments
		wstring domainArgs = L"~/.acme.sh/acme.sh --issue";
		for (const auto& d : domainList) {
			domainArgs += L" -d " + d;
		}
		// Ask verfication method
		InputDialog i(L"Select Verification Method", 600); std::optional<wstring> r;
		r = i.getInput<wstring>(L"Verification Method (full data, such as `--dns dns_cf`)", L"--dns dns_cf");
		if (!r.has_value()) return;
		domainArgs += L" " + r.value();
		// Ask key length
		i = InputDialog(L"Set Key Length");
		r = i.getInput<wstring>(L"Key Length (such as `4096` or `ec-384`)", L"ec-384");
		if (!r.has_value()) return;
		domainArgs += L" --keylength " + r.value();
		// Confirm the command
		wstring cpSrcPath = L"~/.acme.sh/" + domainList[0] + ((r.value().find(L"ec-") != wstring::npos) ? L"_ecc" : L"");
		wstring cpDstPath = saveTo.ends_with(L"\\") ? saveTo : (saveTo + L"\\");
		w32oop::util::str::operations::replace(cpDstPath, L"\\", L"/");
		cpDstPath = L"/mnt/" + towlower(cpDstPath.substr(0, 1)[0]) + cpDstPath.substr(2); // C:/path -> /mnt/c/path
		wstring cmd = L"wsl -- cd ~ && " + domainArgs + L" && cp -r \"" + cpSrcPath + L"/\" \"" + cpDstPath + L"\" && rm -rf \"" + cpSrcPath + L"\" && echo 'Success!' && exit";
		int cf = 0;
		TaskDialog(hwnd,hInst, L"Confirm Command",
			L"The command to be executed in WSL is shown below. Please confirm that you have configured the verification method correctly and that you want to proceed.",
			cmd.c_str(), TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON, TD_INFORMATION_ICON, &cf);
		if (cf != IDYES) return;
		// Execute the command
		canClose = false;
		btnExec.disable();
		adjSplit.disable();
		editList.disable();
		editSaveToDir.disable();
		btnBrowse.disable();
		thread([this](wstring cmd) {
			w32oop::util::RAIIHelper R([this]() {
				canClose = true;
				btnExec.enable();
				adjSplit.enable();
				editList.enable();
				editSaveToDir.enable();
				btnBrowse.enable();
			});

			DWORD r = exec_app(cmd, SW_SHOW);
			if (r == -1) {
				MessageBoxW(hwnd, L"Failed to execute WSL command.", L"Error", MB_ICONERROR);
			}
			else if (r != 0) {
				wstring msg = L"WSL command exited with code " + to_wstring(r) + L". Please check the output window for details.";
				MessageBoxW(hwnd, msg.c_str(), L"Error", MB_ICONERROR);
			}
			else {
				MessageBoxW(hwnd, L"Certificates obtained and copied successfully!", L"Success", MB_ICONINFORMATION);
			}
		}, cmd).detach();
	}
};

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow
) {
	::hInst = hInstance;
	if (!SUCCEEDED(::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
		MessageBoxW(NULL, L"Failed to initialize COM library.", L"Error", MB_ICONERROR);
		return -1;
	}
	w32oop::util::RAIIHelper R([]() { ::CoUninitialize(); });
	// Running your app
	GetCertViaWslWindow app;
	app.create();
	app.set_main_window();
	app.center();
	app.show(nCmdShow);
	return app.run();
}
