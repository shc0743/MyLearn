#include <iostream>
#pragma comment(lib, "MyProgressWizardLib64.lib")
#include "../../resource/tool.h"
#include "wizard.user.h"
#include <cfgmgr32.h>
#include <SetupAPI.h>

#include "resource.h"

using namespace std;

#pragma comment(linker, "/entry:mainCRTStartup /subsystem:windows")

#define LOG(x) MessageBoxW(NULL, x, L"LOG", MB_ICONINFORMATION)


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
#if 0
unsigned long long QueryDiskDriveNumberByPath(std::wstring volumePath) {
	HANDLE hDevice = CreateFileW(volumePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return -1;
	}

	STORAGE_DEVICE_NUMBER sdn{};
	DWORD dwBytesReturned = 0;
	if (!DeviceIoControl(hDevice, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL)) {
		CloseHandle(hDevice);
		return -1;
	}

	CloseHandle(hDevice);
	return sdn.DeviceNumber;
}
#endif
std::wstring QueryVolumePathNameByFilePath(std::wstring filePath) {
	WCHAR szVolumePath[MAX_PATH] = L"";
	if (!GetVolumePathNameW(filePath.c_str(), szVolumePath, MAX_PATH)) {
		return L"";
	}

	return szVolumePath;
}
std::wstring QueryVolumePathByFilePath(std::wstring filePath) {
	WCHAR szVolumeGuid[64] = L""; // GUID string representation is always 38 characters + null terminator
	if (GetVolumeNameForVolumeMountPointW(QueryVolumePathNameByFilePath(filePath).c_str(), szVolumeGuid, 64)) {
		return szVolumeGuid;
	}
	return L"";
}
bool SetDiskDriveConnectionStatus(DWORD diskNumber, bool bOnline) {
	HANDLE hDevice = CreateFileW((L"\\\\.\\PhysicalDrive" + to_wstring(diskNumber)).c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return false;
	}

	SET_DISK_ATTRIBUTES sda{};
	sda.Version = sizeof(SET_DISK_ATTRIBUTES);
	sda.Persist = true;
	sda.AttributesMask = DISK_ATTRIBUTE_OFFLINE;
	sda.Attributes = bOnline ? 0 : DISK_ATTRIBUTE_OFFLINE;

	DWORD dwBytesReturned = 0;
	BOOL bResult = DeviceIoControl(hDevice, IOCTL_DISK_SET_DISK_ATTRIBUTES, &sda, sizeof(sda), 0, 0, &dwBytesReturned, 0);
	CloseHandle(hDevice);
	return bResult;
}
static bool IsRemovableDrive(const std::wstring& disk) {
	UINT driveType = GetDriveTypeW(disk.c_str());

	// 可移除的磁盘驱动器通常是U盘  
	if (driveType == DRIVE_REMOVABLE) {
		return true;
	}

	return false;
}

bool EjectRemovableDevice(wstring volume);
bool EjectDevice(DWORD DiskNumber);
DWORD GetSystemDrivePhysicalDriveId();


static bool isEjectCancelled = false;
bool __stdcall EjectCancelHandler (HMPRGWIZ hWiz, HMPRGOBJ hObj) {
	isEjectCancelled = true;
	SetMprgWizAttribute(hWiz, MPRG_WIZARD_EXTENSIBLE_ATTRIBUTES::WizAttrCancelHandler, 0);
	SetMprgWizardText(hWiz, L"正在取消...");
	return true;
}

int main()
{
	CmdLine cl(GetCommandLine());

	if (!IsRunAsAdmin()) {
		bool success = 0 == ShellExecuteW(NULL, L"runas", GetProgramDirW().c_str(), GetCommandLineW(), NULL, SW_SHOWNORMAL);
		return success ? 0 : 1;
	}

	if (cl.argc() < 1 || (cl[0] != L"/" && (!cl.getopt(L"debug")))) {
		// TODO: Generate temp file name
		WCHAR szTempPath[1024];
		GetTempPathW(1024, szTempPath);
		wstring szTempName = L"DEV" + GenerateUUIDWithoutDelimW();
		if (cl.getopt(L"debugable")) szTempName += L".exe";
#pragma comment(lib, "Shlwapi.lib")
		PathAppendW(szTempPath, szTempName.c_str());
		bool succ = CopyFileW(GetProgramDirW().c_str(), szTempPath, FALSE);
		if (!succ) {
			MessageBoxW(NULL, LastErrorStrW().c_str(), L"Error - Failed to copy file", MB_ICONERROR);
			return GetLastError();
		}
		
		WCHAR cmd[256]{};
		wsprintfW(cmd, L"/ --spawn=%i --from=\"%s\" -- %s", (int)GetCurrentProcessId(), GetProgramDirW().c_str(), GetCommandLineW());
		STARTUPINFO si{}; PROCESS_INFORMATION pi{};
		if (!CreateProcessW(szTempPath, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			MessageBoxW(NULL, LastErrorStrW().c_str(), L"Error - Failed to create process", MB_ICONERROR);
			return GetLastError();
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return 0;
	}

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
	InitMprgComponent();

	HMPRGOBJ hObj = CreateMprgObject();
	HMPRGWIZ hWiz = CreateMprgWizard(hObj, MPRG_CREATE_PARAMS{
		.cb = sizeof(MPRG_CREATE_PARAMS),
		.szTitle = L"Eject Device",
		.szText = L"Please wait while the device is being ejected...",
		.max = (ULONGLONG)-1,
	});

	OpenMprgWizard(hWiz);

	SetMprgWizardText(hWiz, L"正在请求查询...");
	SetCurrentDirectoryW(GetProgramPathW().c_str());

	const auto self_delete = [] {
		wstring out = GetProgramPathW() + L"\\DELETE" + s2ws(GetProgramInfo().name) + L".bat";
		FreeResFile(IDR_BIN_DELETESELF, L"BIN", out);
		Process.StartOnly_HiddenWindow(L"cmd /c \"" + out + L"\" \"" + GetProgramDirW() + L"\"");
	};

	wstring filewhere; cl.getopt(L"from", filewhere);
	wstring volume = QueryVolumePathByFilePath(filewhere);
	volume.erase(volume.length() - 1);
	DWORD devnum = GetPhysicalDriveFromVolumeName(volume);
	if (devnum == -1) {
		DWORD err = GetLastError();
		MessageBoxW(GetMprgHwnd(hWiz), ErrorCodeToString(err).c_str(), L"Error - Failed to query device number", MB_ICONERROR);
		DestroyMprgWizard(hObj, hWiz);
		DeleteMprgObject(hObj);
		self_delete();
		return err;
	}

	if (!cl.getopt(L"allow-bsod")) {
		if (devnum == GetSystemDrivePhysicalDriveId()) {
			MessageBoxW(GetMprgHwnd(hWiz), L"无法弹出系统驱动器所在磁盘，这可能会导致系统崩溃。\n如果你知道这样做的后果，使用 --allow-bsod 命令行选项。", NULL, MB_ICONERROR);
			DestroyMprgWizard(hObj, hWiz);
			DeleteMprgObject(hObj);
			self_delete();
			return ERROR_NOT_ALLOWED_ON_SYSTEM_FILE;
		}
	}

	SetMprgWizAttribute(hWiz, MPRG_WIZARD_EXTENSIBLE_ATTRIBUTES::WizAttrCancelHandler, (LONG_PTR)EjectCancelHandler);
	auto wizdata = GetModifiableMprgWizardData(hWiz);
	wizdata->max = 20;
	wizdata->value = 0;
	UpdateMprgWizard(hWiz);
	for (int i = 2; i >= 0; --i) {
		if (isEjectCancelled) break;
		wstring ss = (L"磁盘 " + to_wstring(devnum) + L" 将于 " + to_wstring(i) + L" 秒后弹出，是否取消？");
		SetMprgWizardText(hWiz, ss.c_str());
		for (int j = 0; j < 10; ++j) {
			wizdata->value++;
			UpdateMprgWizard(hWiz);

			Sleep(100);
			if (isEjectCancelled) break;
		}
	}
	if (isEjectCancelled) {
		Sleep(1000);
		DestroyMprgWizard(hObj, hWiz);
		DeleteMprgObject(hObj);
		self_delete();
		return 0;
	}
	SetMprgWizAttribute(hWiz, MPRG_WIZARD_EXTENSIBLE_ATTRIBUTES::WizAttrCancelHandler, (LONG_PTR)0);

	wizdata->max = 100;
	wizdata->value = 5;
	UpdateMprgWizard(hWiz);
	SetMprgWizardText(hWiz, L"等待进程结束...");
	{
		wstring s_spawn; cl.getopt(L"spawn", s_spawn);
		if (!s_spawn.empty()) {
			DWORD pid = atoi(ws2c(s_spawn));
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
			if (hProcess) {
				WaitForSingleObject(hProcess, INFINITE);
				CloseHandle(hProcess);
			}
		}
	}

	wizdata->value = 15;
	UpdateMprgWizard(hWiz);
	SetMprgWizardText(hWiz, (L"尝试弹出磁盘 " + to_wstring(devnum) + L"...").c_str());

	const auto cleanup = [&]() {
		DestroyMprgWizard(hObj, hWiz);
		DeleteMprgObject(hObj);
		CoUninitialize();
		self_delete();
	};

	if (EjectDevice(devnum)) {
		directejectsuccess:
		wizdata->value = 100;
		UpdateMprgWizard(hWiz);
		SetMprgWizardText(hWiz, (L"磁盘 " + to_wstring(devnum) + L" 已弹出。").c_str());
		MessageBoxTimeoutW(NULL, L"设备已弹出。", L"成功", MB_ICONINFORMATION, 0, 2000);
		Sleep(3000);
		cleanup();
		return 0;
	}
	Sleep(500);
	wizdata->value = 20;
	UpdateMprgWizard(hWiz);
	if (IsRemovableDrive(QueryVolumePathNameByFilePath(filewhere))) {
		SetMprgWizardText(hWiz, (L"尝试弹出媒体..."));
		if (EjectRemovableDevice(volume)) {
			if (EjectDevice(devnum)) goto directejectsuccess;

			wizdata->value = 100;
			UpdateMprgWizard(hWiz);
			SetMprgWizardText(hWiz, (L"磁盘 " + to_wstring(devnum) + L" 中的卷已弹出。").c_str());
			MessageBoxTimeoutW(GetMprgHwnd(hWiz), L"磁盘中的卷已弹出。请注意，在某些系统上，该设备可能仍未自动断开电源，但通常此时可以安全移除设备。", L"成功", MB_ICONINFORMATION, 0, 5000);
			Sleep(2000);
			cleanup();
			return 0;
		}
	}

	wizdata->value = 30;
	UpdateMprgWizard(hWiz);
	SetMprgWizardText(hWiz, (L"尝试弹出磁盘 " + to_wstring(devnum) + L"... 失败。").c_str());
	Sleep(1000);

	SetMprgWizardText(hWiz, L"正在执行...");
	wizdata->value = 35;
	UpdateMprgWizard(hWiz);
	Sleep(1000);
	if (!SetDiskDriveConnectionStatus(devnum, false)) {
		DWORD err = GetLastError();
		SetMprgWizardText(hWiz, (L"发生错误。" + to_wstring(err) + L": " + ErrorCodeToString(err) + L" - Failed to set disk drive connection status").c_str());
		Sleep(5000);
		cleanup();
		return err;
	}
	else SetMprgWizardText(hWiz, (L"磁盘 " + to_wstring(devnum) + L" 已脱机。").c_str());
	wizdata->value = 45;
	UpdateMprgWizard(hWiz);
	Sleep(2000);

	wizdata->value = 50;
	UpdateMprgWizard(hWiz);
	SetMprgWizardText(hWiz, L"正在执行...");
	Sleep(1000);
	if (!SetDiskDriveConnectionStatus(devnum, true)) {
		DWORD err = GetLastError();
		SetMprgWizardText(hWiz, (L"发生错误。" + to_wstring(err) + L": " + ErrorCodeToString(err) + L" - Failed to set disk drive connection status").c_str());
		Sleep(5000);
		cleanup();
		return err;
	}
	else SetMprgWizardText(hWiz, (L"磁盘 " + to_wstring(devnum) + L" 已联机。").c_str());
	wizdata->value = 60;
	UpdateMprgWizard(hWiz);
	Sleep(2000);

	wizdata->value = 70;
	UpdateMprgWizard(hWiz);
	SetMprgWizardText(hWiz, (L"尝试弹出磁盘 " + to_wstring(devnum) + L"... (1st attempts)").c_str());
	if (EjectDevice(devnum)) goto directejectsuccess;
	Sleep(500);
	wizdata->value = 80;
	UpdateMprgWizard(hWiz);
	SetMprgWizardText(hWiz, (L"尝试弹出磁盘 " + to_wstring(devnum) + L"... (2nd attempts)").c_str());
	if (EjectDevice(devnum)) goto directejectsuccess;
	Sleep(500);

	wizdata->value = 99;
	UpdateMprgWizard(hWiz);
	SetMprgWizardText(hWiz, L"错误次数超限。");
	Sleep(2000);
	if (IDYES == MessageBoxTimeoutW(GetMprgHwnd(hWiz), L"无法弹出您的设备。\n您想要关闭 Windows 吗？", L"无法弹出设备", MB_ICONWARNING | MB_YESNO, 0, 10000)) {
		EnableAllPrivileges();
		// shutdown the windows
		WCHAR str[2]{};
		InitiateSystemShutdownExW(str, NULL, 0, FALSE, FALSE, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
	}

	cleanup();

	return 0;
}



#pragma comment(lib, "setupapi.lib")  
#pragma comment(lib, "cfgmgr32.lib") 



DEVINST GetDrivesDevInstByDiskNumber(long DiskNumber) {
	GUID* guid = (GUID*)(void*)&GUID_DEVINTERFACE_DISK;

	// Get device interface info set handle for all devices attached to system
	HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, NULL, NULL,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	// Retrieve a context structure for a device interface of a device
	// information set.
	DWORD dwIndex = 0;
	SP_DEVICE_INTERFACE_DATA devInterfaceData = { 0 };
	devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	BOOL bRet = FALSE;


	PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd;
	SP_DEVICE_INTERFACE_DATA spdid;
	SP_DEVINFO_DATA spdd;
	DWORD dwSize;

	spdid.cbSize = sizeof(spdid);

	while (true)
	{
		bRet = SetupDiEnumDeviceInterfaces(hDevInfo, NULL, guid, dwIndex,
			&devInterfaceData);
		if (!bRet)
		{
			break;
		}

		SetupDiEnumInterfaceDevice(hDevInfo, NULL, guid, dwIndex, &spdid);

		dwSize = 0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, NULL, 0, &dwSize, NULL);

		if (dwSize)
		{
			pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
			if (pspdidd == NULL)
			{
				continue; // autsch
			}
			pspdidd->cbSize = sizeof(*pspdidd);
			ZeroMemory((PVOID)&spdd, sizeof(spdd));
			spdd.cbSize = sizeof(spdd);


			long res = SetupDiGetDeviceInterfaceDetail(hDevInfo, &spdid, pspdidd, dwSize, &dwSize, &spdd);
			if (res)
			{
				HANDLE hDrive = CreateFile(pspdidd->DevicePath, 0,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
				if (hDrive != INVALID_HANDLE_VALUE)
				{
					STORAGE_DEVICE_NUMBER sdn;
					DWORD dwBytesReturned = 0;
					res = DeviceIoControl(hDrive, IOCTL_STORAGE_GET_DEVICE_NUMBER, NULL, 0, &sdn, sizeof(sdn), &dwBytesReturned, NULL);
					if (res)
					{
						if (DiskNumber == (long)sdn.DeviceNumber)
						{
							CloseHandle(hDrive);
							SetupDiDestroyDeviceInfoList(hDevInfo);
							return spdd.DevInst;
						}
					}
					CloseHandle(hDrive);
				}
			}
			HeapFree(GetProcessHeap(), 0, pspdidd);
		}
		dwIndex++;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return 0;
}

bool EjectRemovableDevice(wstring volume)
{
	DWORD accessMode = GENERIC_WRITE | GENERIC_READ;
	DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	HANDLE hDevice;
	long bResult = 0;
	DWORD retu = 0;
	DWORD dwBytesReturned;

	hDevice = CreateFileW(volume.c_str(), accessMode, shareMode, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		return false;
	}
#if 1
	//卸载U盘卷，不论是否在使用
	dwBytesReturned = 0;
	if (!DeviceIoControl(hDevice, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL)) {

	}

#endif

	dwBytesReturned = 0;
	PREVENT_MEDIA_REMOVAL PMRBuffer;
	PMRBuffer.PreventMediaRemoval = FALSE;
	if (!DeviceIoControl(hDevice, IOCTL_STORAGE_MEDIA_REMOVAL, &PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL), NULL, 0, &dwBytesReturned, NULL)) {
		printf("DeviceIoControl IOCTL_STORAGE_MEDIA_REMOVAL failed:%d\n", GetLastError());
	}
	bResult = DeviceIoControl(hDevice, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &retu, NULL);
	if (!bResult) {
		CloseHandle(hDevice);
		return false;
	}
	CloseHandle(hDevice);

	return true;
}

bool EjectDevice(DWORD DiskNumber)
{
	long bResult = 0;
	DWORD retu = 0;

	DEVINST DevInst = GetDrivesDevInstByDiskNumber(DiskNumber);
	if (!DevInst) return false;

	ULONG Status = 0;
	ULONG ProblemNumber = 0;
	PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown;
	wchar_t VetoName[MAX_PATH]{};
	bool bSuccess = false;

	long res = CM_Get_Parent(&DevInst, DevInst, 0); // disk's parent, e.g. the USB bridge, the SATA controller....
	res = CM_Get_DevNode_Status(&Status, &ProblemNumber, DevInst, 0);
	bool IsRemovable = ((Status & DN_REMOVABLE) != 0);
	
	{
		VetoName[0] = '\0';
		if (IsRemovable)
		{
			res = CM_Request_Device_Eject(DevInst, &VetoType, VetoName, MAX_PATH, 0);
		}
		else
		{
			res = CM_Query_And_Remove_SubTree(DevInst, &VetoType, VetoName, MAX_PATH, 0);
		}
		bSuccess = (res == CR_SUCCESS && VetoName[0] == '\0');
	}
	return bSuccess;
}

#pragma comment(lib, "wbemuuid.lib")

DWORD GetSystemDrivePhysicalDriveId() {
	// initialize WMI
	HRESULT hres;

	// 获取 WMI 服务
	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres)) {
		return -1; // 程序终止
	}

	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(
		BSTR(L"ROOT\\CIMV2"), // WMI 命名空间
		NULL,                    // 用户名
		NULL,                    // 密码
		0,                       // 区域设置
		NULL,                    // 安全标志
		0,                       // 权限
		0,                       // 上下文对象
		&pSvc                    // IWbemServices 指针
	);

	if (FAILED(hres)) {
		pLoc->Release();
		return -1; 
	}

	// 设置 WMI 服务的安全性
	hres = CoSetProxyBlanket(
		pSvc,                        // 代理
		RPC_C_AUTHN_WINNT,           // 身份验证服务
		RPC_C_AUTHZ_NONE,            // 授权服务
		NULL,                        // 服务器主体名称
		RPC_C_AUTHN_LEVEL_CALL,      // 身份验证级别
		RPC_C_IMP_LEVEL_IMPERSONATE, // 模拟级别
		NULL,                        // 客户端身份验证信息
		EOAC_NONE                    // 其他选项
	);

	if (FAILED(hres)) {
		std::cerr << "Could not set proxy blanket. Error code = 0x"
			<< std::hex << hres << std::endl;
		pSvc->Release();
		pLoc->Release();
		return -1; // 程序终止
	}

	// WMI 初始化成功
		// 执行 WMI 查询
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		BSTR(L"WQL"),
		BSTR(L"SELECT * FROM Win32_OperatingSystem"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		return -1; // 程序终止
	}

	// 获取查询结果
	IWbemClassObject* pclsObj = NULL;
	ULONG uReturn = 0;

	wstring systemDriveLetter;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		VariantInit(&vtProp);

		// 获取操作系统名称
		hr = pclsObj->Get(L"SystemDrive", 0, &vtProp, 0, 0);
		systemDriveLetter = vtProp.bstrVal;
		VariantClear(&vtProp);

		pclsObj->Release();

		break; // 只获取第一个操作系统
	}
	pEnumerator->Release();

	pEnumerator = NULL;
	wstring query2 = L"select * from Win32_Volume where DriveLetter=\"" + systemDriveLetter + L"\"";
	hres = pSvc->ExecQuery(
		BSTR(L"WQL"),
		BSTR(query2.c_str()),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres)) {
		pSvc->Release();
		pLoc->Release();
		return -1; // 程序终止
	}

	// 获取查询结果
	 pclsObj = NULL;
	 uReturn = 0;

	wstring volumePath;

	while (pEnumerator) {
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn) {
			break;
		}

		VARIANT vtProp;
		VariantInit(&vtProp);

		// 获取
		hr = pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0);
		volumePath = vtProp.bstrVal;
		VariantClear(&vtProp);

		pclsObj->Release();

		break; // 只获取第一个
	}
	pEnumerator->Release();

	volumePath.erase(volumePath.length() - 1);
	DWORD diskNumber = GetPhysicalDriveFromVolumeName(volumePath);

	// 释放资源
	pLoc->Release();
	pSvc->Release();

	return diskNumber;
}



