#include <iostream>
#include <Windows.h>
using namespace std;

unsigned long long 转换字符串为没有符号的数字(const std::wstring& 字符串, int B = 10);

int main() {
	WCHAR buffer[256]{};
	DWORD tmp{}; char tmp2[4]{};
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