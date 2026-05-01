#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string_view>

using WinStationTerminateProcessFn = BOOLEAN(WINAPI*)(HANDLE hServer, ULONG ProcessId, ULONG ExitCode);

static void print_system_error(DWORD error_code)
{
    char* message = nullptr;
    const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS;
    const DWORD len = FormatMessageA(
        flags,
        nullptr,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&message),
        0,
        nullptr
    );
    if (len != 0 && message != nullptr)
    {
        std::fprintf(stderr, "Error %lu: %s", error_code, message);
        LocalFree(message);
        return;
    }
    std::fprintf(stderr, "Error %lu: (FormatMessageA failed)\n", error_code);
}

int main(int argc, char* argv[])
{
    // 可选安全策略：限制只加载 System32 中的 DLL
    PROCESS_MITIGATION_IMAGE_LOAD_POLICY policy{};
    policy.PreferSystem32Images = 1;
    policy.NoRemoteImages = 1;
    if (!SetProcessMitigationPolicy(ProcessImageLoadPolicy, &policy, sizeof(policy)))
    {
        // 此策略失败不影响核心功能，仅警告（可选）
        // return static_cast<int>(GetLastError());
    }

    // 参数校验：最少 1 个参数(PID)，最多 2 个参数(PID + ExitCode)
    if (argc < 2 || argc > 3)
    {
        print_system_error(ERROR_INVALID_PARAMETER);
        std::fprintf(stderr, "Usage: %s <PID> [ExitCode]\n", argv[0]);
        std::fprintf(stderr, "  ExitCode : optional, default is 1\n");
        return static_cast<int>(ERROR_INVALID_PARAMETER);
    }

    // 解析 PID
    unsigned long long pid64 = 0;
    const char* pid_begin = argv[1];
    const char* pid_end = pid_begin + std::strlen(pid_begin);
    auto [pid_ptr, pid_ec] = std::from_chars(pid_begin, pid_end, pid64, 10);
    if (pid_ec != std::errc{} || pid_ptr != pid_end || pid64 > 0xFFFFFFFFull)
    {
        print_system_error(ERROR_INVALID_PARAMETER);
        std::fprintf(stderr, "Invalid PID format.\n");
        return static_cast<int>(ERROR_INVALID_PARAMETER);
    }
    const DWORD pid = static_cast<DWORD>(pid64);

    // 解析 ExitCode（可选参数）
    ULONG exit_code = 1;          // 默认值
    if (argc == 3)
    {
        unsigned long long code64 = 0;
        const char* code_begin = argv[2];
        const char* code_end = code_begin + std::strlen(code_begin);
        auto [code_ptr, code_ec] = std::from_chars(code_begin, code_end, code64, 10);
        if (code_ec != std::errc{} || code_ptr != code_end || code64 > 0xFFFFFFFFull)
        {
            print_system_error(ERROR_INVALID_PARAMETER);
            std::fprintf(stderr, "Invalid ExitCode format (must be 0..4294967295).\n");
            return static_cast<int>(ERROR_INVALID_PARAMETER);
        }
        exit_code = static_cast<ULONG>(code64);
    }

    // 加载 winsta.dll
    HMODULE winsta = LoadLibraryW(L"winsta.dll");
    if (!winsta)
    {
        const DWORD err = GetLastError();
        print_system_error(err);
        return static_cast<int>(err);
    }

    auto terminate_process = reinterpret_cast<WinStationTerminateProcessFn>(
        GetProcAddress(winsta, "WinStationTerminateProcess"));
    if (!terminate_process)
    {
        const DWORD err = GetLastError();
        print_system_error(err);
        FreeLibrary(winsta);
        return static_cast<int>(err);
    }

    // 调用终止函数
    const BOOLEAN ok = terminate_process(nullptr, static_cast<ULONG>(pid), exit_code);
    if (!ok)
    {
        const DWORD err = GetLastError();
        print_system_error(err);
        FreeLibrary(winsta);
        return static_cast<int>(err);
    }

    FreeLibrary(winsta);
    std::printf("Process %lu terminated with exit code %lu.\n", pid, exit_code);
    return 0;
}
