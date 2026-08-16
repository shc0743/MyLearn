#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;

unsigned long long 转换字符串为没有符号的数字(const std::wstring& 字符串, int B = 10);

int main(int argc, char** argv) {
	DWORD tmp{}; char tmp2[4]{};
	if (argc > 1) {
		if (argv[1] == "/benchmark"s) {
			ULONGLONG n = 1000000;
			if (argc > 2) try { n = std::stoull(argv[2]); }
			catch (...) { cerr << "Invalid input" << endl; return 87; }
			cout << "Running " << n << " times..." << endl;
			time_t s = time(0);
			cout.flush();
			WCHAR 字符串[] = L"123456789";
			for (ULONGLONG i = 0;i < n;++i) {
				转换字符串为没有符号的数字(字符串);
			}
			time_t e = time(0);
			cout << "s=" << s << " t=" << e << endl;
			cerr << "Press Enter to continue...";
			cin.getline(tmp2, 4);
			return 0;
		}
	}
	cout << "[TIP] Use /benchmark [n] to benchmark" << endl;

	WCHAR buffer[256]{};
	cout << "Please input a string and I will convert it to number!!\nYour input: ";
	cout.flush();
	if (!ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), buffer, 256, &tmp, nullptr)) {
		return static_cast<int>(GetLastError());
	}

	try {
		const auto result = 转换字符串为没有符号的数字(buffer);
		cout << "Your result is: " << result << endl;
	}
	catch (std::exception& e) {
		cerr << e.what() << endl;
	}

	cerr << "Press Enter to continue...";
	cin.getline(tmp2, 4);
	return 0;
}