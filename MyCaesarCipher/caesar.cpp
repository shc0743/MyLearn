// CaesarCipher - 凯撒加密/解密二进制文件 (Win32 API)
// 用法: <ARGV0> <FileName> [<OutFileName> [password]]
//   password 是一个数字；写成负数即为解密。
//   未指定 OutFileName 时，直接在原文件上原地(in-place)加密/解密。
#include <windows.h>
#include <cstdio>
#include <cwchar>
#include <string>
#include <vector>
#include <algorithm>

static void usage(const wchar_t* argv0)
{
    std::fwprintf(stderr,
        L"Usage: %ls <FileName> [<OutFileName> [password]]\n"
        L"  FileName    : input file (binary mode)\n"
        L"  OutFileName : output file; if omitted, FileName is modified IN PLACE\n"
        L"  password    : an integer Caesar shift (default: 3);\n"
        L"                use a NEGATIVE password to decrypt.\n",
        argv0 ? argv0 : L"caesar");
}

static bool read_all(HANDLE hFile, std::vector<unsigned char>& data)
{
    LARGE_INTEGER size;
    if (!::GetFileSizeEx(hFile, &size) || size.QuadPart < 0)
        return false;

    data.resize(static_cast<size_t>(size.QuadPart));
    size_t off = 0;
    while (off < data.size()) {
        DWORD chunk = static_cast<DWORD>((std::min)(data.size() - off, static_cast<size_t>(MAXDWORD)));
        DWORD br = 0;
        if (!::ReadFile(hFile, data.data() + off, chunk, &br, nullptr) || br == 0)
            return false;
        off += br;
    }
    return true;
}

static bool write_all(HANDLE hFile, const unsigned char* p, size_t len)
{
    size_t off = 0;
    while (off < len) {
        DWORD chunk = static_cast<DWORD>((std::min)(len - off, static_cast<size_t>(MAXDWORD)));
        DWORD bw = 0;
        if (!::WriteFile(hFile, p + off, chunk, &bw, nullptr) || bw == 0)
            return false;
        off += bw;
    }
    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2 || argc > 4) {
        usage(argv[0]);
        return 1;
    }

    const wchar_t* inName = argv[1];
    long shift = 3; // 经典凯撒默认位移

    if (argc >= 4) {
        wchar_t* end = nullptr;
        shift = std::wcstol(argv[3], &end, 10);
        if (end == argv[3] || *end != L'\0') {
            std::fwprintf(stderr, L"Error: password must be an integer, got \"%ls\".\n", argv[3]);
            return 1;
        }
    }

    // 把位移归一化到 [0,255]，负数（解密）自动取模处理
    int key = static_cast<int>(shift % 256);
    if (key < 0)
        key += 256;

    // 1) 读取整个输入文件
    HANDLE hIn = ::CreateFileW(inName, GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hIn == INVALID_HANDLE_VALUE) {
        std::fwprintf(stderr, L"Error: cannot open input file (error %lu)\n", ::GetLastError());
        return 1;
    }
    std::vector<unsigned char> data;
    if (!read_all(hIn, data)) {
        std::fwprintf(stderr, L"Error: read failed (error %lu)\n", ::GetLastError());
        ::CloseHandle(hIn);
        return 1;
    }
    ::CloseHandle(hIn);

    // 2) 凯撒变换
    for (size_t i = 0; i < data.size(); ++i)
        data[i] = static_cast<unsigned char>(data[i] + key);

    // 3) 写出：指定了 OutFileName 则写新文件，否则原地写回
    HANDLE hOut;
    if (argc >= 3) {
        hOut = ::CreateFileW(argv[2], GENERIC_WRITE, 0, nullptr,
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    } else {
        hOut = ::CreateFileW(inName, GENERIC_WRITE, 0, nullptr,
                             TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    }
    if (hOut == INVALID_HANDLE_VALUE) {
        std::fwprintf(stderr, L"Error: cannot create output file (error %lu)\n", ::GetLastError());
        return 1;
    }

    BOOL ok = write_all(hOut, data.data(), data.size());
    if (!ok)
        std::fwprintf(stderr, L"Error: write failed (error %lu)\n", ::GetLastError());

    ::CloseHandle(hOut);
    return ok ? 0 : 1;
}
