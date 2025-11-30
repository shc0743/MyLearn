#include "replay.hpp"
#include "start.hpp"
#include "record.hpp"
#include "ch9329.hpp"
#include "serial.hpp"
using namespace std;

void app::Replay::onCreated() {
	textFile = Static(hwnd, L"File:", 1, 1);
	editFile = Edit(hwnd, L"", 1, 1);
	btnChooseFile = Button(hwnd, L"Choose File", 1, 1);
	btnStartReplay = Button(hwnd, L"Start", 1, 1);
	textComPort = Static(hwnd, L"COM Port:", 1, 1);
	editComPort = Edit(hwnd, L"1", 1, 1, 0, 0, Edit::STYLE | ES_NUMBER);
	textFile.create(); editFile.create(); btnChooseFile.create();
	textComPort.create(); editComPort.create();
	btnStartReplay.create();

	btnChooseFile.onClick([this](EventData&) {
		wchar_t filePath[MAX_PATH] = L"";

		OPENFILENAMEW ofn = {};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"Record Files\0*.kbr\0All Files\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameW(&ofn)) {
			editFile.text(filePath);
		}
	});

	btnStartReplay.onClick([this](EventData&) {
		if (isRunning) {
			isRunning = false;
			btnChooseFile.enable();
			editFile.enable();
			btnStartReplay.text(L"Start");
			return;
		}
		std::wstring filePath = editFile.text();
		if (!filePath.empty()) {
			if (worker.joinable()) {
				worker.detach();
			}
			worker = std::thread(std::bind(&Replay::Worker, this));
			isRunning = true;
			btnChooseFile.disable();
			editFile.disable();
			btnStartReplay.text(L"Stop");
		}
		else {
			MessageBoxW(hwnd, L"No file selected", NULL, MB_OK | MB_ICONERROR);
		}
	});

	DragAcceptFiles(hwnd, TRUE);
}

void app::Replay::Worker() {
	wstring file = editFile.text();
	w32oop::util::RAIIHelper r([this] {
		isRunning = false;
		btnChooseFile.enable();
		editFile.enable();
		btnStartReplay.enable();
		btnStartReplay.text(L"Start");
	});

	btnStartReplay.disable(); // 在加载完成之前禁用停止按钮
	try {
		// 打开文件
		w32FileHandle hFile = CreateFileW(
			file.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		w32FileHandle hSerial = OpenComPort(std::stoi(editComPort.text()));
		LARGE_INTEGER fileSize;
		if (!GetFileSizeEx(hFile.get(), &fileSize)) {
			throw exception();
		}
		const LONGLONG MAX_FILE_SIZE = 1LL * 1024 * 1024 * 1024; // 1GB
		if (fileSize.QuadPart > MAX_FILE_SIZE) {
			SetLastError(ERROR_FILE_TOO_LARGE);
			throw exception();
		}
		vector<RecordItem> records;
		// 基于文件大小预留空间
		size_t estimatedCount = fileSize.QuadPart / sizeof(RecordItem);
		records.reserve(estimatedCount > 1000000 ? 1000000 : estimatedCount);
		// 加载文件
		{
			const DWORD BUFFER_SIZE = 32 * 1024; // 32KiB
			vector<BYTE> buffer(BUFFER_SIZE);
			DWORD bytesRead;
			size_t remainingBuffer = 0;
			BYTE* dataPtr = buffer.data();
			while (isRunning && ReadFile(hFile.get(), buffer.data(), BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
				dataPtr = buffer.data();
				remainingBuffer = bytesRead;
				while (remainingBuffer >= sizeof(RecordItem)) {
					RecordItem item;
					memcpy(&item, dataPtr, sizeof(RecordItem));
					records.push_back(item);

					dataPtr += sizeof(RecordItem);
					remainingBuffer -= sizeof(RecordItem);
				}
			}
		}
		// 业务逻辑
		if (records.empty()) return;
		btnStartReplay.enable();

		auto replayStartTime = chrono::steady_clock::now();

		for (const auto& record : records) {
			if (!isRunning) break;

			auto targetDelay = chrono::milliseconds(record.t);
			auto executeTime = replayStartTime + targetDelay;

			// 等待到正确的时间点
			while (chrono::steady_clock::now() < executeTime && isRunning) {
				auto remaining = chrono::duration_cast<chrono::milliseconds>(executeTime - chrono::steady_clock::now());
				if (remaining.count() > 1) {
					Sleep(1);
					if (!isRunning) break;
				}
				else {
					this_thread::yield();
				}
			}

			if (!isRunning) break;

			// 构建并发送键盘数据帧
			auto frame = replay::devices::CH9329::build_kb_frame(
				replay::devices::CH9329::ADDR,
				record.modifier,
				record.keycode
			);

			replay::devices::CH9329::SendToCH9329Device(hSerial, frame);
		}

		// 最后发送全0重置键盘，防止出现意外
		if (isRunning) {
			this_thread::sleep_for(chrono::milliseconds(100));
			uint8_t zeroKeys[6] = { 0, 0, 0, 0, 0, 0 };
			auto resetFrame = replay::devices::CH9329::build_kb_frame(
				replay::devices::CH9329::ADDR,
				0x00,  // 修饰键全0
				zeroKeys // 键码全0
			);
			replay::devices::CH9329::SendToCH9329Device(hSerial, resetFrame);
		}

		if (isRunning)
			TaskDialog(hwnd, NULL, L"Replay Record", L"Replay finished",
				L"Replay has finished.", TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
				TD_INFORMATION_ICON, NULL);
	}
	catch (...) {
		MessageBoxW(hwnd, ErrorChecker().message().c_str(), L"Replay Interrupted", MB_ICONERROR);
	}
}

void app::Replay::onClose(EventData& ev) {
	if (isRunning) {
		ev.preventDefault();
	}
}

void app::Replay::onDestroy() {
	if (isRunning) {
		isRunning = false;
	}
	if (worker.joinable()) {
		worker.detach();
	}
}

void app::Replay::doLayout(EventData& ev) {
	if (!created()) return;

	RECT rc{}; GetClientRect(hwnd, &rc);
	const int margin = 10;
	int clientWidth = rc.right - rc.left;
	int clientHeight = rc.bottom - rc.top;

	// 第一行高度
	int rowHeight = 25;
	int rowSpacing = 5;

	// 第一行：文件相关控件
	int labelWidth = 40;
	int buttonWidth = 120;

	textFile.resize(margin, margin, labelWidth, rowHeight);
	btnChooseFile.resize(clientWidth - buttonWidth, margin, buttonWidth - margin, rowHeight);
	int editWidth = clientWidth - margin - labelWidth - buttonWidth - 10;
	editFile.resize(margin + labelWidth + margin, margin, editWidth - margin, rowHeight);

	// 第二行：串口相关控件
	int comPortY = margin + rowHeight + rowSpacing;
	textComPort.resize(margin, comPortY, 70, rowHeight);
	editComPort.resize(margin + 75, comPortY, 60, rowHeight);

	// 第三行：开始按钮（填满剩余空间）
	int startButtonY = comPortY + rowHeight + rowSpacing;
	int startButtonHeight = clientHeight - startButtonY - margin;
	btnStartReplay.resize(margin, startButtonY, clientWidth - 2 * margin, startButtonHeight);
}

void app::Replay::onFile(EventData& ev) {
	HDROP hDrop = (HDROP)ev.wParam;

	// 获取拖放的文件数量
	UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);

	if (fileCount == 1) {
		// 只拖放了一个文件
		wchar_t filePath[MAX_PATH];
		DragQueryFileW(hDrop, 0, filePath, MAX_PATH);

		// 检查是否是文件（不是文件夹）
		DWORD attr = GetFileAttributesW(filePath);
		if (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
			// 是文件，不是文件夹，设置到编辑框
			editFile.text(filePath);
		}
		else {
			// 是文件夹或无效路径
			MessageBoxW(hwnd, L"Only files are accepted", NULL, MB_OK | MB_ICONERROR);
		}
	}
	else if (fileCount > 1) {
		// 拖放了多个文件
		MessageBoxW(hwnd, L"Only one file is accepted", NULL, MB_OK | MB_ICONERROR);
	}

	// 释放拖放资源
	DragFinish(hDrop);
}
