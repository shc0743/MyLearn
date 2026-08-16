#include <Windows.h>
#include <string>
#include <stdexcept>

unsigned long long 转换字符串为没有符号的数字(const std::wstring& 字符串, const int B)
{
	using T = unsigned long long (__stdcall *)(PCWSTR, int);
	static T f = nullptr;
	if (f == nullptr) {
		BYTE b[] = {65,137,208,72,133,201,15,148,192,65,141,80,219,131,250,221,15,146,194,8,194,117,39,102,15,31,132,0,0,0,0,0,72,131,193,2,15,183,65,254,15,183,208,68,141,74,247,65,131,249,5,114,235,131,250,32,116,230,133,210,117,7,69,49,201,76,137,200,195,69,137,194,69,49,201,15,31,68,0,0,15,183,208,68,141,88,208,184,208,255,255,255,102,65,131,251,10,114,32,68,141,90,159,184,169,255,255,255,102,65,131,251,26,114,16,68,141,90,191,184,201,255,255,255,102,65,131,251,25,119,190,1,208,68,57,192,115,183,65,137,195,76,137,216,72,247,208,49,210,73,247,242,73,57,193,119,21,77,15,175,202,77,1,217,15,183,1,72,131,193,2,102,133,192,117,160,235,143,72,199,192,255,255,255,255,195};
		const auto p = VirtualAlloc(nullptr, sizeof b, MEM_COMMIT, PAGE_READWRITE);
		if (!p) throw std::runtime_error("cannot convert");
		const auto d = static_cast<BYTE *>(p);
		
		SIZE_T tmp1{}; DWORD tmp{};
		if (!(
			WriteProcessMemory(GetCurrentProcess(), p, b, sizeof b, &tmp1) &&
			tmp1 == sizeof b &&
			VirtualProtect(p, sizeof b, PAGE_EXECUTE_READ, &tmp)
		)) {
			VirtualFree(p, 0, MEM_RELEASE);
			throw std::runtime_error("cannot convert");
		}
		f = reinterpret_cast<T>(p);
	}
	return f(字符串.c_str(), B);
}
