/* =====================================================================
 * fake_console.c
 * ---------------------------------------------------------------------
 * 一个"看起来像控制台"的假控制台。
 *
 * 实现方式：
 *   - 纯 Win32 API + GDI：普通的 WS_OVERLAPPEDWINDOW 顶层窗口，
 *     黑色客户区 + 等宽字体 + 自绘文本缓冲 + 闪烁块状光标；
 *   - 不使用任何控制台 API（AllocConsole / WriteConsole / conhost），
 *     进程也从未连接过控制台；
 *   - 支持：命令回显、滚动回看（滚轮/PageUp/PageDown）、命令历史
 *     （上下方向键）、Ctrl+C 取消、右键/Ctrl+V 粘贴、左键定位光标、
 *     dir/cls/color/cd/echo/ver/date/time/whoami/help/ping/title/exit
 *     等伪命令（dir 列出真实目录，cd 真的会切换目录）。
 * ===================================================================== */

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <winternl.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define ID_TIMER_BLINK 1
#define MAX_INPUT 4096
#define MAX_HIST  64
#define MAX_LINE  8192

#ifndef DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((HANDLE)-4)
#endif

/* ---------------- 全局状态 ---------------- */

static HINSTANCE g_hInst;
static HWND      g_hwnd = NULL;
static HFONT     g_font = NULL;
static int       cellW = 8, cellH = 16, cols = 80, rows = 25;
static COLORREF  g_fg = RGB(0xC0, 0xC0, 0xC0);   /* 经典控制台银灰色 */
static COLORREF  g_bg = RGB(0, 0, 0);
static wchar_t   g_prompt[MAX_LINE];
static wchar_t   g_title[256] = L"命令提示符";

/* 文本缓冲：完成的行 + 命令历史 + 正在输入的行 */
typedef struct {
    wchar_t **lines;
    int n, cap;
} StrVec;

static StrVec g_lines;                 /* 所有输出行 */
static StrVec g_hist;                  /* 命令历史 */
static wchar_t g_input[MAX_INPUT];     /* 当前输入 */
static int     g_cur = 0;              /* 光标在输入行中的位置 */
static int     g_histPos = -1;         /* -1 = 正在编辑新输入 */
static wchar_t g_draft[MAX_INPUT];     /* 临时保存未完成输入 */
static int     g_scroll = 0;           /* 回看的物理行数（0 = 底部） */
static BOOL    g_blink = TRUE;
static BOOL    g_focused = FALSE;

/* ---------------- 小工具函数 ---------------- */

static int imax(int a, int b) { return a > b ? a : b; }

static void swpf(wchar_t *dst, size_t cap, const wchar_t *fmt, ...) {
    if (cap == 0) return;
    va_list ap;
    va_start(ap, fmt);
    _vsnwprintf(dst, cap - 1, fmt, ap);
    va_end(ap);
    dst[cap - 1] = 0;
}

static wchar_t *myDup(const wchar_t *s) {
    size_t n = wcslen(s) + 1;
    wchar_t *p = (wchar_t *)malloc(n * sizeof(wchar_t));
    if (p) memcpy(p, s, n * sizeof(wchar_t));
    return p;
}

static void vecPush(StrVec *v, const wchar_t *s) {
    if (v->n >= v->cap) {
        v->cap = v->cap ? v->cap * 2 : 64;
        v->lines = (wchar_t **)realloc(v->lines, v->cap * sizeof(wchar_t *));
    }
    v->lines[v->n++] = myDup(s);
}

static void vecClear(StrVec *v) {
    for (int i = 0; i < v->n; i++) free(v->lines[i]);
    v->n = 0;
}

static void vecFree(StrVec *v) {
    vecClear(v);
    free(v->lines);
    v->lines = NULL; v->cap = 0;
}

static void addLine(const wchar_t *s) {
    vecPush(&g_lines, s);
    g_scroll = 0;
    if (g_hwnd) InvalidateRect(g_hwnd, NULL, FALSE);
}

static void addLinef(const wchar_t *fmt, ...) {
    wchar_t buf[MAX_LINE];
    va_list ap;
    va_start(ap, fmt);
    _vsnwprintf(buf, MAX_LINE - 1, fmt, ap);
    va_end(ap);
    buf[MAX_LINE - 1] = 0;
    addLine(buf);
}

static void toLowerW(wchar_t *s) {
    for (; *s; s++) if (*s >= L'A' && *s <= L'Z') *s += 32;
}

static int hexVal(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    return -1;
}

static COLORREF paletteColor(int i) {
    static const COLORREF pal[16] = {
        RGB(0,0,0),      RGB(0,0,128),    RGB(0,128,0),    RGB(0,128,128),
        RGB(128,0,0),    RGB(128,0,128),  RGB(128,128,0),  RGB(192,192,192),
        RGB(128,128,128),RGB(0,0,255),    RGB(0,255,0),    RGB(0,255,255),
        RGB(255,0,0),    RGB(255,0,255),  RGB(255,255,0),  RGB(255,255,255)
    };
    return pal[i & 15];
}

static void refreshPrompt(void) {
    wchar_t cwd[MAX_PATH];
    if (GetCurrentDirectoryW(MAX_PATH, cwd))
        swpf(g_prompt, MAX_LINE, L"%s>", cwd);
    else
        swpf(g_prompt, MAX_LINE, L"C:\\>");
}

/* ---------------- 文本缓冲几何 ---------------- */

/* 一行文本在 cols 列下占多少个物理行 */
static int physRows(const wchar_t *s, int len) {
    return len ? (len + cols - 1) / cols : 1;
}

/* 缓冲总物理行数（历史 + 输入行） */
static int totalPhys(void) {
    int t = 0;
    for (int i = 0; i < g_lines.n; i++)
        t += physRows(g_lines.lines[i], (int)wcslen(g_lines.lines[i]));
    wchar_t inp[MAX_LINE + MAX_INPUT];
    swpf(inp, MAX_LINE + MAX_INPUT, L"%s%s", g_prompt, g_input);
    t += physRows(inp, (int)wcslen(inp));
    return t;
}

/* ---------------- 输入编辑 ---------------- */

static void insertChar(wchar_t c) {
    if (c < 0x20) return;
    if ((int)wcslen(g_input) >= MAX_INPUT - 1) return;
    memmove(g_input + g_cur + 1, g_input + g_cur,
            (wcslen(g_input) - g_cur + 1) * sizeof(wchar_t));
    g_input[g_cur] = c;
    g_cur++;
    g_scroll = 0;
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static void backspace(void) {
    if (g_cur <= 0) return;
    memmove(g_input + g_cur - 1, g_input + g_cur,
            (wcslen(g_input) - g_cur + 1) * sizeof(wchar_t));
    g_cur--;
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static void deleteChar(void) {
    if (g_cur >= (int)wcslen(g_input)) return;
    memmove(g_input + g_cur, g_input + g_cur + 1,
            (wcslen(g_input) - g_cur) * sizeof(wchar_t));
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static void doPaste(void) {
    if (!OpenClipboard(g_hwnd)) return;
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (h) {
        const wchar_t *s = (const wchar_t *)GlobalLock(h);
        if (s) {
            for (int i = 0; s[i]; i++)
                if (s[i] >= 0x20) insertChar(s[i]);
            GlobalUnlock(h);
        }
    }
    CloseClipboard();
}

static void cancelInput(void) {
    if (g_input[0]) {
        addLine(L"^C");
        g_input[0] = 0;
        g_cur = 0;
    }
}

static void histAdd(const wchar_t *s) {
    if (!s[0]) return;
    if (g_hist.n && !wcscmp(g_hist.lines[g_hist.n - 1], s)) return;
    if (g_hist.n >= MAX_HIST) {
        free(g_hist.lines[0]);
        memmove(g_hist.lines, g_hist.lines + 1,
                (g_hist.n - 1) * sizeof(wchar_t *));
        g_hist.n--;
    }
    vecPush(&g_hist, s);
}

static void histUp(void) {
    if (g_hist.n == 0) return;
    if (g_histPos == -1) {
        swpf(g_draft, MAX_INPUT, L"%s", g_input);
        g_histPos = g_hist.n - 1;
    } else if (g_histPos > 0) {
        g_histPos--;
    }
    swpf(g_input, MAX_INPUT, L"%s", g_hist.lines[g_histPos]);
    g_cur = (int)wcslen(g_input);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

static void histDown(void) {
    if (g_histPos == -1) return;
    g_histPos++;
    if (g_histPos >= g_hist.n) {
        g_histPos = -1;
        swpf(g_input, MAX_INPUT, L"%s", g_draft);
    } else {
        swpf(g_input, MAX_INPUT, L"%s", g_hist.lines[g_histPos]);
    }
    g_cur = (int)wcslen(g_input);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

/* ---------------- 伪命令 ---------------- */

static void cmd_dir(void) {
    wchar_t path[MAX_PATH];
    if (!GetCurrentDirectoryW(MAX_PATH, path)) { addLine(L"系统找不到指定的路径。"); return; }

    wchar_t root[4] = { path[0], L':', L'\\', 0 };
    wchar_t label[256] = {0}, fs[64] = {0};
    DWORD serial = 0, mfgl = 0, flags = 0;
    if (GetVolumeInformationW(root, label, 256, &serial, &mfgl, &flags, fs, 64)) {
        if (label[0])
            addLinef(L" 驱动器 %c 中的卷是 %s", path[0], label);
        else
            addLinef(L" 驱动器 %c 中的卷没有标签。", path[0]);
        addLinef(L" 卷的序列号是 %04X-%04X",
                 (unsigned)((serial >> 16) & 0xFFFF), (unsigned)(serial & 0xFFFF));
        addLine(L"");
    }
    addLinef(L" %s 的目录", path);
    addLine(L"");

    wchar_t pat[MAX_PATH + 4];
    swpf(pat, MAX_PATH + 4, L"%s\\*", path);
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) { addLine(L"找不到文件。"); return; }

    int nf = 0, nd = 0;
    ULARGE_INTEGER total; total.QuadPart = 0;
    do {
        SYSTEMTIME st; FILETIME ftl;
        FileTimeToLocalFileTime(&fd.ftLastWriteTime, &ftl);
        FileTimeToSystemTime(&ftl, &st);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            addLinef(L"%04d/%02d/%02d  %02d:%02d    <DIR>          %s",
                     st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, fd.cFileName);
            nd++;
        } else {
            ULARGE_INTEGER sz;
            sz.LowPart = fd.nFileSizeLow;
            sz.HighPart = fd.nFileSizeHigh;
            total.QuadPart += sz.QuadPart;
            addLinef(L"%04d/%02d/%02d  %02d:%02d    %13I64u %s",
                     st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
                     sz.QuadPart, fd.cFileName);
            nf++;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);

    addLine(L"");
    addLinef(L"               %d 个文件  %I64u 字节", nf, total.QuadPart);
    ULARGE_INTEGER freeAvail;
    if (GetDiskFreeSpaceExW(root, &freeAvail, NULL, NULL))
        addLinef(L"               %d 个目录  %I64u 可用字节", nd, freeAvail.QuadPart);
    else
        addLinef(L"               %d 个目录", nd);
}

static void cmd_echo(const wchar_t *rest) {
    if (!rest[0]) { addLine(L"ECHO 处于打开状态。"); return; }
    wchar_t out[MAX_LINE];
    int oi = 0;
    for (int i = 0; rest[i] && oi < MAX_LINE - 1; i++) {
        if (rest[i] == L'%') {
            int j = i + 1;
            while (rest[j] && rest[j] != L'%') j++;
            if (rest[j] == L'%' && j > i + 1 && (size_t)(j - i - 1) < 128) {
                wchar_t name[128];
                memcpy(name, rest + i + 1, (j - i - 1) * sizeof(wchar_t));
                name[j - i - 1] = 0;
                wchar_t val[MAX_LINE] = {0};
                int found = 0;
                if (!_wcsicmp(name, L"CD")) {
                    if (GetCurrentDirectoryW(MAX_LINE, val)) found = 1;
                } else if (GetEnvironmentVariableW(name, val, MAX_LINE)) {
                    found = 1;
                }
                if (found && val[0]) {
                    int room = MAX_LINE - 1 - oi;
                    int cp = (int)wcslen(val) < room ? (int)wcslen(val) : room;
                    memcpy(out + oi, val, cp * sizeof(wchar_t));
                    oi += cp;
                } else {
                    /* 未定义变量：原样回显 %name% */
                    int nl = (int)wcslen(name);
                    for (int k = 0; k < nl && oi < MAX_LINE - 2; k++) out[oi++] = name[k];
                }
                i = j;
                continue;
            }
        }
        out[oi++] = rest[i];
    }
    out[oi] = 0;
    addLine(out);
}

static void cmd_color(const wchar_t *rest) {
    if (wcslen(rest) != 2) { addLine(L"无效参数。"); return; }
    int hi = hexVal(rest[0]), lo = hexVal(rest[1]);
    if (hi < 0 || lo < 0 || hi > 7) { addLine(L"无效参数。"); return; }
    g_fg = paletteColor(lo);
    g_bg = paletteColor(hi);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

typedef LONG(WINAPI *RtlGetVersionFn)(PRTL_OSVERSIONINFOW);
static void cmd_ver(void) {
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    RtlGetVersionFn fn = ntdll ? (RtlGetVersionFn)GetProcAddress(ntdll, "RtlGetVersion") : NULL;
    if (fn) {
        RTL_OSVERSIONINFOW vi;
        ZeroMemory(&vi, sizeof(vi));
        vi.dwOSVersionInfoSize = sizeof(vi);
        if (fn(&vi) == 0) {
            addLinef(L"Microsoft Windows [版本 %lu.%lu.%lu]",
                     vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber);
            return;
        }
    }
    addLine(L"Microsoft Windows");
}

static void cmd_date(void) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    static const wchar_t *w = L"日一二三四五六";
    addLinef(L"当前日期: %04d/%02d/%02d 周%c",
             st.wYear, st.wMonth, st.wDay, w[st.wDayOfWeek]);
}

static void cmd_time(void) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    addLinef(L"当前时间: %02d:%02d:%02d.%02d",
             st.wHour, st.wMinute, st.wSecond, st.wMilliseconds / 10);
}

static void cmd_whoami(void) {
    wchar_t user[256] = {0}, comp[256] = {0};
    DWORD n1 = 256, n2 = 256;
    GetUserNameW(user, &n1);
    GetComputerNameW(comp, &n2);
    addLinef(L"%s\\%s", comp, user);
}

static void cmd_help(void) {
    addLine(L"有关某个命令的详细信息，请键入 HELP 命令名");
    addLine(L"CD          显示当前目录的名称或将其更改。");
    addLine(L"CLS         清除屏幕。");
    addLine(L"COLOR       设置默认控制台前景和背景颜色。");
    addLine(L"DATE        显示或设置日期。");
    addLine(L"DIR         显示一个目录中的文件和子目录。");
    addLine(L"ECHO        显示消息，或将命令回显打开或关闭。");
    addLine(L"EXIT        退出 CMD.EXE 程序(命令解释程序)。");
    addLine(L"HELP        提供 Windows 命令的帮助信息。");
    addLine(L"PING        通过发送 ICMP 回显请求来测试与另一台 TCP/IP 计算机的连接。");
    addLine(L"SET         显示、设置或删除环境变量。");
    addLine(L"TIME        显示或设置系统时间。");
    addLine(L"TITLE       设置命令提示窗口的窗口标题。");
    addLine(L"VER         显示 Windows 的版本。");
    addLine(L"WHOAMI      显示当前用户的用户和组信息。");
}

static void cmd_cd(const wchar_t *rest) {
    if (!rest[0]) {
        wchar_t cwd[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, cwd);
        addLine(cwd);
        return;
    }
    const wchar_t *p = rest;
    if ((p[0] == L'/' || p[0] == L'-') && (p[1] == L'd' || p[1] == L'D')) {
        p += 2;
        while (*p == L' ') p++;
    }
    wchar_t target[MAX_PATH];
    swpf(target, MAX_PATH, L"%s", p);
    if (SetCurrentDirectoryW(target)) { refreshPrompt(); return; }
    if (wcslen(target) >= 2 && target[1] == L':' && target[2] == 0) {
        wchar_t t2[MAX_PATH];
        swpf(t2, MAX_PATH, L"%s\\", target);
        if (SetCurrentDirectoryW(t2)) { refreshPrompt(); return; }
    }
    addLine(L"系统找不到指定的路径。");
}

static void cmd_set(void) {
    static const wchar_t *names[] = {
        L"USERNAME", L"COMPUTERNAME", L"USERPROFILE",
        L"TEMP", L"OS", L"PROCESSOR_ARCHITECTURE"
    };
    for (int i = 0; i < 6; i++) {
        wchar_t v[4096] = {0};
        if (GetEnvironmentVariableW(names[i], v, 4096))
            addLinef(L"%s=%s", names[i], v);
    }
}

static void cmd_ping(const wchar_t *rest) {
    (void)rest;
    addLine(L"正在 Ping 127.0.0.1 具有 32 字节的数据:");
    addLine(L"来自 127.0.0.1 的回复: 字节=32 时间<1ms TTL=128");
    addLine(L"来自 127.0.0.1 的回复: 字节=32 时间<1ms TTL=128");
    addLine(L"来自 127.0.0.1 的回复: 字节=32 时间<1ms TTL=128");
    addLine(L"来自 127.0.0.1 的回复: 字节=32 时间<1ms TTL=128");
    addLine(L"");
    addLine(L"127.0.0.1 的 Ping 统计信息:");
    addLine(L"    数据包: 已发送 = 4，已接收 = 4，丢失 = 0 (0% 丢失)，");
    addLine(L"往返行程的估计时间(以毫秒为单位):");
    addLine(L"    最短 = 0ms，最长 = 0ms，平均 = 0ms");
}

static void cmd_title(const wchar_t *rest) {
    if (!rest[0]) return;
    swpf(g_title, 256, L"%s", rest);
    SetWindowTextW(g_hwnd, g_title);
}

static void cmd_exit(void) {
    PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
}
static void execCommand(const wchar_t *cmdline) {
    const wchar_t *p = cmdline;
    while (*p == L' ') p++;
    const wchar_t *sp = p;
    while (*sp && *sp != L' ') sp++;
    size_t nl = sp - p;
    wchar_t cmd[64];
    if (nl >= 64) nl = 63;
    memcpy(cmd, p, nl * sizeof(wchar_t));
    cmd[nl] = 0;
    toLowerW(cmd);
    const wchar_t *rest = sp;
    while (*rest == L' ') rest++;
    if (!nl) return;

    if      (!wcscmp(cmd, L"dir"))   cmd_dir();
    else if (!wcscmp(cmd, L"cls"))   { vecClear(&g_lines); g_scroll = 0; InvalidateRect(g_hwnd, NULL, FALSE); }
    else if (!wcscmp(cmd, L"echo"))  cmd_echo(rest);
    else if (!wcscmp(cmd, L"color")) cmd_color(rest);
    else if (!wcscmp(cmd, L"ver"))   cmd_ver();
    else if (!wcscmp(cmd, L"date"))  cmd_date();
    else if (!wcscmp(cmd, L"time"))  cmd_time();
    else if (!wcscmp(cmd, L"whoami"))cmd_whoami();
    else if (!wcscmp(cmd, L"help"))  cmd_help();
    else if (!wcscmp(cmd, L"cd"))    cmd_cd(rest);
    else if (!wcscmp(cmd, L"set"))   cmd_set();
    else if (!wcscmp(cmd, L"ping"))  cmd_ping(rest);
    else if (!wcscmp(cmd, L"title")) cmd_title(rest);
    else if (!wcscmp(cmd, L"exit"))  cmd_exit();
    else addLine(L"不是内部或外部命令，也不是可运行的程序或批处理文件。");
}

static void doEnter(void) {
    addLinef(L"%s%s", g_prompt, g_input);
    histAdd(g_input);
    execCommand(g_input);
    g_input[0] = 0;
    g_cur = 0;
    g_histPos = -1;
    g_scroll = 0;
    InvalidateRect(g_hwnd, NULL, FALSE);
}

/* ---------------- 绘制 ---------------- */

static void layout(void) {
    RECT rc;
    GetClientRect(g_hwnd, &rc);
    cols = imax(1, rc.right / cellW);
    rows = imax(1, rc.bottom / cellH);
}

static void createFontFromDpi(int dpi) {
    static const wchar_t *names[] = { L"Consolas", L"Lucida Console", L"Courier New", L"Terminal" };
    int px = MulDiv(16, dpi, 96);
    HDC dc = GetDC(g_hwnd);
    for (int i = 0; i < 4; i++) {
        HFONT f = CreateFontW(-px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, names[i]);
        if (!f) continue;
        TEXTMETRICW tm;
        HGDIOBJ old = SelectObject(dc, f);
        if (GetTextMetricsW(dc, &tm) && tm.tmAveCharWidth >= 6 && tm.tmHeight >= 10) {
            cellW = tm.tmAveCharWidth;
            cellH = tm.tmHeight;
            HFONT oldFont = g_font;
            g_font = f;
            SelectObject(dc, old);
            if (oldFont) DeleteObject(oldFont);
            ReleaseDC(g_hwnd, dc);
            return;
        }
        SelectObject(dc, old);
        DeleteObject(f);
    }
    ReleaseDC(g_hwnd, dc);
}

static int currentDpi(void) {
    HDC dc = GetDC(g_hwnd);
    int dpi = GetDeviceCaps(dc, LOGPIXELSY);
    ReleaseDC(g_hwnd, dc);
    return dpi ? dpi : 96;
}

static void onPaint(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC dc = BeginPaint(hwnd, &ps);
    RECT rc;
    GetClientRect(hwnd, &rc);
    if (rc.right <= 0 || rc.bottom <= 0) { EndPaint(hwnd, &ps); return; }

    /* 双缓冲，避免闪烁 */
    HDC mem = CreateCompatibleDC(dc);
    HBITMAP bmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
    HGDIOBJ ob = SelectObject(mem, bmp);

    HBRUSH bb = CreateSolidBrush(g_bg);
    FillRect(mem, &rc, bb);
    DeleteObject(bb);

    SelectObject(mem, g_font);
    SetBkMode(mem, TRANSPARENT);
    SetTextColor(mem, g_fg);

    int P = totalPhys();
    int maxScroll = P > rows ? P - rows : 0;
    if (g_scroll > maxScroll) g_scroll = maxScroll;
    if (g_scroll < 0) g_scroll = 0;
    int start = P - rows - g_scroll;
    if (start < 0) start = 0;

    /* 输入行（含提示符） */
    wchar_t inp[MAX_LINE + MAX_INPUT];
    swpf(inp, MAX_LINE + MAX_INPUT, L"%s%s", g_prompt, g_input);
    int inLen = (int)wcslen(inp);
    int inPhys = physRows(inp, inLen);
    int inputStart = P - inPhys;

    /* 历史行 */
    int physIdx = 0;
    for (int li = 0; li < g_lines.n; li++) {
        const wchar_t *s = g_lines.lines[li];
        int len = (int)wcslen(s);
        int pr = physRows(s, len);
        for (int k = 0; k < pr; k++) {
            if (physIdx >= start && physIdx < start + rows) {
                int off = k * cols;
                int cl = len - off;
                if (cl > cols) cl = cols;
                TextOutW(mem, 0, (physIdx - start) * cellH, s + off, cl);
            }
            physIdx++;
        }
    }
    /* 输入行 */
    for (int k = 0; k < inPhys; k++) {
        if (physIdx >= start && physIdx < start + rows) {
            int off = k * cols;
            int cl = inLen - off;
            if (cl > cols) cl = cols;
            TextOutW(mem, 0, (physIdx - start) * cellH, inp + off, cl);
        }
        physIdx++;
    }

    /* 块状光标 */
    int prefix = (int)wcslen(g_prompt) + g_cur;
    int crow = inputStart + prefix / cols;
    int ccol = prefix % cols;
    if (crow >= start && crow < start + rows) {
        int x = ccol * cellW;
        int y = (crow - start) * cellH;
        RECT cr = { x, y, x + cellW, y + cellH };
        if (g_focused && g_blink) {
            HBRUSH cb = CreateSolidBrush(g_fg);
            FillRect(mem, &cr, cb);
            DeleteObject(cb);
        } else {
            HPEN pen = CreatePen(PS_SOLID, 1, g_fg);
            HGDIOBJ op = SelectObject(mem, pen);
            HGDIOBJ obr = SelectObject(mem, GetStockObject(NULL_BRUSH));
            Rectangle(mem, cr.left, cr.top, cr.right, cr.bottom);
            SelectObject(mem, op);
            SelectObject(mem, obr);
            DeleteObject(pen);
        }
    }

    BitBlt(dc, 0, 0, rc.right, rc.bottom, mem, 0, 0, SRCCOPY);

    SelectObject(mem, ob);
    DeleteObject(bmp);
    DeleteDC(mem);
    EndPaint(hwnd, &ps);
}

/* ---------------- 窗口图标（GDI 绘制，像控制台图标） ---------------- */

static HICON makeConsoleIcon(int size) {
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;   /* 自上而下 */
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void *bits = NULL;
    HDC sdc = GetDC(NULL);
    HBITMAP cbmp = CreateDIBSection(sdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!cbmp) { ReleaseDC(NULL, sdc); return NULL; }
    HDC mdc = CreateCompatibleDC(sdc);
    HGDIOBJ oc = SelectObject(mdc, cbmp);

    RECT rc = { 0, 0, size, size };
    HBRUSH bb = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(mdc, &rc, bb);
    DeleteObject(bb);

    int inset = size / 8;
    HPEN pen = CreatePen(PS_SOLID, imax(1, size / 32), RGB(0xC0, 0xC0, 0xC0));
    HGDIOBJ op = SelectObject(mdc, pen);
    HGDIOBJ obr = SelectObject(mdc, GetStockObject(NULL_BRUSH));
    Rectangle(mdc, inset, inset, size - inset, size - inset);

    HFONT f = CreateFontW(-imax(7, MulDiv(size, 9, 16)), 0, 0, 0, FW_BOLD,
                          FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                          OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                          CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN, L"Consolas");
    if (f) {
        HGDIOBJ of = SelectObject(mdc, f);
        SetTextColor(mdc, RGB(255, 255, 255));
        SetBkMode(mdc, TRANSPARENT);
        DrawTextW(mdc, L"C:", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(mdc, of);
        DeleteObject(f);
    }
    SelectObject(mdc, oc);
    SelectObject(mdc, op);
    SelectObject(mdc, obr);
    DeleteObject(pen);
    DeleteDC(mdc);

    BITMAPINFO bm;
    ZeroMemory(&bm, sizeof(bm));
    bm.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bm.bmiHeader.biWidth = size;
    bm.bmiHeader.biHeight = -size;
    bm.bmiHeader.biPlanes = 1;
    bm.bmiHeader.biBitCount = 1;
    bm.bmiHeader.biCompression = BI_RGB;
    void *mbits = NULL;
    HBITMAP mbmp = CreateDIBSection(sdc, &bm, DIB_RGB_COLORS, &mbits, NULL, 0);
    if (mbmp && mbits) memset(mbits, 0, ((size * size) + 7) / 8);  /* 全 0 = 不透明 */

    ICONINFO ii;
    ZeroMemory(&ii, sizeof(ii));
    ii.fIcon = TRUE;
    ii.hbmColor = cbmp;
    ii.hbmMask = mbmp ? mbmp : cbmp;
    HICON icon = CreateIconIndirect(&ii);
    if (mbmp) DeleteObject(mbmp);
    DeleteObject(cbmp);
    ReleaseDC(NULL, sdc);
    return icon;
}

/* ---------------- 窗口过程 ---------------- */

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, ID_TIMER_BLINK, 530, NULL);
        cmd_ver();
        addLine(L"(c) Microsoft Corporation。保留所有权利。");
        addLine(L"");
        return 0;

    case WM_TIMER:
        if (wParam == ID_TIMER_BLINK) {
            g_blink = !g_blink;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;

    case WM_SIZE:
        layout();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_DPICHANGED: {
        UINT dpi = HIWORD(wParam);
        createFontFromDpi((int)dpi);
        layout();
        RECT *pr = (RECT *)lParam;
        SetWindowPos(hwnd, NULL, pr->left, pr->top,
                     pr->right - pr->left, pr->bottom - pr->top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;   /* 全部由 WM_PAINT 绘制 */

    case WM_PAINT:
        onPaint(hwnd);
        return 0;

    case WM_SETFOCUS:
        g_focused = TRUE;
        g_blink = TRUE;
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_KILLFOCUS:
        g_focused = FALSE;
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP:     histUp(); break;
        case VK_DOWN:   histDown(); break;
        case VK_LEFT:   if (g_cur > 0) { g_cur--; InvalidateRect(hwnd, NULL, FALSE); } break;
        case VK_RIGHT:  if (g_cur < (int)wcslen(g_input)) { g_cur++; InvalidateRect(hwnd, NULL, FALSE); } break;
        case VK_HOME:   g_cur = 0; InvalidateRect(hwnd, NULL, FALSE); break;
        case VK_END:    g_cur = (int)wcslen(g_input); InvalidateRect(hwnd, NULL, FALSE); break;
        case VK_DELETE: deleteChar(); break;
        case VK_PRIOR: {
            int P = totalPhys(), ms = P > rows ? P - rows : 0;
            g_scroll += rows - 1;
            if (g_scroll > ms) g_scroll = ms;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case VK_NEXT: {
            g_scroll -= rows - 1;
            if (g_scroll < 0) g_scroll = 0;
            InvalidateRect(hwnd, NULL, FALSE);
            break;
        }
        case 'C':
            if (GetKeyState(VK_CONTROL) & 0x8000) cancelInput();
            break;
        case 'V':
            if (GetKeyState(VK_CONTROL) & 0x8000) doPaste();
            break;
        }
        return 0;

    case WM_CHAR:
        switch (wParam) {
        case 0x08: backspace(); break;           /* Backspace */
        case 0x0D: doEnter(); break;             /* Enter */
        case 0x1B:                                /* Esc */
        case 0x1A: break;                         /* Ctrl+Z */
        default:   insertChar((wchar_t)wParam); break;
        }
        return 0;

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        int P = totalPhys(), ms = P > rows ? P - rows : 0;
        g_scroll -= delta / WHEEL_DELTA;
        if (g_scroll < 0) g_scroll = 0;
        if (g_scroll > ms) g_scroll = ms;
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
        int col = x / cellW, row = y / cellH;
        int P = totalPhys();
        int maxScroll = P > rows ? P - rows : 0;
        int start = P - rows - g_scroll;
        if (start < 0) start = 0;
        int physRow = start + row;
        wchar_t inp[MAX_LINE + MAX_INPUT];
        swpf(inp, MAX_LINE + MAX_INPUT, L"%s%s", g_prompt, g_input);
        int inPhys = physRows(inp, (int)wcslen(inp));
        int inputStart = P - inPhys;
        if (physRow >= inputStart && physRow < P) {
            int prefix = (physRow - inputStart) * cols + col - (int)wcslen(g_prompt);
            if (prefix < 0) prefix = 0;
            if (prefix > (int)wcslen(g_input)) prefix = (int)wcslen(g_input);
            g_cur = prefix;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        SetFocus(hwnd);
        return 0;
    }

    case WM_RBUTTONUP:
        doPaste();   /* 经典快速编辑：右键粘贴 */
        return 0;

    case WM_GETMINMAXINFO: {
        MINMAXINFO *mmi = (MINMAXINFO *)lParam;
        mmi->ptMinTrackSize.x = 320;
        mmi->ptMinTrackSize.y = 200;
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER_BLINK);
        vecFree(&g_lines);
        vecFree(&g_hist);
        if (g_font) { DeleteObject(g_font); g_font = NULL; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

/* ---------------- 入口 ---------------- */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow) {
    (void)hPrev; (void)lpCmd; (void)nCmdShow;
    g_hInst = hInstance;

    /* DPI 感知（每监视器 v2） */
    {
        HMODULE u = GetModuleHandleW(L"user32.dll");
        typedef BOOL(WINAPI *SetDpiFn)(HANDLE);
        SetDpiFn fn = u ? (SetDpiFn)GetProcAddress(u, "SetProcessDpiAwarenessContext") : NULL;
        if (fn) fn((HANDLE)DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }

    /* 启动目录设为用户主目录，更像真实的 cmd */
    {
        wchar_t profile[MAX_PATH] = {0};
        if (GetEnvironmentVariableW(L"USERPROFILE", profile, MAX_PATH) && profile[0])
            SetCurrentDirectoryW(profile);
    }

    HICON icon32 = makeConsoleIcon(32);
    HICON icon16 = makeConsoleIcon(16);

    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = icon32 ? icon32 : LoadIconW(NULL, IDI_APPLICATION);
    wc.hIconSm = icon16 ? icon16 : LoadIconW(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursorW(NULL, IDC_IBEAM);
    wc.hbrBackground = NULL;
    wc.lpszClassName = L"FakeConsoleGDI";
    RegisterClassExW(&wc);

    refreshPrompt();

    HWND hwnd = CreateWindowExW(0, L"FakeConsoleGDI", g_title,
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, 980, 660,
                                NULL, NULL, hInstance, NULL);
    if (!hwnd) return 1;
    g_hwnd = hwnd;

    if (icon32) SetClassLongPtrW(hwnd, GCLP_HICON, (LONG_PTR)icon32);
    if (icon16) SetClassLongPtrW(hwnd, GCLP_HICONSM, (LONG_PTR)icon16);

    createFontFromDpi(currentDpi());
    layout();

    /* 深色标题栏，更接近现代 conhost 的外观 */
    {
        HMODULE d = LoadLibraryW(L"dwmapi.dll");
        if (d) {
            typedef HRESULT(WINAPI *DwmFn)(HWND, DWORD, const void *, DWORD);
            DwmFn fn = (DwmFn)GetProcAddress(d, "DwmSetWindowAttribute");
            if (fn) {
                int v = 1;
                fn(hwnd, 20, &v, sizeof(v));   /* Windows 10 2004+ */
                fn(hwnd, 19, &v, sizeof(v));   /* Windows 10 1809  */
            }
            FreeLibrary(d);
        }
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (icon32) DestroyIcon(icon32);
    if (icon16) DestroyIcon(icon16);
    return (int)msg.wParam;
}
