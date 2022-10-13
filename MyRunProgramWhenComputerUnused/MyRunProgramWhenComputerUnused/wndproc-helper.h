#pragma once

#define DefineWndProcDeclare(name) \
static LRESULT CALLBACK name(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)

#define BeginWndProc(name) \
static LRESULT CALLBACK name(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

#define BeginWndProcSwitchBlock(name) \
switch(name) {

#define WndProcSwitchCase(n) case (n):

#define WndProcDefault() DefWindowProc(hwnd,message,wParam,lParam)
#define WndProcSwitchDefault() default:\
	return WndProcDefault();

#define EndWndProcSwitchBlock() }

#define WndProcReturnDefault() return 0;

#define EndWndProc() }


