#include<windows.h>
#include<commctrl.h>
#pragma comment(lib, "comctl32.lib")
int main() {
	TaskDialog(0, 0, L"Hello", L"World", L"Content", TDCBF_CANCEL_BUTTON, TD_INFORMATION_ICON, NULL);
	return 0;
}