// MyUsbDeviceHelper.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "MyUsbDeviceHelper.h"
#include <Windows.h>
#include "../../resource/tool.h"
using namespace std;

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
HWND                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#include <Dbt.h>
#include <initguid.h>
#include <usbiodef.h>
#include <sciter-x-window.hpp>
#include <setupapi.h>  
#include <cfgmgr32.h>  
#include <devguid.h>

//HDEVNOTIFY g_hDevNotify = NULL;
HCMNOTIFICATION hcm;

// 通知回调函数的声明  
__callback DWORD(CALLBACK MyDevCb)(
	_In_ HCMNOTIFICATION       hNotify,
	_In_opt_ PVOID             Context,
	_In_ CM_NOTIFY_ACTION      Action,
	_In_reads_bytes_(EventDataSize) PCM_NOTIFY_EVENT_DATA EventData,
	_In_ DWORD                 EventDataSize
	);
// 注册设备通知  
bool MyRegisterDevNotif(HWND hwnd) {
#if 0
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = { 0 };
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	// 这里我们监控所有USB设备，但你也可以指定特定的GUID  
	// 例如，对于USB存储设备，可以使用GUID_DEVINTERFACE_USB_STORAGE  
	NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

	g_hDevNotify = RegisterDeviceNotification(
		hwnd,
		&NotificationFilter,
		DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES
	);

	if (g_hDevNotify == NULL) {
		return false;
	}
#else
	CM_NOTIFY_FILTER NotificationFilter{
		.cbSize = sizeof(NotificationFilter),
		.Flags = CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES,
		.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE,
	};

	// 注册设备接口到达通知  
	auto cr = CM_Register_Notification(
		&NotificationFilter, NULL, MyDevCb, &hcm
	);
	if (CR_SUCCESS != cr) {
		// 处理注册失败  
		return false;
	}

#endif
	return true;
}void MyUnregisterDevNotif() {
	/*if (g_hDevNotify != NULL) {
		UnregisterDeviceNotification(g_hDevNotify);
		g_hDevNotify = NULL;
	}*/
	CM_Unregister_Notification(hcm);
}


#pragma region sciter

  class NativePromise : public sciter::om::asset<NativePromise>
  {
  public:

	sciter::value resolver;
	sciter::value rejector;

	NativePromise() {}

	sciter::value then(sciter::value resolver, sciter::value rejector) {
	  this->resolver = resolver;
	  this->rejector = rejector;
	  return sciter::value::wrap_asset(this);
	}

	void resolve(sciter::value data) {  if (resolver.is_undefined()) return; resolver.call(data); }
	void reject(sciter::value data) { if (rejector.is_undefined()) return; rejector.call(data);
	}

	SOM_PASSPORT_BEGIN(NativePromise)
	  SOM_FUNCS(
		SOM_FUNC(then),
	  )
	  SOM_PASSPORT_END
  };

// this object is used as a global namespace NativeBackend in JS
// it is set by SciterSetGlobalAsset(new NativeBackend()); 
// and is available globally - in all windows
class NativeBackend : public sciter::om::asset<NativeBackend>
{
public:
  NativeBackend() {}

  // this method starts native thread and calls callbacks methods
  bool startNativeThread(sciter::value doneCb, sciter::value progressCb)
  {
	std::thread([=]() {
	  // simulate long running task
	  for (int n = 0; n < 100; ++n) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		progressCb.call(n);
	  }
	  doneCb.call(100);
	  }).detach();
	  return true;
  }

  // this method starts native thread and calls callbacks methods
  bool startNativeThreadWithObject(sciter::value params)
  {
	std::thread([=]() {
	  sciter::value doneCb = params["done"];
	  sciter::value progressCb = params["progress"];
	  // simulate long running task
	  for (int n = 0; n < 100; ++n) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (!progressCb.is_undefined()) progressCb.call(n);
	  }
	  if (!doneCb.is_undefined()) doneCb.call(100);
	  }).detach();
	  return true;
  }

  // this method returns thennable object and so the function is await'able in JS
  sciter::value nativeAsyncFunction(int milliseconds)
  {
	sciter::om::hasset<NativePromise> promise = new NativePromise();

	std::thread([=]() {
	  std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
	  promise->resolve(42);
	  }).detach();

	  return sciter::value::wrap_asset(promise);
  }

  bool status = false;

  bool getStatus() const { return status; }
  bool setStatus(bool nv) {
	status = nv;
	return true;
  }

  // members of NativeBackend namespace  
  SOM_PASSPORT_BEGIN(NativeBackend)
	SOM_FUNCS(
	  SOM_FUNC(startNativeThread),
	  SOM_FUNC(startNativeThreadWithObject),
	  SOM_FUNC(nativeAsyncFunction)
	)
	SOM_PROPS(
	  SOM_VIRTUAL_PROP(status, getStatus, setStatus)
	)
  SOM_PASSPORT_END
};

#pragma endregion

DWORD WINAPI SCTH(PVOID) {
	throw "deprecared";
	sciter::application::start();

	SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);

	SciterSetGlobalAsset(new NativeBackend());

	sciter::application::run();

	sciter::application::shutdown();
	return 0;
}

static DWORD GetPhysicalDriveFromVolumeName(wstring path)
{
	// 如："\\.\Volume{f0a05445-2d33-11e6-9b0b-806e6f6e6963}"
	HANDLE  hDevice = CreateFileW(path.c_str(),                // drive to open
		GENERIC_READ | GENERIC_WRITE,    // access to the drive
		FILE_SHARE_READ | FILE_SHARE_WRITE,    //share mode
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attribute
	if (hDevice == INVALID_HANDLE_VALUE) // cannot open the drive
	{
		return DWORD(-1);
	}

	DWORD readed = 0;                   // discard results
	STORAGE_DEVICE_NUMBER number{};       //use this to get disk numbers
	BOOL result = DeviceIoControl(
		hDevice,                // handle to device
		IOCTL_STORAGE_GET_DEVICE_NUMBER, // dwIoControlCode
		NULL,                            // lpInBuffer
		0,                               // nInBufferSize
		&number,           // output buffer
		sizeof(number),         // size of output buffer
		&readed,       // number of bytes returned
		NULL      // OVERLAPPED structure
	);
	if (!result) // fail
	{
		AssertEx(result);
		(void)CloseHandle(hDevice);
		return (DWORD)-1;
	}
	//printf("%d %d %d\n\n", number.DeviceType, number.DeviceNumber, number.PartitionNumber);

	(void)CloseHandle(hDevice);
	return number.DeviceNumber;
}
#include<cwctype>
static bool caseInsensitiveCompare(wchar_t a, wchar_t b) {
	// 如果两个字符都是字母，则转换为小写进行比较  
	if (std::iswalpha(a) && std::iswalpha(b)) {
		return std::towlower(a) == std::towlower(b);
	}
	// 否则，直接比较  
	return a == b;
}
static std::wstring::const_iterator find_substring_case_insensitive(const std::wstring& haystack, const std::wstring& needle) {
	auto it = std::search(haystack.begin(), haystack.end(),
		needle.begin(), needle.end(),
		[](wchar_t a, wchar_t b) {
			return caseInsensitiveCompare(a, b);
		});
	return it;
}
static
// 递归查找最新修改的文件  
std::wstring FindNewestFile(const std::wstring& directory) {
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile((directory + L"\\*").c_str(), &findData);
	if (hFind == INVALID_HANDLE_VALUE) {
		std::wcerr << L"Failed to find files in directory: " << directory << std::endl;
		return L"";
	}

	std::wstring newestFilePath;
	FILETIME newestFileTime = { 0 };

	do {
		if ((wcscmp(findData.cFileName, L".") == 0) ||
			(wcscmp(findData.cFileName, L"..") == 0)) {
			// 忽略目录和特殊文件（. 和 ..）  
			continue;
		}

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// 如果是目录，则递归查找  
			std::wstring subdirPath = directory + L"\\" + findData.cFileName;
			std::wstring result = FindNewestFile(subdirPath);
			if (!result.empty()) {
				FILETIME subDirFileTime;
				GetFileTime((HANDLE)_wtoi64(result.substr(0, result.find_last_of(L'\\')).c_str()), nullptr, nullptr, &subDirFileTime);
				if (CompareFileTime(&subDirFileTime, &newestFileTime) > 0) {
					newestFileTime = subDirFileTime;
					newestFilePath = result;
				}
			}
		}
		else {
			// 如果是文件，则比较修改时间  
			if (CompareFileTime(&findData.ftLastWriteTime, &newestFileTime) > 0) {
				newestFileTime = findData.ftLastWriteTime;
				newestFilePath = directory + L"\\" + findData.cFileName;
			}
		}
	} while (FindNextFile(hFind, &findData));

	FindClose(hFind);
	return newestFilePath;
}

class UI_Main;
#include <sciter-x.h>
#include <sciter-x-window.hpp>
#include <sciter-x-graphics.hpp>
#include <set>
#include <unordered_set>
class MainUI_data {
public:
	MainUI_data() :
		hWnd(0),
		phi_endTime(0)
	{};

	HWND hWnd;

	sciter::om::hasset<UI_Main> phInject, phEject;
	time_t phi_endTime;
};
class UI_Main
	: public sciter::window
{
public:
	UI_Main() : window(SW_RESIZEABLE | SW_ENABLE_DEBUG | SW_CONTROLS | SW_TITLEBAR) {
		pUiData = nullptr;
	}

	void expand(bool ma = false) {
		window::expand(ma);
		activate(true);
	}

	vector<wstring> devices;
	vector<wstring> disks, ddsk;
	MainUI_data* pUiData;

	int getSomeData(int param1) { return param1 + 1; };

	//void addTime() {
	//	this->pUiData->phi_endTime += 5;
	//};

	sciter::value getDevices() {
		return sciter::value(devices);
	};
	
	map<wstring, vector<sciter::value>> findVolumeFileList__cache;
	sciter::value findVolumeFileList(wstring volume) {
		if (findVolumeFileList__cache.contains(volume)) {
			return findVolumeFileList__cache.at(volume); // caching
		}
		vector<sciter::value> result;
		struct my { 
			sciter::value val;
			ULONGLONG i;
		};
		vector<my> rb;

		WCHAR buffer2[256]{}; DWORD buffer3 = 0;
		if (GetVolumePathNamesForVolumeNameW(volume.c_str(), buffer2, 256, &buffer3))
			volume = buffer2;

#if 0
		wstring latestFile = FindNewestFile(volume);
		if (!latestFile.empty()) {
			sciter::value file;
			file["name"] = latestFile.substr(latestFile.find_last_of(L"\\") + 1);
			file["path"] = latestFile;
			result.push_back(file);
		}
#else

		WIN32_FIND_DATA findData;
		HANDLE hFind = FindFirstFile((volume + L"\\*").c_str(), &findData);
		if (hFind == INVALID_HANDLE_VALUE) {
			return result;
		}

		std::wstring newestFilePath;
		FILETIME newestFileTime = { 0 };

		do {
			if ((wcscmp(findData.cFileName, L".") == 0) ||
				(wcscmp(findData.cFileName, L"..") == 0)) {
				// 忽略目录和特殊文件（. 和 ..）  
				continue;
			}

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) {
				continue; // 排除$Recycle.Bin、System Volume Information一类的特殊文件夹
			}

			{
				sciter::value file;
				file["name"] = wstring(findData.cFileName);
				file["path"] = wstring(volume) + findData.cFileName;
				ULARGE_INTEGER uint{};
				uint.LowPart = findData.ftLastWriteTime.dwLowDateTime;
				uint.HighPart = findData.ftLastWriteTime.dwHighDateTime;
				{
					SYSTEMTIME st{};
					WCHAR buffer[512]{};
					WCHAR buffer2[512]{};
					wstring result;
					FileTimeToSystemTime(&findData.ftLastWriteTime, &st);
					PCWSTR pDateFormat = NULL, pTimeFormat = NULL;
					GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pTimeFormat, buffer, 512);
					GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pDateFormat, buffer2, 512, 0);
					result = buffer2;
					result += L" ";
					result += buffer;
					file["time"] = result;
				}
				my st{.val=file,.i=uint.QuadPart};
				rb.push_back(st);
			}
		} while (FindNextFile(hFind, &findData));

		FindClose(hFind);
#endif

		std::sort(rb.begin(), rb.end(), [](const my& a, const my& b) {
			return a.i > b.i; // 降序排序  
		});

		for (auto& i : rb) result.push_back(i.val);
		
		findVolumeFileList__cache.insert(std::make_pair(volume, result)); // to avoid duplicate finding
		return result;
	};

	void openDisk(wstring volume) {
		WCHAR buffer2[256]{}; DWORD buffer3 = 0;
		if (!GetVolumePathNamesForVolumeNameW(volume.c_str(), buffer2, 256, &buffer3)) {
			ShellExecute(0, L"open", volume.c_str(), 0, 0, 1);
			return;
		}

		ShellExecute(0, L"open", buffer2, 0, 0, 1);
	};
	void openFile(wstring file) {
		ShellExecute(0, L"open", file.c_str(), 0, 0, 1);
	};

	sciter::value getDiskList() {
		//if (!this->ddsk.empty()) return ddsk;
		//if (!this->disks.empty()) return disks;

		vector<wstring>& dsk = ddsk;
		dsk.clear(); disks.clear();

		WCHAR volumeName[MAX_PATH]{};
		HANDLE hFind = FindFirstVolumeW(volumeName, MAX_PATH);
		if (hFind == INVALID_HANDLE_VALUE) {
			return 0;
		}

		WCHAR buffer2[256]{}, buffer4[128]{}, buffer7[32]{};
		DWORD buffer3 = 0, buffer5 = 0, buffer6 = 0;

		unordered_set<LONGLONG> vol;
#if 0
#define h {(MessageBoxW(0,LastErrorStrW().c_str(),to_wstring(GetLastError()).c_str(),MB_ICONERROR));}
#undef h
#define h 
		for (auto& I : this->devices) {
			vector<wstring> _Tmp1; str_split(I, L"|", _Tmp1); wstring i = _Tmp1.size() > 1 ? _Tmp1[1] : L"";
			if (!i.starts_with(L"\\\\?\\USBSTOR#")) continue;
			HANDLE hFile = CreateFileW(i.c_str(), GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				0, OPEN_EXISTING, NULL, NULL);
			if (!hFile || hFile == INVALID_HANDLE_VALUE) {
				h continue;
			}

			DWORD dwBytesReturned = 0;
#if 0
			constexpr DWORD dwLayoutSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + 256 * sizeof(PARTITION_INFORMATION_EX); // 假设最多256个分区  
			DRIVE_LAYOUT_INFORMATION_EX* pDriveLayout = (DRIVE_LAYOUT_INFORMATION_EX*)calloc(1, dwLayoutSize);
			if (!pDriveLayout) {
				CloseHandle(hFile);
				continue;
			}

			BOOL bResult = DeviceIoControl(
				hFile, IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
				NULL, 0, pDriveLayout, dwLayoutSize,
				&dwBytesReturned, NULL
			);

			if (!bResult) {
				free(pDriveLayout);
				CloseHandle(hFile);
				continue;
			}

			for (size_t i = 0; i < 256 && i < pDriveLayout->PartitionCount; ++i) {
				auto& entry = pDriveLayout->PartitionEntry[i];
				entry.PartitionNumber
			}

			free(pDriveLayout);
#else
			STORAGE_DEVICE_NUMBER sdn{};
			BOOL bResult = DeviceIoControl(
				hFile, IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
				NULL, 0, &sdn, sizeof(sdn),
				&dwBytesReturned, NULL
			);
			if (!bResult) {
				CloseHandle(hFile);
				h continue;
			}
			vol.emplace(sdn.DeviceNumber);
#endif
			CloseHandle(hFile);
		}
#else
		do {
			// wmi call
			HRESULT hres = 0;
			// 获取WMI的IWbemLocator对象指针  
#pragma comment(lib, "wbemuuid.lib")
			IWbemLocator* pLoc = NULL;
			hres = CoCreateInstance(
				CLSID_WbemLocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID*)&pLoc);
			if (FAILED(hres)) {
				break;
			}
			// 连接到WMI命名空间  
			IWbemServices* pSvc = NULL;
			// 使用root\CIMV2命名空间  
			hres = pLoc->ConnectServer(
				BSTR(L"root\\CimV2"), // Object path of WMI namespace  
				NULL,                    // User name. NULL = current user  
				NULL,                    // User password. NULL = current  
				0,                       // Locale. NULL indicates current  
				NULL,                    // Security flags.  
				0,                       // Authority (e.g. Kerberos)  
				0,                       // Context object   
				&pSvc                    // pointer to IWbemServices proxy  
			);
			if (FAILED(hres)) {

				pLoc->Release();
				break;
			}

			// 在这里，你可以使用pSvc指针来执行WMI查询  
			IEnumWbemClassObject* pEnumerator = nullptr;
			BSTR Language = SysAllocString(L"WQL");
			BSTR Query = SysAllocString(L"SELECT DeviceID,InterfaceType,PNPDeviceID FROM Win32_DiskDrive");
			if (FAILED(pSvc->ExecQuery(
				Language, Query,
				WBEM_FLAG_RETURN_IMMEDIATELY | WBEM_FLAG_FORWARD_ONLY,
				NULL, &pEnumerator
			))) {
				pSvc->Release();
				pLoc->Release();
				break;
			}
			SysFreeString(Language); SysFreeString(Query);

			// 遍历结果  
			IWbemClassObject* pclsObj = NULL;
			ULONG uReturn = 0;

			while (pEnumerator) {
				HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
				if (0 == uReturn) {
					//MessageBoxW(0, to_wstring(hr).c_str(), 0, 0);
					break;
				}

				VARIANT vtProp{};
				wstring dev, type, pnp;

				// 获取 DeviceID  
				hr = pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0);
				//wprintf(L"DeviceID : %s\n", vtProp.bstrVal ? vtProp.bstrVal : L"(null)");
				dev = vtProp.bstrVal ? vtProp.bstrVal : L"";
				VariantClear(&vtProp);

				// 获取 interfacetype  
				hr = pclsObj->Get(L"interfacetype", 0, &vtProp, 0, 0);
				//wprintf(L"InterfaceType : %lu\n", vtProp.uintVal);
				type = vtProp.bstrVal ? vtProp.bstrVal : L"";
				VariantClear(&vtProp);

				// 获取 PnPDeviceID  
				hr = pclsObj->Get(L"PNPDeviceID", 0, &vtProp, 0, 0);
				//wprintf(L"PnPDeviceID : %s\n", vtProp.bstrVal ? vtProp.bstrVal : L"(null)");
				pnp = vtProp.bstrVal ? vtProp.bstrVal : L"";
				VariantClear(&vtProp);

				pclsObj->Release();

				// 使用values

				for (auto& I : this->devices) try {
					vector<wstring> _Tmp1; str_split(I, L"|", _Tmp1); wstring i = _Tmp1.size() > 1 ? _Tmp1[1] : L"";

					if (!(
						i.starts_with(L"\\\\?\\USBSTOR#") || 
						i.starts_with(L"\\\\?\\SCSI#Disk")
					)) continue;
					str_replace(i, L"#", L"\\");
					if (i.end() != find_substring_case_insensitive(i, pnp)) {
						//MessageBox(0, L"found", 0, 0);
						//MessageBox(0, pnp.c_str(), 0, 0);

						vol.emplace(atoll(ws2c(dev.substr(17))));
					}
				} catch (std::bad_alloc&) {}
				
			}
			// 清理  
			pEnumerator->Release();

			// 清理  
			pSvc->Release();
			pLoc->Release();
		} while (0);
#endif
		
		do {
			volumeName[2] = L'.'; //  \\?\ -> \\.\ 
			volumeName[48] = 0; // to remove \ on the tail 
			HANDLE hFile = CreateFileW(volumeName, FILE_GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				0, OPEN_EXISTING, NULL, NULL);
			volumeName[2] = L'?';
			volumeName[48] = L'\\';
			if (!hFile || hFile == INVALID_HANDLE_VALUE) {
				{ (MessageBoxW(0, volumeName, LastErrorStrW().c_str(), 0x00000010L)); }
				continue;
			}
			DWORD dwBytesReturned = 0;
			STORAGE_DEVICE_NUMBER sdn{};
			BOOL bResult = DeviceIoControl(
				hFile, IOCTL_STORAGE_GET_DEVICE_NUMBER,
				NULL, 0, &sdn, sizeof(sdn),
				&dwBytesReturned, NULL
			);
			if (!bResult) {
				CloseHandle(hFile);
				continue;
			}
			LONGLONG devN = sdn.DeviceNumber;
			CloseHandle(hFile);
			if (!vol.contains(devN)) continue;

			this->disks.push_back(volumeName);
			wstring str = volumeName + L"|"s;

			if (!GetVolumePathNamesForVolumeNameW(volumeName, buffer2, 256, &buffer3)) {
				dsk.push_back(str + L"无装载点 : "s + volumeName);
				continue;
			}

			if (!GetVolumeInformationW(buffer2, buffer4, 128, NULL, &buffer5, &buffer6, buffer7, 32)) continue;
			if (buffer4[0] == 0) wcscpy_s(buffer4, L"磁盘驱动器");
			str += buffer4 + L" : "s + buffer2;
			dsk.push_back(str);
		} while (FindNextVolumeW(hFind, volumeName, MAX_PATH));

		FindVolumeClose(hFind);

		return dsk;
#undef h
	};

	SOM_PASSPORT_BEGIN(UI_Main)
		SOM_FUNCS(
			SOM_FUNC(getSomeData),
			SOM_FUNC(openDisk),
			SOM_FUNC(openFile),
			SOM_FUNC(getDiskList),
			SOM_FUNC(findVolumeFileList),
			SOM_FUNC(getDevices)
		)
	SOM_PASSPORT_END


};



// 托盘图标的句柄  
HMENU hTrayMenu;
NOTIFYICONDATA nid;

HWND hwMain;



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!IsRunAsAdmin()) {
		CmdLineW cl(GetCommandLineW());
		SHELLEXECUTEINFO execinfo{ 0 };
		wstring lpFile = s2ws(GetProgramDir());
		execinfo.lpFile = lpFile.c_str();
		execinfo.cbSize = sizeof(execinfo);
		execinfo.lpVerb = L"runas";
		execinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		execinfo.nShow = SW_SHOWDEFAULT;
		wstring szParam;
		for (size_t i = 1; i < cl.argc(); ++i)
			szParam += L" \""s + cl[i] + L"\"";
		if (IsDebuggerPresent()) {
			szParam += L" --attach-debug";
		}
		execinfo.lpParameters = szParam.c_str();
		if (!ShellExecuteExW(&execinfo)) return GetLastError();
		DWORD exitCode = 1;
		if (execinfo.hProcess) {
			WaitForSingleObject(execinfo.hProcess, INFINITE);
			GetExitCodeProcess(execinfo.hProcess, &exitCode);
			CloseHandle(execinfo.hProcess);
		}
		return exitCode;
	}
	

	// TODO: 在此处放置代码。
	auto co = CoInitializeEx(0, COINIT_MULTITHREADED); (void)co;
	//auto ole = OleInitialize(0);
	//AssertEx_AutoHandle(ole == S_OK || ole == S_FALSE); // for system drag-n-drop
	AssertEx(SUCCEEDED(CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication  
		NULL,                        // Authentication services  
		NULL,                        // Reserved  
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication  
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info  
		EOAC_NONE,                   // Additional capabilities  
		NULL                         // Reserved  
	)));
	SetCurrentDirectoryW(GetProgramPathW().c_str());
	CmdLineW cl(GetCommandLineW());
	if (cl.getopt(L"attach-debug")) {
		Process.StartAndWait(L"vsjitdebugger -p " + to_wstring(GetCurrentProcessId()));
	}
	{ // codes to load Sciter
		HMODULE hSciter = LoadLibraryExW(L"sciter.dll", NULL,
			LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		if (!hSciter) {
			FreeResFile(IDR_BIN_SciterAPI, L"BIN", L"sciter.dll");
		}
	}
	SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MYUSBDEVICEHELPER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (HWND _ = FindWindowW(szWindowClass, NULL)) {
		// already running
		PostMessage(_, WM_USER + 2, 0, 0);
		return 0;
	}

	// 执行应用程序初始化:
	nCmdShow = 0;
	HWND hwnd = InitInstance(hInstance, nCmdShow);
	if (!hwnd)
	{
		MessageBoxW(0, LastErrorStrW().c_str(), 0, MB_ICONERROR);
		return FALSE;
	}
	hwMain = hwnd;

	if (!MyRegisterDevNotif(hwnd)) {
		MessageBoxW(0, LastErrorStrW().c_str(), 0, MB_ICONERROR);
		DestroyWindow(hwnd);
		return GetLastError();
	}
	
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);


	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYUSBDEVICEHELPER));

	MSG msg;

	//HANDLE hSciterThread = CreateThread(0, 0, SCTH, 0, 0, 0);

	sciter::application::start();
	::SciterSetOption(NULL,SCITER_SET_UX_THEMING,TRUE);

	SciterSetOption(NULL, SCITER_SET_DEBUG_MODE, TRUE);
	SciterSetOption(NULL, SCITER_SET_SCRIPT_RUNTIME_FEATURES,
		ALLOW_FILE_IO |
		ALLOW_SOCKET_IO |
		ALLOW_EVAL |
		ALLOW_SYSINFO);

	SciterSetGlobalAsset(new NativeBackend());

#if 0
	sciter::application::run();

	sciter::application::shutdown();

	return 0;
#else
	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnBd, hAccelTable, &msg))
		//{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		//}
	}

	//sciter::application::shutdown();

	//OleUninitialize();
	CoUninitialize();
#if 0
	if (hSciterThread) {
		sciter::application::request_quit(0);
		if (WAIT_TIMEOUT == WaitForSingleObject(hSciterThread, 5000)) {
			sciter::application::shutdown();
			if (WAIT_TIMEOUT == WaitForSingleObject(hSciterThread, 1000)) {
#pragma warning(disable: 6258)
				TerminateThread(hSciterThread, ERROR_TIMEOUT);
			}
		}
		CloseHandle(hSciterThread);
	}
#endif

	return (int) msg.wParam;
#endif
}




#pragma comment(lib, "setupapi.lib")  
#pragma comment(lib, "cfgmgr32.lib") 
__callback DWORD(CALLBACK MyDevCb)(
	_In_ HCMNOTIFICATION       hNotify,
	_In_opt_ PVOID             Context,
	_In_ CM_NOTIFY_ACTION      Action,
	_In_reads_bytes_(EventDataSize) PCM_NOTIFY_EVENT_DATA EventData,
	_In_ DWORD                 EventDataSize
) {
	switch (Action)
	{
	case CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL:
	case CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL:
	{
		wchar_t* mem = (wchar_t*)calloc(sizeof(wchar_t), 2104);
		if (!mem) break;

		wchar_t tmp[40] = { 0 };
		GUID& uuid = EventData->u.DeviceInterface.ClassGuid;
		wsprintf(tmp, L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x|",
			uuid.Data1, uuid.Data2, uuid.Data3,
			uuid.Data4[0], uuid.Data4[1],
			uuid.Data4[2], uuid.Data4[3],
			uuid.Data4[4], uuid.Data4[5],
			uuid.Data4[6], uuid.Data4[7]);
		wcscpy_s(mem, 2048, tmp);
		wcscat_s(mem, 2104, EventData->u.DeviceInterface.SymbolicLink);

		PostMessage(hwMain, WM_USER + 3, Action, (LPARAM)mem);
	}
		break;

	default:
		break;
	}


	return ERROR_SUCCESS;
}

#if 0
#define DEBUG_NO_DISAPPEAR 1
#endif


#pragma region useless
//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYUSBDEVICEHELPER));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = 0;//MAKEINTRESOURCEW(IDC_MYUSBDEVICEHELPER);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }


   return hWnd;
}
#pragma endregion
//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MainUI_data* data = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	switch (message)
	{
	case WM_CREATE: {
		MainUI_data* dat = new MainUI_data;
		memset(dat, 0, sizeof(MainUI_data));
		dat->hWnd = hwnd;

		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)dat);

		// 创建上下文菜单  
		hTrayMenu = CreatePopupMenu();
		AppendMenu(hTrayMenu, MF_STRING, 1, _T("退出 (&Exit)"));

		// 初始化托盘图标数据  
		ZeroMemory(&nid, sizeof(nid));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd; // 你的主窗口句柄  
		nid.uID = IDI_MYUSBDEVICEHELPER;
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		nid.uCallbackMessage = WM_USER + 1; // 自定义消息，用于托盘图标的点击等事件  
		nid.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_MYUSBDEVICEHELPER));
		lstrcpy(nid.szTip, _T("USB Device Helper"));

		// 显示托盘图标  
		Shell_NotifyIcon(NIM_ADD, &nid);

		nid.uFlags |= NIF_INFO;
		wcscpy_s(nid.szInfo, L"应用程序正在运行");
		wcscpy_s(nid.szInfoTitle, L"USB Device Helper 应用程序");
		nid.dwInfoFlags = NIIF_INFO;
		//nid.hBalloonIcon = LoadIconW(NULL, IDI_INFORMATION);
		Shell_NotifyIcon(NIM_MODIFY, &nid);
		std::thread([] {
			Sleep(2000);
			nid.szInfo[0] = 0;
			nid.dwInfoFlags = 0;
			Shell_NotifyIcon(NIM_MODIFY, &nid);
		}).detach();
	}
		break;

	case WM_USER + 3:
	{
		wchar_t* mem = (wchar_t*)lParam;
		if (!mem) return 0;
		sciter::application::start();
		switch (wParam)
		{
		case CM_NOTIFY_ACTION_DEVICEINTERFACEARRIVAL: {
			data->phi_endTime = time(0) + 24;
			if (!data->phInject || !data->phInject->is_valid()) {
				WCHAR pCurrentDir[MAX_PATH + 16]{};
				GetCurrentDirectoryW(MAX_PATH + 16, pCurrentDir);
				std::wstring full_file_name = pCurrentDir + L"\\.data\\device_arrival.html"s;
				data->phInject = new UI_Main();
				data->phInject->asset_add_ref();
				SciterSetMediaType(data->phInject->get_hwnd(), WSTR("desktop"));
				data->phInject->load((L"file://" + str_replace(full_file_name, L"\\", L"/")).c_str());
				data->phInject->bind();

				data->phInject->pUiData = data;

#ifndef DEBUG_NO_DISAPPEAR
				std::thread([data] {
					while (time(0) < data->phi_endTime) Sleep(1000);
					if (!data->phInject) return;
					data->phInject->request_close();
					data->phInject = nullptr;
				}).detach();
#endif
			}
			auto wnd = (data->phInject);
			wnd->devices.push_back(mem);
			wnd->expand(false);

			try { wnd->call_function("Load"); }
			catch (sciter::script_error& error) {
				MessageBoxW(wnd->get_hwnd(), s2wc(error.what()), 0, MB_ICONERROR);
			}
			SetWindowPos(wnd->get_hwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(wnd->get_hwnd(), 0, 0, 0, 400, 400, SWP_NOMOVE | SWP_NOZORDER);
			RECT screenRect{}, windowRect{};
			SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0); // 获取工作区大小，考虑任务栏  
			GetWindowRect(wnd->get_hwnd(), &windowRect);
			int newX = screenRect.right - windowRect.right + windowRect.left  - 20;
			int newY = screenRect.bottom - windowRect.bottom + windowRect.top - 20;
			SetWindowPos(wnd->get_hwnd(), NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

		}
			break;
		case CM_NOTIFY_ACTION_DEVICEINTERFACEREMOVAL: {
			if (!data->phEject || !data->phEject->is_valid()) {
				WCHAR pCurrentDir[MAX_PATH + 16]{};
				GetCurrentDirectoryW(MAX_PATH + 16, pCurrentDir);
				std::wstring full_file_name = pCurrentDir + L"\\.data\\device_eject.html"s;
				data->phEject = new UI_Main();
				SciterSetMediaType(data->phEject->get_hwnd(), WSTR("desktop"));
				data->phEject->load((L"file://" + str_replace(full_file_name, L"\\", L"/")).c_str());
			}
			auto& wnd = (data->phEject);
			wnd->devices.push_back(mem);
			wnd->expand(false);
			SetWindowPos(wnd->get_hwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			SetWindowPos(wnd->get_hwnd(), 0, 0, 0, 150, 100, SWP_NOMOVE | SWP_NOZORDER);
			RECT screenRect{}, windowRect{};
			SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0); // 获取工作区大小，考虑任务栏  
			GetWindowRect(wnd->get_hwnd(), &windowRect);
			int newX = screenRect.right - windowRect.right + windowRect.left  - 20;
			int newY = screenRect.bottom - windowRect.bottom + windowRect.top - 20;
			SetWindowPos(wnd->get_hwnd(), NULL, newX, newY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

#ifndef DEBUG_NO_DISAPPEAR
			std::thread([data] {
				Sleep(3000);
				if (!data->phEject) return;
				data->phEject->request_close();
				data->phEject = nullptr;
			}).detach();
#endif
		}
			break;
		default:;
		}
		free(mem);
	}
		break;

	case WM_USER + 1: // 自定义的托盘图标消息  
		switch (lParam) {
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		{
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow(hwnd);
			int r = TrackPopupMenu(hTrayMenu, TPM_RIGHTBUTTON | TPM_RETURNCMD,
				pt.x, pt.y, 0, hwnd, NULL);

			if (r == 0) break;
			else if (r == 1) PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
			break;
		}
		break;

	case WM_USER+2:
	{
		nid.uFlags |= NIF_INFO;
		wcscpy_s(nid.szInfo, L"已经有一个应用程序的实例在运行");
		wcscpy_s(nid.szInfoTitle, L"USB Device Helper 应用程序");
		nid.dwInfoFlags = NIIF_ERROR;
		Shell_NotifyIcon(NIM_MODIFY, &nid);
		std::thread([] {
			Sleep(2000);
			nid.szInfo[0] = 0;
			nid.dwInfoFlags = 0;
			Shell_NotifyIcon(NIM_MODIFY, &nid);
		}).detach();
	}
		break;

#if 0
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hwnd);
				break;
			default:
				return DefWindowProc(hwnd, message, wParam, lParam);
			}
		}
		break;
#endif

	case WM_DEVICECHANGE: {
		return 1;
#if 0
		static time_t antitik = 0;
		time_t ct = time(0);
		if (ct - antitik < 2) break;
		antitik = ct;

		switch (wParam) {
		case DBT_DEVICEARRIVAL: {
			PDEV_BROADCAST_HDR lpdbh = (PDEV_BROADCAST_HDR)lParam;
			if (lpdbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
				PDEV_BROADCAST_DEVICEINTERFACE lpdbdi = (PDEV_BROADCAST_DEVICEINTERFACE)lpdbh;
				// 处理设备插入事件  
				// 例如，你可以使用lpdbdi->dbcc_name来获取设备接口名

				WCHAR pCurrentDir[MAX_PATH + 16]{};
				GetCurrentDirectoryW(MAX_PATH + 16, pCurrentDir);
				using namespace std;
				std::wstring full_file_name = pCurrentDir + L"\\.data\\device_arrival.html"s;
				auto wnd = new UI_Main;
				wnd->device = lpdbdi->dbcc_name;
				wnd->load(full_file_name.c_str());
				wnd->expand(false);
				SetWindowPos(wnd->get_hwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			}
		}
			break;
		case DBT_DEVICEREMOVECOMPLETE: {
			PDEV_BROADCAST_HDR lpdbh = (PDEV_BROADCAST_HDR)lParam;
			if (lpdbh->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
				PDEV_BROADCAST_DEVICEINTERFACE lpdbdi = (PDEV_BROADCAST_DEVICEINTERFACE)lpdbh;
				// 处理设备拔出事件  
				WCHAR pCurrentDir[MAX_PATH + 16]{};
				GetCurrentDirectoryW(MAX_PATH + 16, pCurrentDir);
				using namespace std;
				std::wstring full_file_name = pCurrentDir + L"\\.data\\device_eject.html"s;
				auto wnd = new UI_Main;
				wnd->device = lpdbdi->dbcc_name;
				wnd->load(full_file_name.c_str());
				wnd->expand(false);
				SetWindowPos(wnd->get_hwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
			break;
		default:;
		}
		return TRUE;
#endif
	}
		break;

	case WM_DESTROY:
		MyUnregisterDevNotif();
		Shell_NotifyIcon(NIM_DELETE, &nid);
		DestroyMenu(hTrayMenu);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}







