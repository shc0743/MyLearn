/*
Copied and modified from JiYuTrainer
https://github.com/imengyu/JiYuTrainer/blob/master/JiYuTrainer/DriverLoader.cpp

MIT License

Copyright (c) 2019 Mengyu (���) www.imengyu.top

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "DriverLoader.h"
#include "RegHlp.h"

//��������
//    lpszDriverName�������ķ�����
//    driverPath������������·��
//    lpszDisplayName��������ʾ����
bool MLoadKernelDriver(const wchar_t* lpszDriverName, const wchar_t* driverPath,
	const wchar_t* lpszDisplayName)
{
	//MessageBox(0, driverPath, L"driverPath", 0);
	wchar_t sDriverName[32];
	wcscpy_s(sDriverName, lpszDriverName);

	bool recreatee = false;

RECREATE:
	DWORD dwRtn = 0;
	bool bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	hServiceMgr = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL) {
		bRet = FALSE;
		goto BeforeLeave;
	}

	hServiceDDK = CreateServiceW(hServiceMgr, lpszDriverName, lpszDisplayName,
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE, driverPath, NULL, NULL, NULL, NULL, NULL);


	if (hServiceDDK == NULL) {
		dwRtn = GetLastError();
		//MessageBoxW(NULL, LastErrorStrW(), L"", 0);
		if (dwRtn == ERROR_SERVICE_MARKED_FOR_DELETE)
		{
			if (!recreatee) {
				recreatee = true;
				if (hServiceDDK) CloseServiceHandle(hServiceDDK);
				if (hServiceMgr) CloseServiceHandle(hServiceMgr);
				if (MRegForceDeleteServiceRegkey(sDriverName)) goto RECREATE;
			}
			bRet = false;
		}
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			bRet = FALSE;
			goto BeforeLeave;
		}
		hServiceDDK = OpenServiceW(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			dwRtn = GetLastError();
			//currentLogger->LogError2(L"Load driver error in OpenService : %d", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
	}
	bRet = StartServiceW(hServiceDDK, NULL, NULL);
	if (!bRet)
	{
		DWORD dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			//LogError2(L"Load driver error in StartService : %d", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		} else {
			if (dwRtn == ERROR_IO_PENDING) {
				bRet = FALSE;
			} else {
				bRet = TRUE;
			}
			goto BeforeLeave;
		}
	}
	bRet = TRUE;
	//�뿪ǰ�رվ��
BeforeLeave:
	if (hServiceDDK) CloseServiceHandle(hServiceDDK);
	if (hServiceMgr) CloseServiceHandle(hServiceMgr);
	return bRet;
}
//ж������
//    szSvrName��������
bool MUnLoadKernelDriver(const wchar_t* szSvrName) {
	//if (hKDrv && (wcscmp(szSvrName, L"JiYuTrainerDriver") == 0)) {
	//	CloseHandle(hKDrv);
	//	hKDrv = NULL;
	//}

	bool bDeleted = FALSE;
	bool bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	SERVICE_STATUS SvrSta;
	hServiceMgr = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL) {
		//LogError2(L"UnLoad driver error in OpenSCManager : %d", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	//����������Ӧ�ķ���
	hServiceDDK = OpenServiceW(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);
	if (hServiceDDK == NULL)
	{
		/*if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST)
			LogWarn(L"UnLoad driver error because driver not load.");
		else LogError2(L"UnLoad driver error in OpenService : %d", GetLastError());*/
		bRet = FALSE;
		goto BeforeLeave;
	}
	//ֹͣ�����������ֹͣʧ�ܣ�ֻ�������������ܣ��ٶ�̬���ء� 
	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta)) {
		//LogError2(L"UnLoad driver error in ControlService : %d", GetLastError());
	}
	//��̬ж���������� 
	if (!DeleteService(hServiceDDK)) {
		//LogError2(L"UnLoad driver error in DeleteService : %d", GetLastError());
		bRet = FALSE;
	}
	else bDeleted = TRUE;

BeforeLeave:
	//�뿪ǰ�رմ򿪵ľ��
	if (hServiceDDK) CloseServiceHandle(hServiceDDK);
	if (hServiceMgr) CloseServiceHandle(hServiceMgr);

	if (bDeleted) bRet = MRegForceDeleteServiceRegkey((LPWSTR)szSvrName);

	return bRet;
}
bool MUnLoadDriverServiceWithMessage(const wchar_t* szSvrName)
{
	bool bDeleted = FALSE;
	bool bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	SERVICE_STATUS SvrSta;
	DWORD lastErr = 0;

	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		lastErr = GetLastError();
		//FAST_STR_BINDER(str, L"ж���������󣬴������������%s\n
		// �볢���Թ���Ա������������", 128, SysHlp::ConvertErrorCodeToString(lastErr));
		//MessageBox(NULL, str, L"JiYuTrainer - ����", MB_ICONERROR);
		bRet = FALSE;
		goto BeforeLeave;
	}
	//����������Ӧ�ķ���
	hServiceDDK = OpenService(hServiceMgr, szSvrName, SERVICE_ALL_ACCESS);
	if (hServiceDDK == NULL)
	{/*
		lastErr = GetLastError();
		if (lastErr == ERROR_SERVICE_DOES_NOT_EXIST) 
			MessageBox(NULL, L"������ж�ز�ɾ�����벻Ҫ�ظ�����", L"", MB_ICONEXCLAMATION);
		else if ( lastErr == ERROR_SERVICE_MARKED_FOR_DELETE) 
			MessageBox(NULL, L"û������̨��������ҵ��ҵ������������������Ѿ���ж����", 
			L"JiYuTrainer - ��ʾ", MB_ICONEXCLAMATION);
		else {
			FAST_STR_BINDER(str, L"ж���������󣬴���������%s", 128, 
			SysHlp::ConvertErrorCodeToString(lastErr));
			MessageBox(NULL, str, L"JiYuTrainer - ����", MB_ICONERROR);
		}*/
		bRet = FALSE;
		goto BeforeLeave;
	}
	//ֹͣ�����������ֹͣʧ�ܣ�ֻ�������������ܣ��ٶ�̬���ء� 
	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &SvrSta)) {
		lastErr = GetLastError();
		if (lastErr == ERROR_SERVICE_MARKED_FOR_DELETE) {
			//������ж�ز�ɾ�����벻Ҫ�ظ�����
			bRet = FALSE;
			goto BeforeLeave;
		}
		else {
			//FAST_STR_BINDER(str, L"ж����������ֹͣ����ʧ�ܣ�%s", 128, );
			//MessageBox(NULL, str, L"JiYuTrainer - ����", MB_ICONERROR);
		}
	}
	//��̬ж���������� 
	if (!DeleteService(hServiceDDK)) {
		//lastErr = GetLastError();
		//if (lastErr == ERROR_SERVICE_MARKED_FOR_DELETE) 
		//	MessageBox(NULL, L"������ж�ز�ɾ�����벻Ҫ�ظ�����",
		//  L"JiYuTrainer - ��ʾ", MB_ICONEXCLAMATION);
		//else {
		//	//FAST_STR_BINDER(str, L"ж����������ɾ����������%s", 128, );
		//	//MessageBox(NULL, str, L"JiYuTrainer - ����", MB_ICONERROR);
			bRet = FALSE;
		//}
	}
	else bDeleted = TRUE;
BeforeLeave:
	//�뿪ǰ�رմ򿪵ľ��
	if (hServiceDDK) CloseServiceHandle(hServiceDDK);
	if (hServiceMgr) CloseServiceHandle(hServiceMgr);

	if (bDeleted) bRet = MRegForceDeleteServiceRegkey((LPWSTR)szSvrName);

	return bRet;
}
//������
HANDLE MOpenDriver(LPCWSTR szDrvName)
{
	HANDLE hKDrv = NULL;
	hKDrv = CreateFileW(szDrvName,
		GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hKDrv || hKDrv == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	return hKDrv;
}

