#include <Windows.h>

int dick();

void fuck() {
	PROCESS_MITIGATION_CHILD_PROCESS_POLICY f{};
	f.NoChildProcessCreation = 1;
	SetProcessMitigationPolicy(ProcessChildProcessPolicy, &f, sizeof f) || dick();
}

static PVOID gpc(HMODULE h, const char* name) {
	static const HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
	static auto GetProcAddress = reinterpret_cast<decltype(&::GetProcAddress)>(::GetProcAddress(k32, "GetProcAddress"));
	if (!k32 || !GetProcAddress) __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	return GetProcAddress(h, name);
}

DWORD WINAPI dick_code(PVOID) {
	//__fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	RaiseFailFastException(NULL, NULL, FAIL_FAST_GENERATE_EXCEPTION_ADDRESS);
	HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
	if (!k32) __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	PVOID addr = gpc(k32, "CreateThread");
	if (!addr) __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	auto func = reinterpret_cast<decltype(&CreateThread)>(addr);
	HANDLE hThread = func(NULL, 0, nullptr, NULL, 0, NULL);
	if (hThread) CloseHandle(hThread);
	return TerminateProcess(GetCurrentProcess(), GetLastError());
}

int dick() {
	HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
	if (!k32) __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	auto VirtualAlloc = reinterpret_cast<decltype(&::VirtualAlloc)>(gpc(k32, "VirtualAlloc"));
	auto VirtualProtect = reinterpret_cast<decltype(&::VirtualProtect)>(gpc(k32, "VirtualProtect"));
	auto VirtualFree = reinterpret_cast<decltype(&::VirtualFree)>(gpc(k32, "VirtualFree"));
	if (!VirtualAlloc || !VirtualProtect || !VirtualFree) __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	PVOID memory = VirtualAlloc(NULL, 0x1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	RtlCopyMemory(memory, dick_code, 0x1000);
	if (!VirtualProtect(memory, 0x1000, PAGE_EXECUTE_READ, NULL)) __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);

	// fuck up
	HANDLE hThread = CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(memory), NULL, 0, NULL);
	if (!hThread) __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	VirtualFree(memory, 0, MEM_RELEASE);
	return 0;
}

void unfuck() {
	PROCESS_MITIGATION_CHILD_PROCESS_POLICY d{};
	d.NoChildProcessCreation = 0;
	SetProcessMitigationPolicy(ProcessChildProcessPolicy, &d, sizeof d);
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#ifdef AUTOMODE
		fuck();
#endif
		break;
	case DLL_PROCESS_DETACH:
#ifdef AUTOMODE
		unfuck();
		dick();
#endif
		break;
	default:
		break;
	}
	return TRUE;
}

