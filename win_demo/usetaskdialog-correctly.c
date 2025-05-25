#include<windows.h>
#include<commctrl.h>
#pragma comment(lib, "comctl32.lib")
// See: https://learn.microsoft.com/zh-cn/windows/win32/controls/cookbook-overview
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
int main() {
	TaskDialog(0, 0, L"Hello", L"World", L"Content", TDCBF_CANCEL_BUTTON, TD_INFORMATION_ICON, NULL);
	return 0;
}