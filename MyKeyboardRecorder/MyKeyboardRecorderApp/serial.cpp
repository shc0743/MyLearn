#include "serial.hpp"
#include <string>

HANDLE OpenComPort(int comPortNumber, DWORD baudRate) {
	std::wstring portName = L"\\\\.\\COM" + std::to_wstring(comPortNumber);

	HANDLE hCom = CreateFileW(
		portName.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,    // 独占方式
		NULL, // 安全属性
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (hCom == INVALID_HANDLE_VALUE) {
		return INVALID_HANDLE_VALUE;
	}

	// 配置串口参数
	DCB dcb = { 0 };
	dcb.DCBlength = sizeof(DCB);

	if (!GetCommState(hCom, &dcb)) {
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	dcb.BaudRate = baudRate;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;
	dcb.fDtrControl = DTR_CONTROL_ENABLE;

	if (!SetCommState(hCom, &dcb)) {
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	// 设置超时
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = MAXDWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts(hCom, &timeouts);

	return hCom;
}

