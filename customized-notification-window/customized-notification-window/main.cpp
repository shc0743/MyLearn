#include "cnw_user.h"

#include "../../resource/tool.h"
#include <unordered_map>
using namespace std;


constexpr LONG nCheckBit = 0xF0ADFF7E;
struct OBJ_DATA
{
	LONG nCheckBit;
	HWND hwnd;
	HANDLE __internal__hCreatorEvent;
	bool closed;


	HFONT static_titlebar_font;
	HWND static_titlebar;
	HFONT static_content_font;
	HWND static_content;
	HFONT button_closebtn_font;
	HWND button_closebtn;
	//bool mousemoving;
	//long msdown_x;
	//long msdown_y;
	size_t close_sleep_id;

	RECT screensize;

	wstring title;
	wstring content;
	wstring contfont;
	DWORD autoclose_time;
};


static std::atomic<ULONGLONG> HCUSTOMIZEDNOTIFICATIONOBJECTIncreaser = 1;
static std::unordered_map<HCUSTOMIZEDNOTIFICATIONOBJECT, OBJ_DATA>* datas;



LRESULT CALLBACK WndProc_34d29f1d205240389d5ef7d600c79311(HWND, UINT, WPARAM, LPARAM);
constexpr PCWSTR CNW_WND_CLASS = L"34d29f1d205240389d5ef7d600c79311";



bool CustomNotifications_Init() {
	datas = new std::unordered_map<HCUSTOMIZEDNOTIFICATIONOBJECT, OBJ_DATA>;

	if (!s7::MyRegisterClassW(CNW_WND_CLASS, WndProc_34d29f1d205240389d5ef7d600c79311))
		return false;



	return true;
}
bool CustomNotifications_Unload() {
	UnregisterClassW(CNW_WND_CLASS, NULL);

	delete datas;
	datas = nullptr;

	return true;
}
HCUSTOMIZEDNOTIFICATIONOBJECT CustomNotifications_Create() {
	if (!datas) { SetLastError(STATUS_ACCESS_VIOLATION); return nullptr; }
	OBJ_DATA data{};
	HANDLE evt = CreateEventW(0, 1, 0, 0);
	data.__internal__hCreatorEvent = evt;
	data.nCheckBit = nCheckBit;
	ULONGLONG newHandle = ++HCUSTOMIZEDNOTIFICATIONOBJECTIncreaser;
	HCUSTOMIZEDNOTIFICATIONOBJECT h = (decltype(h))(LONG_PTR)newHandle;

	data.contfont = L"Consolas";


	datas->insert(std::make_pair(h, data));
	// do not use [[data]] again below this line
	
	HANDLE thrd = CreateThread(0, 0, [](PVOID pData)->DWORD {
		HCUSTOMIZEDNOTIFICATIONOBJECT hdata = (decltype(hdata))pData;

		HWND hwnd = CreateWindowExW(0, CNW_WND_CLASS, L"", WS_OVERLAPPEDWINDOW,
			0, 0, 500, 300, nullptr, nullptr, nullptr, hdata);
		if (!hwnd) {
			return FALSE;
		}
		(datas->at(hdata)).hwnd = hwnd;

		if ((datas->at(hdata)).__internal__hCreatorEvent)
			SetEvent((datas->at(hdata)).__internal__hCreatorEvent);

		// message loop
		MSG msg{};
		while (GetMessage(&msg, 0, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		(datas->at(hdata)).closed = true;

		return 0;
	}, h, 0, 0);
	if (!thrd) {
		datas->erase(h);
		return nullptr;
	}

	if (evt) WaitForSingleObject(evt, 30000);

	return h;
}

#if 0
NTSTATUS CustomNotifications_Template(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	LPCWSTR pszTitle
) {
	if (!datas) { return(STATUS_ACCESS_VIOLATION); }
	struct EINVDAT {};
	try {
		auto& data = datas->at(hObj);
		if (data.nCheckBit != nCheckBit) throw EINVDAT();
		// write app logic here...


	}
	catch (std::out_of_range&) {
		return HTTP_E_STATUS_NOT_FOUND;
	}
	catch (EINVDAT&) {
		return STATUS_INVALID_PARAMETER;
	}
}
#endif

NTSTATUS CustomNotifications_SetTitle(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	LPCWSTR pszTitle
) {
	if (!datas) { return(STATUS_ACCESS_VIOLATION); }
	struct EINVDAT {};
	try {
		auto& data = datas->at(hObj);
		if (data.nCheckBit != nCheckBit) throw EINVDAT();
		// write app logic here...
		SendMessageW( data.hwnd, WM_SETTEXT, 0, (LPARAM)pszTitle );
		return ERROR_SUCCESS;
	}
	catch (std::out_of_range&) {
		return HTTP_E_STATUS_NOT_FOUND;
	}
	catch (EINVDAT&) {
		return STATUS_INVALID_PARAMETER;
	}
}

NTSTATUS CustomNotifications_SetText(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	LPCWSTR pszText
) {
	if (!datas) { return(STATUS_ACCESS_VIOLATION); }
	struct EINVDAT {};
	try {
		auto& data = datas->at(hObj);
		if (data.nCheckBit != nCheckBit) throw EINVDAT();
		// write app logic here...

		SendMessageW( data.hwnd, WM_USER + WM_SETTEXT, 0, (LPARAM)pszText );
		return ERROR_SUCCESS;

	}
	catch (std::out_of_range&) {
		return HTTP_E_STATUS_NOT_FOUND;
	}
	catch (EINVDAT&) {
		return STATUS_INVALID_PARAMETER;
	}
}

NTSTATUS CustomNotifications_SetShownState(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	int nShow
) {
	if (!datas) { return(STATUS_ACCESS_VIOLATION); }
	struct EINVDAT {};
	try {
		auto& data = datas->at(hObj);
		if (data.nCheckBit != nCheckBit) throw EINVDAT();
		// write app logic here...

		ShowWindow( data.hwnd, nShow );
		return ERROR_SUCCESS;

	}
	catch (std::out_of_range&) {
		return HTTP_E_STATUS_NOT_FOUND;
	}
	catch (EINVDAT&) {
		return STATUS_INVALID_PARAMETER;
	}
}

NTSTATUS CustomNotifications_Destroy(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj
) {
	if (!datas) { return(STATUS_ACCESS_VIOLATION); }
	struct EINVDAT {};
	try {
		auto& data = datas->at(hObj);
		if (data.nCheckBit != nCheckBit) throw EINVDAT();
		// write app logic here...

		SendMessageW( data.hwnd, WM_CLOSE, 0, 0 );
		return ERROR_SUCCESS;

	}
	catch (std::out_of_range&) {
		return HTTP_E_STATUS_NOT_FOUND;
	}
	catch (EINVDAT&) {
		return STATUS_INVALID_PARAMETER;
	}
}

[[nodiscard]] HANDLE CustomNotifications_DelayDestroy(
	HCUSTOMIZEDNOTIFICATIONOBJECT hObj,
	DWORD dwMillseconds
) {
	if (!datas) { SetLastError(STATUS_ACCESS_VIOLATION); return 0; }
	struct EINVDAT {};
	try {
		auto& data = datas->at(hObj);
		if (data.nCheckBit != nCheckBit) throw EINVDAT();
		// write app logic here...
		data.autoclose_time = dwMillseconds;
		HANDLE hThread = CreateThread(0, 0, [](PVOID pData)->DWORD {
			HCUSTOMIZEDNOTIFICATIONOBJECT hdata = (decltype(hdata))pData;
			auto tt = (datas->at(hdata).autoclose_time);
			if (tt) Sleep(tt);
			SendMessageW(datas->at(hdata).hwnd, WM_CLOSE, 0, 0);
			return 0;
		}, hObj, 0, 0);

		return hThread;

	}
	catch (...) {
		return NULL;
	}
}


#pragma region WndProc
LRESULT CALLBACK WndProc_34d29f1d205240389d5ef7d600c79311
(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto data34d2 = (HCUSTOMIZEDNOTIFICATIONOBJECT)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	OBJ_DATA* data = nullptr;
	try {
		data = &(datas->at(data34d2));
	}
	catch (...) {}
	switch (message)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCTW pcr = (LPCREATESTRUCTW)lParam;
		if (!pcr) break;
		auto dat34d2 = (HCUSTOMIZEDNOTIFICATIONOBJECT)pcr->lpCreateParams;
		OBJ_DATA* dat = nullptr;
		try {
			dat = &(datas->at(dat34d2));
		}
		catch (...) {}
		if (!dat) break;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(void*)dat34d2);
		data = dat;

		//LONG Style = 0;
		//Style = GetWindowLong(hwnd, GWL_STYLE);	 // 得到窗口风格   
		//Style &= ~WS_CAPTION;			 //去掉标题栏
		//SetWindowLong(hwnd, GWL_STYLE, Style);	 //为窗口设置新的风格
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION);
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZE);
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZE);
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MINIMIZEBOX);
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
		SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_THICKFRAME);
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, 
					 SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);
		WindowShowOnTaskBar(hwnd, false);

		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		{
			RECT rectDesktop;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktop, 0); 
				// 获得屏幕工作区大小 left:0,top:0,right:width,bottom:height
			RECT rectDlg;
			GetWindowRect(hwnd, &rectDlg); // 获取当前窗口工作区大小
			int dlgWidth = rectDlg.right - rectDlg.left;
			int dlgHeight = rectDlg.bottom - rectDlg.top;
			//dlgWidth  -= 10;
			//dlgHeight -= 40;
			SetWindowPos(hwnd,NULL,
				rectDesktop.right  - dlgWidth  - 20,
				rectDesktop.bottom - dlgHeight - 20,
				dlgWidth, dlgHeight, SWP_NOZORDER | SWP_NOSIZE);
		}

		RECT rt;
		GetClientRect(hwnd, &rt);

		//创建逻辑字体
		data->static_titlebar_font = CreateFontW(
			-30/*高度*/, -15/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			L"Consolas"  //字体名
		);
		//创建静态文本框控件
		data->static_titlebar = CreateWindowExW(0,
			L"Static",  //静态文本框的类名
			data->title.c_str(),  //控件的文本
			WS_CHILD /*子窗口*/ | WS_VISIBLE /*创建时显示*/ | WS_BORDER /*带边框*/,
			0 /*X坐标*/, 0/*Y坐标*/, rt.right-rt.left/*宽度*/, 42/*高度*/, hwnd,
			(HMENU)1001,  //为控件指定一个唯一标识符
			NULL,  //当前实例句柄
			NULL
		);
		//设置控件字体
		SendMessage(
			data->static_titlebar,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)data->static_titlebar_font,  //字体句柄
			NULL  //传空值即可
		);

		//创建逻辑字体
		data->static_content_font = CreateFontW(
			-20/*高度*/, -10/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			data->contfont.c_str()  //字体名
		);
		//创建静态文本框控件
		data->static_content = CreateWindowExW(0,
			L"edit",  //静态文本框的类名
			data->content.c_str(),  //控件的文本
			WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_BORDER | ES_WANTRETURN |
			WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0 /*X坐标*/, 50/*Y坐标*/, rt.right-rt.left/*宽度*/, 250/*高度*/, hwnd,
			(HMENU)1003,  //为控件指定一个唯一标识符
			NULL,  //当前实例句柄
			NULL
		);
		//设置控件字体
		SendMessage(
			data->static_content,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)data->static_content_font,  //字体句柄
			NULL  //传空值即可
		);

		//创建逻辑字体
		data->button_closebtn_font = CreateFontW(
			-24/*高度*/, -12/*宽度*/, 0/*不用管*/, 0/*不用管*/, 400 /*一般这个值设为400*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			L"Consolas"  //字体名
		);
		//创建控件
		data->button_closebtn = CreateWindowExW(0,
			L"button",  //静态文本框的类名
			L"x",  //控件的文本
			WS_CHILD | WS_VISIBLE/* | WS_BORDER*/,
			rt.right - 35 /*X坐标*/, 5/*Y坐标*/, 30/*宽度*/, 30/*高度*/, hwnd/*父窗口*/,
			(HMENU)1002,  //为控件指定一个唯一标识符
			NULL,  //当前实例句柄
			NULL
		);
		//设置控件字体
		SendMessage(
			data->button_closebtn,  //欲设置字体的控件句柄
			WM_SETFONT,  //消息名（消息类型）
			(WPARAM)data->button_closebtn_font,  //字体句柄
			NULL  //传空值即可
		);

		if (data->autoclose_time)
		{
			data->close_sleep_id = SetTimer(hwnd, 4881, data->autoclose_time,
				[](HWND h, UINT u, UINT_PTR id, DWORD dw)->void {
					KillTimer(h, id);
					SendMessageW(h, WM_CLOSE, 0, 0);
				});
		}

	}
		break;

#if 1
	case WM_NCHITTEST:
		return HTCAPTION;
		break;
#else
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		data->mousemoving = true;
		POINT point;
		::GetCursorPos(&point);
		::ScreenToClient(hwnd, &point);
		data->msdown_x = point.x + 0;
		data->msdown_y = point.y + 0;
		SetCapture(hwnd);
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		data->mousemoving = false;
		ReleaseCapture();
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		if (data->mousemoving) {
			POINT point;
			::GetCursorPos(&point);
			//if (int(point.x - data->msdown_x) < 0) break;
			//if (int(point.y - data->msdown_y) < 0) break;
			::SetWindowPos(
				hwnd,
				(HWND)-1,
				int(point.x - data->msdown_x),
				int(point.y - data->msdown_y),
				0, 0,
				SWP_NOSIZE);
		}
		break;
#endif

	case WM_MOVING:
	case WM_MOVE:
		//RECT rt;
		//GetWindowRect(hwnd, &rt);
		//SetWindowPos(data->static_titlebar, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		//SetWindowPos(data->static_content, NULL, 0, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		//SetWindowPos(data->button_closebtn, NULL,
		//			 465, 5, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		//GetClientRect(hwnd, &rt);
		//
		//UpdateWindow(hwnd);
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;

	case WM_SETTEXT:
		if (lParam && data) {
			PCWSTR str = (PCWSTR)(LONG_PTR)lParam;
			SetWindowTextW(data->static_titlebar, str);
			UpdateWindow(data->static_titlebar);
			
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
		break;

	case (WM_USER + WM_SETTEXT):
		if (lParam && data) {
			PCWSTR str = (PCWSTR)(LONG_PTR)lParam;
			SetWindowTextW(data->static_content, str);
			UpdateWindow(data->static_content);
			
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
		break;

	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId)
			{
			//case IDM_ABOUT:
			//	DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
			//	break;
			//case IDM_EXIT:
			//	DestroyWindow(hwnd);
			//	break;
			case 1002:
				SendMessageW(hwnd, WM_CLOSE, 0, 0);
				break;
			case 1003:

				break;
			default:
				return DefWindowProc(hwnd, message, wParam, lParam);
			}
		}
		break;
	case WM_DESTROY:
		if (data) {
			DeleteObject(data->static_titlebar_font);
			DeleteObject(data->button_closebtn_font);
			DeleteObject(data->static_content_font);
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
#pragma endregion






