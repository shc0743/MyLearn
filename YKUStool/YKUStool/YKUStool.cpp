// YKUStool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

using namespace std;


int main(int argc, char* argv[]) {
	if (argc < 4) {
		cout << "Usage:\n\t"
			"Program COMn Id Operation\n\n"
			"COMn:\t\tCOM id\n"
			"Id:\t\tID to control (YKUS1: 1, YKUS2: 1|2)\n"
			"Operation:\t\n"
			"\t0x0 - Close (no feedback)\n"
			"\t0x1 - Open (no feedback)\n"
			"\t0x2 - Close (feedback)\n"
			"\t0x3 - Open (feedback)\n"
			"\t0x4 - Toggle (feedback)\n"
			"\t0x5 - Query\n"
			"\t0x6 - Dot (feedback)\n"

			;
		return 87;
	}

	// 打开串口
	HANDLE hCom = CreateFileA(argv[1], GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCom == INVALID_HANDLE_VALUE) {
		cerr << "Error opening file: error " << GetLastError() << endl;
		return GetLastError();
	}

	// 初始化串口 
	// https://wangbaiyuan.cn/c-serial-communication-write-reading.html
	// https://blog.csdn.net/wy749929317/article/details/123822353
	{
		if (!SetupComm(hCom, 1024, 1024)) //输入缓冲区和输出缓冲区的大小都是1024
		{
			cerr << "Failed to SetupComm, Error " << GetLastError() << endl;
			CloseHandle(hCom);
			return GetLastError();
		}
		DCB dcb{};
		dcb.DCBlength = sizeof(dcb);
		dcb.BaudRate = 9600; //波特率为9600
		dcb.ByteSize = 8; //每个字节有8位
		dcb.Parity = NOPARITY; //无奇偶校验位
		dcb.StopBits = ONESTOPBIT; //停止位
		if (!SetCommState(hCom, &dcb)) {
			cerr << "Failed to SetCommState, Error " << GetLastError() << endl;
			CloseHandle(hCom);
			return GetLastError();
		}
		COMMTIMEOUTS TimeOuts{}; //设定读超时
		TimeOuts.ReadIntervalTimeout = 1000;
		TimeOuts.ReadTotalTimeoutMultiplier = 500;
		TimeOuts.ReadTotalTimeoutConstant = 5000; //设定写超时
		TimeOuts.WriteTotalTimeoutMultiplier = 500;
		TimeOuts.WriteTotalTimeoutConstant = 2000;
		SetCommTimeouts(hCom, &TimeOuts); //设置超时
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	}

	// 解析参数

	/*
	YKUS 通讯协议
	(hex)
	[0] 0xa0
	[1] 控制哪个继电器
	[2] 0x0关 0x1开 0x2关(反馈) 0x3开(反馈) 0x4切换 0x5查询 0x6点动
	[3] 校验码（和校验，取低8位）
	*/

	unsigned char hex_array[4]{
		(unsigned char)0xa0, 0x0, 0x0, 0x0
	};

	if (argv[2][0] == '1') hex_array[1] = 0x01;
	if (argv[2][0] == '2') hex_array[1] = 0x02;

	if (argv[3][0] == '0') hex_array[2] = 0x00;
	if (argv[3][0] == '1') hex_array[2] = 0x01;
	if (argv[3][0] == '2') hex_array[2] = 0x02;
	if (argv[3][0] == '3') hex_array[2] = 0x03;
	if (argv[3][0] == '4') hex_array[2] = 0x04;
	if (argv[3][0] == '5') hex_array[2] = 0x05;
	if (argv[3][0] == '6') hex_array[2] = 0x06;

	hex_array[3] = (hex_array[0] + hex_array[1] + hex_array[2]) & 0xFF;

	// 发送数据
	printf("Datas to send: %#x %#x %#x %#x\n",
		hex_array[0], hex_array[1], hex_array[2], hex_array[3]);
	DWORD nBytesWritten = 0;
	WriteFile(hCom, hex_array, sizeof(hex_array), &nBytesWritten, NULL);

	// 接收数据
	(void)ReadFile(hCom, hex_array, sizeof(hex_array), &nBytesWritten, NULL);
	// 输出数据
	printf("Received: %#x %#x %#x %#x\n",
		hex_array[0], hex_array[1], hex_array[2], hex_array[3]);

	// 清理
	CloseHandle(hCom);

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
