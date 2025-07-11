/*
Copyright 2020-2025 shc0743<github.com/shc0743>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
						   o8888888o
						   88" . "88
						   (| -_- |)
							O\ = /O
						 ___/'---'\___
						 .' \\| |// '.
					   / \\||| : |||// \
					 / _||||| -:- |||||_ \
					   | | \\\ - /// | |
					 | |_| ''\---/'' |_| |
					  \ .-\__ '-' __/-. /
				   ___'. .' /--.--\ '. .'___
				."" '< '.___\_<|>_/___.' >'"".
			   | | : '- \'.:'\ _ /':.'/ -' : | |
				 \ \ '-. \_ __\ /__ _/ .-' / /
		 ******'-.____'-.___\_____/___.-'____.-'******
							'*---*'
							
		 ---------------------------------------------
				  佛祖保佑            永无BUG


   佛曰：
		 写字楼里写字间，写字间里程序员；
		 程序人员写程序，又拿程序换酒钱。
		 酒醒只在网上坐，酒醉还来网下眠；
		 酒醉酒醒日复日，网上网下年复年。
		 但愿老死电脑间，不愿鞠躬老板前；
		 奔驰宝马贵者趣，公交单车屌丝缘。
		 若将贵者比屌丝，一在平地一在天。
		 若将程序比车马，我何碌碌彼何闲。
		 别人笑我忒疯癫，我笑自己命太贱；
		 不见满街漂亮妹，哪个归得程序员？

		写字楼里写字间，写字间里程序员；程序人员写程序，又拿程序换酒钱。
		酒醒只在网上坐，酒醉还来网下眠；酒醉酒醒日复日，网上网下年复年。
		宁愿老死程序间，只要老板多发钱；小车大房不去想，撰个2k好过年。
		若要见识新世面，公务员比程序员；一个在天一在地，而且还比我们闲。
		别人看我穿白领，我看别人穿名牌；天生我才写程序，臀大近视肩周炎。

		年复一年春光度,度得他人做老板;老板扣我薄酒钱,没有酒钱怎过年;.
		春光逝去皱纹起,作起程序也委靡;来到水源把水灌,打死不做程序员.
		别人笑我忒疯癫，我笑他人命太贱；状元三百六十行，偏偏来做程序员！！
		但愿老死电脑间，不愿鞠躬老板前；奔驰宝马贵者趣，公交自行程序员。
		若将程员比妓女，一在平地一在天；若将程员比车马，他得驱驰我无闲。
		别人笑我忒疯癫，我笑自己命太贱；不见满街漂亮妹，哪个归得程序员。
		不想只挣打工钱，那个老板愿发钱；小车大房咱要想，任我享用多悠闲。
		比尔能搞个微软，我咋不能捞点钱；一个在天一在地，定有一日乾坤翻。
		我在天来他在地，纵横天下山水间；傲视武林豪杰墓，一樽还垒风月山。
		电脑面前眼发直，眼镜下面泪茫茫；做梦发财好几亿，从此不用手指忙。
		哪知梦醒手空空，老板看到把我训；待到老时眼发花，走路不知哪是家。
		各位在此穷抱怨，可知小弟更可怜；俺学编程几年半，至今没赚一分钱。
		听说三十是末日，二十三岁在眼前；发誓立志傍微软，渺渺前程对谁言？
		小农村里小民房，小民房里小民工；小民工人写程序，又拿代码讨赏钱。
		钱空只在代码中，钱醉仍在代码间；有钱无钱日复日，码上码下年复年。
		但愿老死代码间，不愿鞠躬奥迪前，奥迪奔驰贵者趣，程序代码贫者缘。
		若将贫贱比贫者，一在平地一在天；若将贫贱比车马，他得驱驰我得闲。
		别人笑我忒疯癫，我笑他人看不穿；不见盖茨两手间，财权富贵世人鉴。

		-- https://bbs.csdn.net/topics/240010413
*/
#ifndef __cplusplus
#error "tool.cpp is a C++ source file"
#endif
#include "tool.h"
using namespace std;

#pragma region Link to libraries
#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")

#ifdef _WIN32
#pragma comment(lib,"Rpcrt4.lib")
#endif

#pragma endregion


string& str_replace(string& strBase, const string strSrc, const string strDes){
	string::size_type pos = 0;
	string::size_type srcLen = strSrc.size();
	string::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while ((pos != string::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, (pos + desLen));
	}
	return strBase;
}
wstring& str_replace(std::wstring& strBase,
	const std::wstring strSrc,const std::wstring strDes){
	wstring::size_type pos = 0;
	wstring::size_type srcLen = strSrc.size();
	wstring::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while ((pos != wstring::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, (pos + desLen));
	}
	return strBase;
}
std::vector<std::string>& str_split(
	const std::string& src, 
	const std::string separator, 
	std::vector<std::string>& dest) //字符串分割到数组
{

	//参数1：要分割的字符串；参数2：作为分隔符的字符；参数3：存放分割后的字符串的vector向量

	string str = src;
	string substring;
	string::size_type start = 0, index;
	dest.clear();
	index = str.find_first_of(separator, start);
	do
	{
		if (index != string::npos)
		{
			substring = str.substr(start, index - start);
			dest.push_back(substring);
			start = index + separator.size();
			index = str.find(separator, start);
			if (start == string::npos) break;
		}
	} while (index != string::npos);

	//the last part
	substring = str.substr(start);
	dest.push_back(substring);
	return dest;
}
std::vector<std::wstring>& str_split(
	const std::wstring& src, 
	const std::wstring separator, 
	std::vector<std::wstring>& dest)
{
	wstring str = src;
	wstring substring;
	wstring::size_type start = 0, index;
	dest.clear();
	index = str.find_first_of(separator, start);
	do
	{
		if (index != wstring::npos)
		{
			substring = str.substr(start, index - start);
			dest.push_back(substring);
			start = index + separator.size();
			index = str.find(separator, start);
			if (start == wstring::npos) break;
		}
	} while (index != wstring::npos);

	//the last part
	substring = str.substr(start);
	dest.push_back(substring);
	return dest;
}
string ws2s(const wstring wstr) {
	string result;
	size_t len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
		(int)(wstr.size()), NULL, 0, NULL, NULL);
	if (len < 0) return result;
	char* buffer = new char[len + 1];
	if (buffer == NULL) return result;
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), 
		(int)(wstr.size()), buffer, (int)len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}
wstring s2ws(const string str) {
	wstring result;
	size_t len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), 
		(int)(str.size()), NULL, 0);
	if (len < 0) return result;
	wchar_t* buffer = new wchar_t[len + 1];
	if (buffer == NULL) return result;
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)(str.size()), 
		buffer, (int)len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

bool IsRunAsAdmin() //判断是否管理员模式
{
	BOOL bElevated = false;
	HANDLE hToken = NULL;
	// Get current process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return false;
	TOKEN_ELEVATION tokenEle = {0};
	DWORD dwRetLen = 0;
	// Retrieve token elevation information
	if (GetTokenInformation(hToken, TokenElevation,
		&tokenEle, sizeof(tokenEle), &dwRetLen))
	{
		if (dwRetLen == sizeof(tokenEle))
		{
			bElevated = tokenEle.TokenIsElevated;
		}
	}
	CloseHandle(hToken);
	return bElevated;
}

string GetProgramDir() {
	WCHAR exeFullPath[MAX_PATH];
	GetModuleFileNameW(NULL, exeFullPath, MAX_PATH);
	return ws2s(exeFullPath);
}
string GetProgramPath() {
	WCHAR exeFullPath[MAX_PATH];
	GetModuleFileNameW(NULL, exeFullPath, MAX_PATH);
	string s = ws2s(exeFullPath);
	s = s.substr(0, s.find_last_of("\\") + 1);
	return s;
}
wstring GetProgramDirW() {
	WCHAR exeFullPath[MAX_PATH];
	GetModuleFileNameW(NULL, exeFullPath, MAX_PATH);
	return (exeFullPath);
}
wstring GetProgramPathW() {
	WCHAR exeFullPath[MAX_PATH];
	GetModuleFileNameW(NULL, exeFullPath, MAX_PATH);
	wstring s = (exeFullPath);
	s = s.substr(0, s.find_last_of(L"\\") + 1);
	return s;
}

ProgramInfo GetProgramInfo(){
	ProgramInfo inf;
	inf.dir = ::GetProgramDir();
	inf.path = inf.dir;
	inf.path = inf.path.substr(0, inf.path.find_last_of("\\") + 1);
	inf.name = inf.dir;
	inf.name.erase(0,inf.name.find_last_of("\\")+1);
	return inf;
}

#ifdef _WIN32
bool FreeResFile(
	DWORD dwResName, wstring lpResType, wstring lpFilePathName, HMODULE hInst
) {
	HMODULE hInstance = hInst ? hInst : GetModuleHandle(NULL);//得到自身实例句柄  
	HRSRC hResID = ::FindResourceW(hInstance,
		MAKEINTRESOURCEW(dwResName), lpResType.c_str());//查找资源  
	if (!hResID) return false;
	HGLOBAL hRes = ::LoadResource(hInstance, hResID);//加载资源  
	if (!hRes) return false;
	LPVOID pRes = ::LockResource(hRes);//锁定资源  

	if (pRes == NULL)//锁定失败  
	{
		return false;
	}
	DWORD dwResSize = ::SizeofResource(hInstance, hResID);//得到待释放文件大小  
	HANDLE hResFile = CreateFileW(
		lpFilePathName.c_str(), 
		GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);//创建文件  

	if (!hResFile || INVALID_HANDLE_VALUE == hResFile) {
		return false;
	}

	DWORD dwWritten = 0;//写入文件的大小     
	WriteFile(hResFile, pRes, dwResSize, &dwWritten, NULL);//写入文件  
	CloseHandle(hResFile);//关闭文件句柄  

	return (dwResSize == dwWritten);//若写入大小等于文件大小返回成功否则失败  
}
#endif

#if 0
bool CheckSelfProcessExists() {
	int i = 0;
	PROCESSENTRY32 pe32; AutoZeroMemory(pe32);
	pe32.dwSize = sizeof(pe32);
	string SelfProcessName = GetProgramDir();
	SelfProcessName = SelfProcessName.erase(0, 
		SelfProcessName.find_last_of("\\") + 1);
#ifdef UNICODE
	wstring PROCName_s = s2ws(SelfProcessName);
	LPCTSTR PROCName = PROCName_s.c_str();
#else
	LPCTSTR PROCName = SelfProcessName.c_str();
#endif
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		i += 0;
	}
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		//printf(" 进程名称：%s \n", pe32.szExeFile);
		if (STRCMP(PROCName, pe32.szExeFile) == 0)
		{
			//printf("进程运行中");
			i += 1;
		}
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	if (i > 1) {           //大于1，排除自身
		return true;
	}
	else {
		return false;
	}
}

size_t CheckProcessCount(TSTRING pname){
	size_t i = 0;
	PROCESSENTRY32 pe32 = {0};
	pe32.dwSize = sizeof(pe32);
	LPCTSTR PROCName = pname.c_str();
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return 0;
	}
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore) {
		if (STRCMP(PROCName, pe32.szExeFile) == 0) i++;
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	return i;
}
#endif



HANDLE GetProcessHandle(DWORD nID){
	return OpenProcess(PROCESS_ALL_ACCESS, false, nID);
}
HANDLE GetProcessHandle(LPCTSTR lpName){
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	Process32First(hSnapshot, &pe);
	do {
		if (!_tcsicmp(pe.szExeFile, lpName)) { // 不区分大小写
			CloseHandle(hSnapshot);
			return GetProcessHandle(pe.th32ProcessID);
		}
	} while (Process32Next(hSnapshot, &pe));
	CloseHandle(hSnapshot);
	return NULL;
}

DWORD GetProcessIdW(wstring lpName){
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
#ifdef UNICODE
		if (!_wcsicmp(pe.szExeFile, lpName.c_str())) // 不区分大小写
#else
		if (!_stricmp(pe.szExeFile, ws2c(lpName))) // 不区分大小写
#endif
		{
			CloseHandle(hSnapshot);
			return (pe.th32ProcessID);
		}
	}
	return NULL;
}
vector<DWORD>* GetProcessIdFromAll(LPCTSTR lpName) {
	vector<DWORD>* v = new vector<DWORD>;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return v;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		if (!_tcsicmp(pe.szExeFile, lpName)) // 不区分大小写
		{
			v->push_back(pe.th32ProcessID);
		}
	}
	CloseHandle(hSnapshot);
	return v;
}

BOOL EnableDebugPrivilege() {
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;
	AutoZeroMemory(tkp);
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue)) {
		__try { if (hToken) CloseHandle(hToken); }
		__except (EXCEPTION_EXECUTE_HANDLER) {};
		return FALSE;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {
		__try {	if (hToken) CloseHandle(hToken); }
		__except (EXCEPTION_EXECUTE_HANDLER) {};
		return FALSE;
	}
	__try {	if (hToken) CloseHandle(hToken); }
	__except (EXCEPTION_EXECUTE_HANDLER) {};
	return TRUE;
}

BOOL EnableAllPrivileges() {
	HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
	if (!ntdll) return FALSE;
	typedef NTSTATUS(WINAPI* RtlAdjustPrivilege_t)
		(ULONG Privilege, BOOL Enable, BOOL CurrentThread, PBOOL Enabled);
	RtlAdjustPrivilege_t RtlAdjustPrivilege = (RtlAdjustPrivilege_t)
		GetProcAddress(ntdll, "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege) return FALSE;
	BOOL bPrev = FALSE, bOk = TRUE;
	for (ULONG i = 1; i < 37; ++i)
		bOk &= NT_SUCCESS(RtlAdjustPrivilege(i, TRUE, FALSE, &bPrev));
	return bOk;
}

//void StartExecute(const TSTRING& paras) {
//	//初始化shellexe信息
//	SHELLEXECUTEINFO ExeInfo;
//	ZeroMemory(&ExeInfo, sizeof(SHELLEXECUTEINFO));
//	ExeInfo.cbSize = sizeof(SHELLEXECUTEINFO);
//	ExeInfo.lpFile = _C_AUTOTEXT("explorer.exe");
//	ExeInfo.lpParameters = (paras).c_str();
//	ExeInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
//	ExeInfo.nShow = SW_HIDE;
//	ExeInfo.hwnd = NULL;
//	ExeInfo.lpVerb = NULL;
//	ExeInfo.lpDirectory = NULL;
//	ExeInfo.hInstApp = NULL;
//	//执行命令
//	ShellExecuteEx(&ExeInfo);
//	//等待进程结束
//	WaitForSingleObject(ExeInfo.hProcess, INFINITE);
//}

template<typename str, typename fs>
wstring _TempFile<str,fs>::GetRdmFileNameW() {
	wstring rdmfn = _T("");
	BASSTRING nlist[] = _T(
		codetostr(0123456789abcdef)
		codetostr(ghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ)
		"+-_=@");
	srand((UINT)time(0)); 
#ifdef UNICODE
	auto len = lstrlenW(nlist) - 1;
#else
	auto len = strlen(nlist) - 1;
#endif
	for (int i = 0; i < (int)16; ++i) {
		rdmfn += nlist[(time(0) % rand()) % len];
	}
	return rdmfn;
}
template<typename str, typename fs>
string _TempFile<str,fs>::GetRdmFileNameA(){
	string rdmfn = ("");
	char nlist[] = (
		codetostr(0123456789abcdef)
		codetostr(ghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ)
		"+-_=@");
	srand((UINT)time(0)); auto len = strlen(nlist) - 1;
	for (int i = 0; i < (int)16; ++i) {
		rdmfn += nlist[(time(0) % rand()) % len];
	}
	return rdmfn;
}

template<typename str, typename fs>
_TempFile<str,fs>::_TempFile(){
	NoRemove = false;
}

template<typename str, typename fs>
_TempFile<str,fs>::_TempFile(str fileName){
	NoRemove = false;
	this->open(fileName);
}

template<typename str, typename fs>
_TempFile<str,fs>::~_TempFile(){
	fs::close();
}

template<typename str, typename fs>
void _TempFile<str,fs>::open(str fileName){
	this->FilePathName = fileName;
	fs::open(fileName, ios::in | ios::out | ios::trunc | ios::binary);
}

template<typename str, typename fs>
str _TempFile<str,fs>::GetFilePathName(){
	return this->FilePathName;
}

int ExecBatch(TSTRING Batch){
	vector<TSTRING> str;
	str.push_back(Batch);
	return ExecBatch(str);
}

int ExecBatch(vector<TSTRING>& Batch){
	TempFile fp;
#ifdef UNICODE
	fp.open(fp.GetRdmFileNameW() + L"bat");
#else
	fp.open(fp.GetRdmFileNameA() + "bat");
#endif

	fp << _T("@echo off") << endl;

	for (auto& i : Batch) {
		fp << i << endl;
	}
	fp << _T("del /f /s /q ") << fp.GetFilePathName() << endl;

	fp.close();

	TSTRING sFiPn = fp.GetFilePathName();

	int nRes = 0;

#if (!(defined(__NO_UNICODE__)))&&defined(UNICODE)
	nRes = system((""+ws2s(sFiPn)).c_str());
#else
	nRes = system((""+sFiPn).c_str());
#endif

	//::SetCurrentDirectory(nWorkPath.c_str());

	return nRes;
}


#if 0
StringEx& StringEx::operator=(StringEx& argv){
	*this = (StringEx)argv; return *this;
}
StringEx& StringEx::operator=(wstring& argv){
	*this = (StringEx)argv; return *this;
}
StringEx& StringEx::operator=(const char* argv){
	*this = (StringEx)argv; return *this;
}

StringEx StringEx::operator+(string& argv){
	string str = (string)(*this);
	return str + argv;
}

StringEx StringEx::operator+(StringEx& argv){
	string str = (string)(*this);
	return str + (string)argv;
}

StringEx StringEx::operator+(wstring& argv){
	string str = (string)(*this);
	return str + ws2s(argv);
}
#endif


#if 0
void CTrayIcon::Create(TSTRING IconText,HWND hwnd){
	m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd = hwnd;
	m_nid.uID = 128;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_USER+1;

	//自定义的消息名称 WM_SHOWTASK 头函数中定义为WM_USER+1
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(128));
	strcpyt(m_nid.szTip, IconText.c_str());//鼠标放在上面显示的内容
	Shell_NotifyIcon(NIM_ADD, &m_nid);//在托盘区添加图标
}
#endif

#if 0
ServiceManager_ ServiceManager;

bool ServiceManager_::InitSch() {
	// Get a handle to the SCM database.
	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database
		SC_MANAGER_ALL_ACCESS);  // full access rights
	if (NULL == schSCManager)
	{
		return false;
	}
	return true;
}

bool ServiceManager_::FreeSch() {
	if (!schSCManager) return false;
	if (CloseServiceHandle(schSCManager)) {
		schSCManager = NULL;
		return true;
	}
	else return false;
}

unsigned ServiceManager_::Start(string ServiceName) {
	if (!InitSch()) return ERROR_OPEN_SCMANAGER;
	SC_HANDLE schService;
	// Get a handle to the service.
	schService = OpenServiceW(
		schSCManager,              // SCM database
		s2ws(ServiceName).c_str(), // name of service        
		SERVICE_START 
	);  
	if (schService == NULL) {
		FreeSch();
		return ERROR_OPEN_SERVICE;
	}
	unsigned ret = OK;
	if (!::StartServiceA(schService, 0, NULL)) {
		ret = ERROR_START_SERVICE;
	}
	::CloseServiceHandle(schService);
	FreeSch();
	return ret;
}

unsigned ServiceManager_::ReStart(string ServiceName) {
	Stop (ServiceName);
	Start(ServiceName);
	return OK;
}

unsigned ServiceManager_::Stop(string ServiceName) {
	if (!InitSch()) return ERROR_OPEN_SCMANAGER;
	SC_HANDLE schService;
	// Get a handle to the service.
	schService = OpenServiceW(
		schSCManager,              // SCM database
		s2ws(ServiceName).c_str(), // name of service        
		SERVICE_STOP
	);  
	if (schService == NULL) {
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_OPEN_SERVICE;
	}
	unsigned ret = OK;
	SERVICE_STATUS stat;
	if (!::ControlService(schService, SERVICE_CONTROL_STOP, &stat)) {
		ret = ERROR_STOP_SERVICE;
	}
	::WaitForSingleObject(schService, MAXDWORD);
	::CloseServiceHandle(schService);
	if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
	return ret;
}

unsigned ServiceManager_::Query(LPCTSTR ServiceName) {
	TCHAR* szSvcName = const_cast<TCHAR*>(ServiceName);

	SERVICE_STATUS_PROCESS ssStatus = {0};
	//DWORD dwOldCheckPoint;

	//DWORD dwStartTickCount;
	//DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	if (!InitSch()) return ERROR_OPEN_SCMANAGER;
	SC_HANDLE schService;
	// Get a handle to the service.
	schService = OpenService(
		schSCManager,         // SCM database
		szSvcName,            // name of service        
		SERVICE_QUERY_STATUS |
		SERVICE_ENUMERATE_DEPENDENTS
	);  // full access
	if (schService == NULL)
	{
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_OPEN_SERVICE;
	}

	// Check the status in case the service is not stopped.
	if (!QueryServiceStatusEx(
		schService,                     // handle to service
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              
	{
		CloseServiceHandle(schService);
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_QUERY_SERVICESTATUS;
	}
	else {
		unsigned return_val = ERROR_UNKNOWN;
		switch (ssStatus.dwCurrentState)
		{
		case SERVICE_STOPPED:
			return_val = STATUS_STOP; break;
		case SERVICE_STOP_PENDING:
			return_val = STATUS_STOPPING; break;
		case SERVICE_PAUSED:
			return_val = STATUS_PAUSE; break;
		case SERVICE_PAUSE_PENDING:
			return_val = STATUS_PAUSE_PENDING; break;
		case SERVICE_CONTINUE_PENDING:
			return_val = STATUS_CONTINUE_PENDING; break;
		case SERVICE_RUNNING:
			return_val = STATUS_START; break;
		case SERVICE_START_PENDING:
			return_val = STATUS_STARTING; break;
		default:
			return_val = ERROR_UNKNOWN;
		}
		CloseServiceHandle(schService);
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return return_val;
	}
#pragma warning(push)
#pragma warning(disable: 4702)
	return ERROR_UNKNOWN;
#pragma warning(pop)
}

unsigned ServiceManager_::Pause(string ServiceName) {
	if (!InitSch()) return ERROR_OPEN_SCMANAGER;
	SC_HANDLE schService;
	// Get a handle to the service.
	schService = OpenServiceW(
		schSCManager,              // SCM database
		s2ws(ServiceName).c_str(), // name of service        
		SERVICE_PAUSE_CONTINUE 
	);  
	if (schService == NULL) {
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_OPEN_SERVICE;
	}
	unsigned ret = OK;
	SERVICE_STATUS sss;
	if (!::ControlService(schService, SERVICE_CONTROL_PAUSE, &sss)) {
		ret = ERROR_PAUSE_SERVICE;
	}
	::CloseServiceHandle(schService);
	if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
	return ret;
}

unsigned ServiceManager_::Continue(string ServiceName) {
	if (!InitSch()) return ERROR_OPEN_SCMANAGER;
	SC_HANDLE schService;
	// Get a handle to the service.
	schService = OpenServiceW(
		schSCManager,              // SCM database
		s2ws(ServiceName).c_str(), // name of service        
		SERVICE_PAUSE_CONTINUE 
	);  
	if (schService == NULL) {
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_OPEN_SERVICE;
	}
	unsigned ret = OK;
	if (!::StartServiceA(schService, SERVICE_CONTROL_CONTINUE, NULL)) {
		ret = ERROR_CONTINUE_SERVICE;
	}
	::CloseServiceHandle(schService);
	if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
	return ret;
}

#pragma warning(push)
#pragma warning(disable: 26812)
unsigned ServiceManager_::New(string name, string bin, STARTUP_TYPE stype,
	string display, string des, DWORD type) {
#pragma warning(pop)
	if (stype == Undefined) return ERROR_UNDEFINED_SCOPE;
	if (!InitSch()) return ERROR_OPEN_SCMANAGER;

	if (display.empty()) display = name;
	SC_HANDLE svc = CreateServiceA(schSCManager, name.c_str(), display.c_str(),
		SERVICE_ALL_ACCESS,	type, stype, SERVICE_ERROR_NORMAL, bin.c_str(),
		NULL, NULL, NULL, NULL, NULL);
	if (!svc) {
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_NEW_SERVICE;
	}

	if (!des.empty()) {
		SERVICE_DESCRIPTIONA a; AutoZeroMemory(a);
		LPSTR des2 = (LPSTR)calloc(sizeof(char), des.length() + 1);
		if (des2) {
			strcpy_s(des2, des.length() + 1, des.c_str());
			a.lpDescription = des2;
			ChangeServiceConfig2A(svc, SERVICE_CONFIG_DESCRIPTION, &a);
		}
	}

	CloseServiceHandle(svc);
	if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
	return OK;
}
#pragma warning(push)
#pragma warning(disable: 26812)
unsigned ServiceManager_::New(wstring name, wstring bin, STARTUP_TYPE stype,
	wstring display, wstring des, DWORD type) {
#pragma warning(pop)
	if (stype == Undefined) return ERROR_UNDEFINED_SCOPE;
	if (!InitSch()) return ERROR_OPEN_SCMANAGER;

	if (display.empty()) display = name;
	DWORD err = 0;
	SC_HANDLE svc = CreateServiceW(schSCManager, name.c_str(), display.c_str(),
		SERVICE_ALL_ACCESS,	type, stype, SERVICE_ERROR_NORMAL, bin.c_str(),
		NULL, NULL, NULL, NULL, NULL);
	if (!svc) {
		err = GetLastError();
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		SetLastError(err);
		return ERROR_NEW_SERVICE;
	}

	if (!des.empty()) {
		SERVICE_DESCRIPTIONW a; AutoZeroMemory(a);
		LPWSTR des2 = (LPWSTR)calloc(sizeof(WCHAR), des.length() + 1);
		if (des2) {
			wcscpy_s(des2, des.length() + 1, des.c_str());
			a.lpDescription = des2;
			ChangeServiceConfig2W(svc, SERVICE_CONFIG_DESCRIPTION, &a);
		}
	}

	CloseServiceHandle(svc);
	if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
	return OK;
}

unsigned ServiceManager_::Remove(string name) {
	if (!InitSch()) return ERROR_OPEN_SCMANAGER;

	SC_HANDLE schService;
	// Get a handle to the service.
	schService = OpenServiceW(
		schSCManager,         // SCM database
		s2ws(name).c_str(),   // name of service        
		SERVICE_ALL_ACCESS 
	);  
	if (schService == NULL) {
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_OPEN_SERVICE;
	}
	if (!DeleteService(schService)) {
		CloseServiceHandle(schService);
		if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
		return ERROR_DEL_SERVICE;
	}
	CloseServiceHandle(schService);

	if (!FreeSch()) return ERROR_CLOSE_SCMANAGER;
	return OK;
}
#endif

DWORD Process_t::GetParentProcessId(DWORD dwProcessId) {
	HANDLE hsnap/*hsapnap*/ = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hsnap || INVALID_HANDLE_VALUE == hsnap) return 0;
	PROCESSENTRY32 pe = {0};
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL b = ::Process32First(hsnap, &pe);
	DWORD dwId = 0;
	while (b) {
		if (pe.th32ProcessID == dwProcessId) {
			dwId = pe.th32ParentProcessID;
			break;
		}
		b = ::Process32Next(hsnap, &pe);
	}
	CloseHandle(hsnap);
	return dwId;
}

TSTRING Process_t::GetProcessNameById(DWORD dwProcessId) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (!hSnapshot) hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32, dwProcessId);
	if (hSnapshot) {
		MODULEENTRY32 pe32; AutoZeroMemory(pe32);
		pe32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &pe32)) {
			CloseHandle(hSnapshot);
			return pe32.szModule;
		}
		CloseHandle(hSnapshot);
	}
	for (auto& i : prs) {
		if (i.id() == dwProcessId) return i.name();
	}
	return _T("");
}

TSTRING Process_t::GetProcessFullPathById(DWORD dwProcessId) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
	if (hProcess) {
		TCHAR buffer[2048] = { 0 }; DWORD size = 2047;
		QueryFullProcessImageName(hProcess, 0, buffer, &size);
		CloseHandle(hProcess);
		return buffer;
	}
	return _T("");
}

void Process_t::checkFlushTime() {
	if (_time64(0) - LastFlush >= 1024) flush();
}

bool Process_t::flush(void) {
	LastFlush = _time64(0);
	HANDLE hsnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hsnap) return false;
	for(auto& i : this->prs){
		if (i.handle) ::CloseHandle(i.handle);
	}
	this->prs.clear();
	PROCESSENTRY32 pe = {0};
	pe.dwSize = sizeof(PROCESSENTRY32);
	int b = ::Process32First(hsnap, &pe);
	while (b) {
		prs.push_back(ProcessInfo(
			pe.szExeFile,
			pe.th32ProcessID,
			pe.th32ParentProcessID,
			NULL));
		b = ::Process32Next(hsnap, &pe);
	}
	return ::CloseHandle(hsnap);
}

void Process_t::close() {
	for (auto& i : prs) ::CloseHandle(i);
}

void Process_t::find(vector<Process_t::ProcessInfo>& dest) {
	checkFlushTime();
	dest.clear();
	dest = this->prs;
}

void Process_t::find(TSTRING pname, vector<Process_t::ProcessInfo>& dest) {
	checkFlushTime();
	dest.clear();
	for (auto& i : prs) {
#ifdef _DEBUG
		bool cmpres = !ua_lstrcmpi(pname.c_str(), i.m_name.c_str());
		if (cmpres) dest.push_back(i);
#else
		if (!ua_lstrcmpi(pname.c_str(), i.m_name.c_str())) dest.push_back(i);
#endif
	}
}

Process_t::ProcessInfo Process_t::find(DWORD pid) {
	checkFlushTime();
	for (auto& i : prs) {
		if (pid == i.m_id) {
			return i; 
		}
	}
	return ProcessInfo();
}

HANDLE Process_t::ProcessInfo::Handle() {
	if (!this->handle) {
		handle = (HANDLE)::OpenProcess(PROCESS_ALL_ACCESS, false, this->m_id);
	}
	return this->handle;
}

Process_t::ProcessInfo Process_t::GetCurrentProcess(void) {
	return Process_t::ProcessInfo(
		to__str(GetProgramInfo().name),
		GetCurrentProcessId(),
		GetParentProcessId(GetCurrentProcessId()),
		::OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId())
	);
}

void Process_t::findWindows(ProcessInfo process, vector<HWND>& dest) {
	HWND hWnd = GetTopWindow(NULL); if (!hWnd) return; do {
		DWORD dwProcessId = 0;
		GetWindowThreadProcessId(hWnd, &dwProcessId);
		if (dwProcessId == process) dest.push_back(hWnd);
	} while ((hWnd = GetWindow(hWnd, GW_HWNDNEXT)) != NULL); // C4706
}

NTSTATUS Process_t::NtOpenProcess(PHANDLE ProcessHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID ClientId)
{
	//HMODULE _ntdll = GetModuleHandle(_T("ntdll.dll"));
	//if (!_ntdll) return -1;
	typedef NTSTATUS(__stdcall* _NtOpenProcess_t)
		(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PCLIENT_ID);
	_NtOpenProcess_t NtOpenProcess = 
		(_NtOpenProcess_t)GetProcAddress(ntdll, "NtOpenProcess");
	if (!NtOpenProcess) return -1;
	return NtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
}

NTSTATUS Process_t::NtClose(HANDLE Handle) {
	//HMODULE _ntdll = GetModuleHandle(_T("ntdll.dll"));
	//if (!_ntdll) return -1;
	typedef NTSTATUS(__stdcall* _NtClose_t)(HANDLE);
	_NtClose_t NtClose = (_NtClose_t)GetProcAddress(ntdll, "NtClose");
	if (!NtClose) return -1;
	return NtClose(Handle);
}

bool Process_t::kill(const Process_t::ProcessInfo& exe, UINT exitCode) {
	if (exe.m_id != current_process_id) suspend(exe);
	_NtTerminateProcess NtTerminateProcess = reinterpret_cast<_NtTerminateProcess>
		(GetProcAddress(ntdll, "NtTerminateProcess"));
	if(!NtTerminateProcess) return false;
	NTSTATUS dwStat = 0;
	if ((dwStat = NtTerminateProcess(exe, exitCode)) == 0) return true;
	SetLastError(dwStat);
	// try to crash it
	//crash(exe);
	// cannot kill
	return false;
}

bool Process_t::killall(TSTRING name, UINT code) {
	vector<ProcessInfo> pis;
	find(name, pis);
	if(pis.empty()) return false;
	bool _ = true;
	for (auto& i : pis) {
		_ &= kill(i, code);
	}
	return _;
}

#pragma warning(push)
#pragma warning(disable: 6335)
PROCESS_INFORMATION Process_t::Start(const TSTRING& paras, STARTUPINFO& si) {
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	LPTSTR cl = (LPTSTR)calloc(paras.length() + 1, sizeof(TCHAR));
	if (!cl) return PROCESS_INFORMATION{ 0,0,0,0 };
#ifdef UNICODE
	wcscpy_s(cl, paras.length() + 1, paras.c_str());
#else
	strcpy_s(cl, paras.length() + 1, paras.c_str());
#endif
	BOOL r = ::CreateProcess( NULL, cl,
		NULL, NULL, FALSE, 0, NULL, NULL,
		&si, &pi);
	free(cl);
	if (!r) return PROCESS_INFORMATION{ 0,0,0,0 };
	flush();
	return pi;
}

PROCESS_INFORMATION Process_t::Start_Suspended(const TSTRING& paras, STARTUPINFO& si) {
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	LPTSTR cl = (LPTSTR)calloc(paras.length() + 1, sizeof(TCHAR));
	if (!cl) return PROCESS_INFORMATION{ 0,0,0,0 };
#ifdef UNICODE
	wcscpy_s(cl, paras.length() + 1, paras.c_str());
#else
	strcpy_s(cl, paras.length() + 1, paras.c_str());
#endif
	BOOL r = ::CreateProcess( NULL, cl,
		NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL,
		&si, &pi);
	free(cl);
	if (!r) return PROCESS_INFORMATION{ 0,0,0,0 };
	flush();
	return pi;
}

bool Process_t::StartOnly(const TSTRING& paras, STARTUPINFO& si) {
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	LPTSTR cl = (LPTSTR)calloc(paras.length() + 1, sizeof(TCHAR));
	if (!cl) return FALSE;
#ifdef UNICODE
	wcscpy_s(cl, paras.length() + 1, paras.c_str());
#else
	strcpy_s(cl, paras.length() + 1, paras.c_str());
#endif
	BOOL r = ::CreateProcess(
		NULL, cl,  
		NULL, NULL, FALSE, 0, NULL, NULL,
		&si, &pi);
	free(cl);
	if (!r) return false;
	flush();
	CloseProcessHandle(pi);
	return true;
}

_Success_(return) BOOL Process_t::StartAsActiveUserT(_In_opt_ LPCTSTR lpApplicationName,
	_Inout_opt_ LPTSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles, _In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment, _In_opt_ LPCTSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFO lpStartupInfo, _Out_ LPPROCESS_INFORMATION lpProcessInformation
) {
#pragma warning(push)
#pragma warning(disable: 6001)
	DWORD dwSessionID = WTSGetActiveConsoleSessionId();
	HANDLE hToken = NULL, hTokenDup = NULL;
	PVOID pEnv = NULL;

	//获取当前处于活动状态用户的Token
	if (!WTSQueryUserToken(dwSessionID, &hToken)) {
		if (hToken) CloseHandle(hToken);
		return FALSE;
	}

	//复制新的Token
	if ((!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
		SecurityIdentification, TokenPrimary, &hTokenDup))
		|| (!hTokenDup)) {
		CloseHandle(hToken);
		return FALSE;
	}

	//创建环境信息
	if (!lpEnvironment) {
		if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE)) {
			CloseHandle(hTokenDup);
			CloseHandle(hToken);
			return FALSE;
		}
	}

	//设置启动参数
	TCHAR lpDesktop[] = _TEXT("winsta0\\default");
	lpStartupInfo->lpDesktop = lpDesktop;

	//开始创建进程
	DWORD dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | dwCreationFlags;
#ifdef UNICODE
	dwCreateFlag |= CREATE_UNICODE_ENVIRONMENT;
#endif
	BOOL bResult = CreateProcessAsUser(hTokenDup, lpApplicationName, lpCommandLine,
		lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreateFlag,
		lpEnvironment ? lpEnvironment : pEnv, lpCurrentDirectory,
		lpStartupInfo, lpProcessInformation);

	//附加操作,回收资源

	if (pEnv) DestroyEnvironmentBlock(pEnv);
	CloseHandle(hTokenDup);
	CloseHandle(hToken);
#pragma warning(pop)

	return bResult;
}

_Success_(return) BOOL Process_t::StartAsUserT(
	DWORD dwSessionId,
	_In_opt_ LPCTSTR lpApplicationName,
	_Inout_opt_ LPTSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles, _In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment, _In_opt_ LPCTSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFO lpStartupInfo, _Out_ LPPROCESS_INFORMATION lpProcessInformation
) {
#pragma warning(push)
#pragma warning(disable: 6001)
	DWORD dwSessionID = dwSessionId;
	HANDLE hToken = NULL, hTokenDup = NULL;
	PVOID pEnv = NULL;

	//获取当前处于活动状态用户的Token
	if (!WTSQueryUserToken(dwSessionID, &hToken)) {
		if (hToken) CloseHandle(hToken);
		return FALSE;
	}

	//复制新的Token
	if ((!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
		SecurityIdentification, TokenPrimary, &hTokenDup))
		|| (!hTokenDup)) {
		CloseHandle(hToken);
		return FALSE;
	}

	//创建环境信息
	if (!lpEnvironment) {
		if (!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE)) {
			CloseHandle(hTokenDup);
			CloseHandle(hToken);
			return FALSE;
		}
	}

	//设置启动参数
	TCHAR lpDesktop[] = _TEXT("winsta0\\default");
	lpStartupInfo->lpDesktop = lpDesktop;

	//开始创建进程
	DWORD dwCreateFlag = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | dwCreationFlags;
#ifdef UNICODE
	dwCreateFlag |= CREATE_UNICODE_ENVIRONMENT;
#endif
	BOOL bResult = CreateProcessAsUser(hTokenDup, lpApplicationName, lpCommandLine,
		lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreateFlag,
		lpEnvironment ? lpEnvironment : pEnv, lpCurrentDirectory,
		lpStartupInfo, lpProcessInformation);

	//附加操作,回收资源

	if (pEnv) DestroyEnvironmentBlock(pEnv);
	CloseHandle(hTokenDup);
	CloseHandle(hToken);
#pragma warning(pop)

	return bResult;
}

#pragma warning(pop)

size_t Process_t::count() {
	size_t i = 0;
	PROCESSENTRY32 pe32;
	AutoZeroMemory(pe32);
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE){
		return (size_t)-1;
	}
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore){
		i++;
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	return i;
}

size_t Process_t::count(const TSTRING& paras) {
	size_t i = 0;
	PROCESSENTRY32 pe32 = {0};
	pe32.dwSize = sizeof(pe32);
	LPCTSTR PROCName = paras.c_str();
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		i += 0;
	}
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	while (bMore)
	{
		if (STRCMP(PROCName, pe32.szExeFile) == 0)
		{
			i++;
		}
		bMore = ::Process32Next(hProcessSnap, &pe32);
	}
	return i;
}

DWORD Process_t::suspend(HANDLE hProcess){
	if (!ZwSuspendProcess) {
		if (!InitSuspend()) return (DWORD)-1;
	}
	return ZwSuspendProcess(hProcess);
}

DWORD Process_t::resume(HANDLE hProcess){
	if (!ZwResumeProcess) {
		if (!InitSuspend()) return (DWORD)-1;
	}
	return ZwResumeProcess(hProcess);
}

bool Process_t::CloseAllWindows(ProcessInfo pi, bool wait, DWORD waittime) {
	vector<HWND> win; this->findWindows(pi, win);
	if (win.empty()) { SetLastError(233); return false; }
	for (auto i : win) {
		::SendMessageW(i, WM_CLOSE, 0, 0);
		::DestroyWindow(i);
	}
	if (!wait) return true;
	return !::WaitForSingleObject(pi, waittime);
}

bool Process_t::InitSuspend(){
	ZwSuspendProcess = (SUSPENDPROCESS)GetProcAddress(ntdll, "ZwSuspendProcess");
	ZwResumeProcess = (RESUMEPROCESS)GetProcAddress(ntdll, "ZwResumeProcess");
	if (!(ZwSuspendProcess&&ZwResumeProcess)) {
		return 0;
	}
	return 1;
}

NTSTATUS Process_t::NtSetInformationProcess(
	HANDLE ProcessHandle, 
	PROCESS_INFORMATION_CLASS ProcessInformationClass,
	PVOID ProcessInformation,
	ULONG ProcessInformationLength){
	if (!_NtSetInformationProcess) {
		_NtSetInformationProcess = (__NtSetInformationProcess)GetProcAddress(
			ntdll, "NtSetInformationProcess");
		if (!_NtSetInformationProcess) return -1;
	}
	return _NtSetInformationProcess(ProcessHandle, ProcessInformationClass,
		ProcessInformation, ProcessInformationLength);
}

__kernel_entry NTSTATUS Process_t::NtQueryInformationProcess(
	HANDLE ProcessHandle,
	PROCESSINFOCLASS ProcessInformationClass,
	PVOID ProcessInformation, 
	ULONG ProcessInformationLength, 
	PULONG ReturnLength){
	if (!_NtQueryInformationProcess) {
		_NtQueryInformationProcess = (__NtQueryInformationProcess)GetProcAddress(
			ntdll, "NtQueryInformationProcess");
		if (!_NtQueryInformationProcess) return -1;
	}
	return _NtQueryInformationProcess(ProcessHandle, ProcessInformationClass,
		ProcessInformation, ProcessInformationLength, ReturnLength);
}

#pragma warning(push)
#pragma warning(disable: 6387)
bool Process_t::crash(HANDLE hProcess, bool wait) {
	if (!hProcess) return false;
	HANDLE h = codecat(CreateRem, oteThread)(hProcess, 0, 0, 0, 0, 0, 0);
	if(!h) return false;
	if (wait) WaitForSingleObject(h, 0xffffffff);
	CloseHandle(h);
	return true;
}
#pragma warning(pop)

Process_t Process;

bool WindowShowOnTaskBar(HWND hWnd, bool bShow) {
	HRESULT hr = 0;
	ITaskbarList* pTaskbarList = NULL;
	hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER,
		IID_ITaskbarList, (void**)&pTaskbarList);

	if (SUCCEEDED(hr))
	{
		pTaskbarList->HrInit();
		if (bShow)
			pTaskbarList->AddTab(hWnd);
		else
			pTaskbarList->DeleteTab(hWnd);

		pTaskbarList->Release();
		return true;
	}

	return false;
}

errno_t CopyText(const wstring& text, UINT uFormat) {
	if (!::OpenClipboard(NULL)) {
		return 1;
	}
	if (!::EmptyClipboard()) {
		if (!::CloseClipboard()) {
			return 2;
		}
		return 3;
	}
	size_t tSize = (text.length() + 1) * sizeof(wchar_t);
	HGLOBAL hMem = ::GlobalAlloc(GHND, tSize);
	if (!hMem) {
		if (!::CloseClipboard()) {
			return 4;
		}
		return 5;
	}
	wchar_t* strText = (wchar_t*)calloc(tSize, 1);
	//wcscpy_s(strText, tSize, text.c_str());
	if (strText == 0) return 15;
	memcpy(strText, text.c_str(), tSize);

	// 把数据拷贝到全局内存中   
	// 锁住内存区    
	LPWSTR lpStr = (LPWSTR)GlobalLock(hMem);

	wchar_t* pMem = reinterpret_cast<wchar_t*>(lpStr);
	if (!pMem) {
		if (!::CloseClipboard()) {
			return 6;
		}
		if ((!::GlobalUnlock(hMem)) && (GetLastError() != NO_ERROR)) {
			return 7;
		}
		if (!::GlobalFree(hMem)) {
			return 8;
		}
		return 9;
	}
	// 内存复制   
	::memcpy(lpStr, strText, tSize);
	// 字符结束符: GMEM_ZEROINIT
	if ((!::GlobalUnlock(hMem)) && (GetLastError() != NO_ERROR)) {
		return 10;
	}
	HANDLE hResult = ::SetClipboardData(uFormat, hMem);
	if (!::CloseClipboard()) {
		return 11;
	}
	::free(strText);
	if ((!hResult) || hResult == INVALID_HANDLE_VALUE) {
		return 14;
	}
	return 0;
}

string CopyText(errno_t err) {
	static map<errno_t, string> kv;
	if (kv.empty()) {
		kv[0] = "No any error";
		kv[1] = "OpenClipboard(NULL)";
		kv[2] = "CloseClipboard()";
		kv[3] = "EmptyClipboard()";
		kv[4] = "CloseClipboard()";
		kv[5] = "GlobalAlloc(GHND, text.length() + 1)";
		kv[6] = "CloseClipboard()";
		kv[7] = "GlobalUnlock(hMem)";
		kv[8] = "GlobalFree(hMem)";
		kv[9] = "GlobalLock(hMem)";
		kv[10] = "GlobalUnlock(hMem)";
		kv[11] = "CloseClipboard()";
		kv[14] = "SetClipboardData(uFormat, hMem)";
		kv[15] = "calloc(tSize, 1)";
	}
	try {
		return kv.at(err);
	}
	catch (std::out_of_range&) {
		return "Unknown";
	}
	catch (std::exception&) {
		return "std::exception";
	}
#pragma warning(push)
#pragma warning(disable: 4702)
	return "[UNKNOWN ERROR]";
#pragma warning(pop)
}

std::string GenerateUUID()
{
	std::string guid;
#ifdef _WIN32
	UUID uuid;
	if (RPC_S_OK != UuidCreate(&uuid))
	{
		return guid;
	}
	char tmp[37] = { 0 };
	sprintf_s(tmp, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid.Data1, uuid.Data2, uuid.Data3,
		uuid.Data4[0], uuid.Data4[1],
		uuid.Data4[2], uuid.Data4[3],
		uuid.Data4[4], uuid.Data4[5],
		uuid.Data4[6], uuid.Data4[7]);
	guid.assign(tmp);
#else
	uuid_t uuid;
	char str[50] = {};
	uuid_generate(uuid);
	uuid_unparse(uuid, str);
	guid.assign(str);
#endif
	return guid;
}

std::string GenerateUUIDWithoutDelim() {
	std::string guid;
#ifdef _WIN32
	UUID uuid;
	if (RPC_S_OK != UuidCreate(&uuid))
	{
		return guid;
	}
	char tmp[37] = { 0 };
	sprintf_s(tmp, "%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",
		uuid.Data1, uuid.Data2, uuid.Data3,
		uuid.Data4[0], uuid.Data4[1],
		uuid.Data4[2], uuid.Data4[3],
		uuid.Data4[4], uuid.Data4[5],
		uuid.Data4[6], uuid.Data4[7]);
	guid.assign(tmp);
#else
	uuid_t uuid;
	char str[50] = {};
	uuid_generate(uuid);
	uuid_unparse(uuid, str);
	guid.assign(str);
#endif
	return guid;
}

std::wstring GenerateUUIDW()
{
	std::wstring guid;
	UUID uuid;
	if (RPC_S_OK != UuidCreate(&uuid)) return guid;
	wchar_t tmp[37*2] = { 0 };
	wsprintf(tmp, L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		uuid.Data1, uuid.Data2, uuid.Data3,
		uuid.Data4[0], uuid.Data4[1],
		uuid.Data4[2], uuid.Data4[3],
		uuid.Data4[4], uuid.Data4[5],
		uuid.Data4[6], uuid.Data4[7]);
	guid.assign(tmp);
	return guid;
}

std::wstring GenerateUUIDWithoutDelimW() {
	std::wstring guid;
	UUID uuid;
	if (RPC_S_OK != UuidCreate(&uuid)) return guid;
	wchar_t tmp[37*2] = { 0 };
	wsprintf(tmp, L"%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",
		uuid.Data1, uuid.Data2, uuid.Data3,
		uuid.Data4[0], uuid.Data4[1],
		uuid.Data4[2], uuid.Data4[3],
		uuid.Data4[4], uuid.Data4[5],
		uuid.Data4[6], uuid.Data4[7]);
	guid.assign(tmp);
	return guid;
}

void PrintTextEachCharW(wstring text, DWORD timeout) {
	for (wchar_t& x : text) {
		_putwch(x);
		if (timeout) Sleep(timeout);
	}
}

void PrintTextEachCharA( string text, DWORD timeout) {
	for (char& x : text) {
		_putch(x);
		if (timeout) Sleep(timeout);
	}
}

string GetCurrentUserSecurityId() {
	constexpr int MAX_NAME = 260;
	char userName[MAX_NAME] = "";
	char sid[MAX_NAME] = "";
	stringstream ss;
	DWORD nameSize = sizeof(userName);
	GetUserNameA((LPSTR)userName, &nameSize);


	char userSID[MAX_NAME] = "";
	char userDomain[MAX_NAME] = "";
	DWORD sidSize = sizeof(userSID);
	DWORD domainSize = sizeof(userDomain);


	SID_NAME_USE snu;
	LookupAccountNameA(NULL,
		(LPSTR)userName,
		(PSID)userSID,
		&sidSize,
		(LPSTR)userDomain,
		&domainSize,
		&snu);


	PSID_IDENTIFIER_AUTHORITY psia = GetSidIdentifierAuthority(userSID);
	//sidSize = sprintf(sid, "S-%lu-", SID_REVISION);
	//sidSize += sprintf(sid + strlen(sid), "%-lu", psia->Value[5]);
	ss << "S-" << SID_REVISION << "-" << int(psia->Value[5]);

	int i = 0;
	int subAuthorities = *GetSidSubAuthorityCount(userSID);


	for (i = 0; i < subAuthorities; i++)
	{
		ss << "-" << (*GetSidSubAuthority(userSID, i));
	}
	
	//printf("current sid is %s\n", sid);
	return ss.str();;
}

wstring ErrorCodeToStringW(DWORD ErrorCode)
{
	typedef LPWSTR str_t;
	str_t LocalAddress = NULL;
	if (NULL == ([&] {
		__try {
			if (!FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_FROM_SYSTEM, NULL,
				ErrorCode, 0, (str_t)&LocalAddress, 0, NULL)) {
				return (str_t)NULL;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return (str_t)NULL; }
		return LocalAddress;
	})()) {
		SetLastError(ErrorCode);
		return L"";
	}
	LocalAddress[wcslen((str_t)LocalAddress) - 1] = 0;
	wstring szText = LocalAddress;
	LocalFree((HLOCAL)LocalAddress);
	SetLastError(ErrorCode);
	return szText;
}

string  ErrorCodeToStringA(DWORD ErrorCode)
{
	typedef LPSTR str_t;
	str_t LocalAddress = NULL;
	if (NULL == ([&] {
		__try {
			if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_FROM_SYSTEM, NULL,
				ErrorCode, 0, (str_t)&LocalAddress, 0, NULL)) {
				return (str_t)NULL;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return (str_t)NULL; }
		return LocalAddress;
	})()) {
		SetLastError(ErrorCode);
		return "";
	}
	LocalAddress[strlen((str_t)LocalAddress) - 1] = 0;
	string szText = LocalAddress;
	LocalFree((HLOCAL)LocalAddress);
	SetLastError(ErrorCode);
	return szText;
}

void* GetBufMemory(size_t& maxsize) {
	void* ptr = NULL;
	while (maxsize > 1) {
		ptr = malloc(maxsize);
		if (ptr != NULL) break;
		maxsize /= 2;
	}
	if (ptr) memset(ptr, 0, maxsize);
	return ptr;
}

wstring GetProcessStdOutputW(wstring cmd_line, bool NoWindow) {
	wstring r;

	SECURITY_ATTRIBUTES sa = {0};                //保存设定管道的安全属性
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;        //安全描述符
	sa.bInheritHandle = TRUE;              //安全描述的对象能否被新创建的进程继承

	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		return r;
	}
	STARTUPINFOW si;             //用于指定新进程的主窗口特性的一个结构
	PROCESS_INFORMATION pi;     //构返回有关新进程及其主线程的信息
	AutoZeroMemory(si);
	si.cb = sizeof(STARTUPINFO);//包含STARTUPINFO结构中的字节数.
	GetStartupInfoW(&si);        //取得进程在启动时被指定的 STARTUPINFO 结构

	//hWrite为匿名管理写入句柄
	si.hStdError = hWrite;     //错误输出写入匿名管道
	si.hStdOutput = hWrite;    //标准输出写入匿名管道
	//si.hStdInput             //标准输入,这里没用到
	if (NoWindow) si.wShowWindow = SW_HIDE;  //隐藏窗口

	si.dwFlags = STARTF_USESHOWWINDOW | //使用wShowWindow 成员(用于隐藏窗口)
		STARTF_USESTDHANDLES;  //使用hStdInput 、hStdOutput和hStdError成员
#pragma warning(push)
#pragma warning(disable: 6335)
	if (!CreateProcessW(NULL, (LPWSTR)(cmd_line.c_str())
		, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		return r;
	}
	CloseHandle(hWrite);     //关闭匿名管道写入
	Process.resume(pi.hProcess);
	Process.CloseProcessHandle(pi);
#pragma warning(pop)

	size_t buffer_size = 4096;
	char* buffer = (char*)GetBufMemory(buffer_size); //用于保存输出信息
	if (buffer == nullptr) {
		return r;
	}
	memset(buffer, 0, buffer_size);
	DWORD bytesRead = 0;
	while (TRUE)
	{
		if (!ReadFile(hRead, buffer, (DWORD)(buffer_size - 1), &bytesRead, NULL)) break;
		r += s2ws(buffer);
	}
	free(buffer);

	::CloseHandle(hRead);

	return r;
}

bool GetProcessStdOutputWithExitCodeW(
	std::wstring cmd_line, DWORD* dwExitCode, std::wstring& output,
	bool NoWindow,
	void(__stdcall* callback)(wstring&, PVOID), PVOID callbackParamter
) {
	SECURITY_ATTRIBUTES sa = { 0 };
	HANDLE hRead, hWrite;

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
		return false;
	}
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	AutoZeroMemory(si);
	si.cb = sizeof(STARTUPINFO);
	GetStartupInfoW(&si);

	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	if (NoWindow) si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW |
		STARTF_USESTDHANDLES;
#pragma warning(push)
#pragma warning(disable: 6335)
	if (!CreateProcessW(NULL, (LPWSTR)(cmd_line.c_str())
		, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		return false;
	}
	CloseHandle(hWrite);
#pragma warning(pop)

	size_t buffer_size = 32768;
	char* buffer = (char*)VirtualAlloc(0, buffer_size, MEM_COMMIT, PAGE_READWRITE);
	if (buffer == nullptr) {
		Process.kill(pi.hProcess);
		Process.CloseProcessHandle(pi);
		return false;
	}
	memset(buffer, 0, buffer_size);
	DWORD bytesRead = 0, bytesLastRead = 0;
	output.clear();
	wstring wsBuffer;
	Process.resume(pi.hProcess);
	while (TRUE)
	{
		bytesLastRead = bytesRead;
		if (!ReadFile(hRead, buffer, (DWORD)(buffer_size - 1), &bytesRead, NULL)) break;
		wsBuffer = s2ws(buffer);
		if (callback) callback(wsBuffer, callbackParamter);
		output += wsBuffer;
	}
	VirtualFree(buffer, 0, MEM_RELEASE);

	if (dwExitCode) GetExitCodeProcess(pi.hProcess, dwExitCode);
	Process.CloseProcessHandle(pi);
	::CloseHandle(hRead);

	return true;
}


ULONGLONG MyGetFileSizeW(wstring filename) {
	GET_FILEEX_INFO_LEVELS fInfoLevelId = GetFileExInfoStandard;
	WIN32_FILE_ATTRIBUTE_DATA FileInformation = {0};
	// 通过winapi获取文件属性
	BOOL bGet = GetFileAttributesExW(filename.c_str(), fInfoLevelId, &FileInformation);
	if (bGet) {
		_fsize_t nFileSizeLow = FileInformation.nFileSizeLow;
		_fsize_t nFileSizeHigh = FileInformation.nFileSizeHigh;
		ULARGE_INTEGER szFile; AutoZeroMemory(szFile);
		szFile.HighPart = nFileSizeHigh; // 超大文件才需要
		szFile.LowPart = nFileSizeLow;
		return szFile.QuadPart;
	}
	return 0;
}

signed char IsFileOrDirectory(string path) {
	if (path.ends_with("..")) path.append("\\"); // a file's name CANNOT ends with ..
	DWORD attr = GetFileAttributesA(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) return 0;
		return -2;
	}
	return (attr & FILE_ATTRIBUTE_DIRECTORY) ? -1 : 1;
}

signed char IsFileOrDirectory(wstring path){
	if (path.ends_with(L"..")) path.append(L"\\"); // a file's name CANNOT ends with ..
	DWORD attr = GetFileAttributesW(path.c_str());
	if (attr == INVALID_FILE_ATTRIBUTES) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) return 0;
		return -2;
	}
	return (attr & FILE_ATTRIBUTE_DIRECTORY) ? -1 : 1;
}

bool LoadLibAsProcessW(HANDLE hProcess, wstring LibPath) {
	HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
	if (!ntdll) return false;
	LPTHREAD_START_ROUTINE LoadLibraryW = 
		(LPTHREAD_START_ROUTINE)::GetProcAddress(ntdll, "LoadLibraryW");
	if (!LoadLibraryW) return false;
	HANDLE hThread = ::CreateRemoteThread(hProcess, NULL, 0, LoadLibraryW,
		static_cast<void*>(const_cast<wchar_t*>(LibPath.c_str())), 0, NULL);
	if(!hThread) return false;
	::WaitForSingleObject(hThread, MAXDWORD);
	::CloseHandle(hThread);
	return true;
}

bool MyStartAsServiceW(wstring svc_name, LPSERVICE_MAIN_FUNCTIONW svc_main) {
	SERVICE_TABLE_ENTRYW ServiceTable[2]{ 0 };

	const size_t _size = (svc_name.length() + 1) * (sizeof(WCHAR));
	LPWSTR sname = (LPWSTR)calloc(_size, 1);
	if (!sname) return false;
#ifdef _MSVC_LANG
	wcscpy_s(sname, _size, svc_name.c_str());
#else
	wcscpy(sname, svc_name.c_str());
#endif
	ServiceTable[0].lpServiceName = sname;
	ServiceTable[0].lpServiceProc = svc_main;

	// 启动服务的控制分派机线程
	BOOL ret = StartServiceCtrlDispatcherW(ServiceTable);
	free(sname);
	return ret;
}

bool MyStartAsServiceA(string svc_name, LPSERVICE_MAIN_FUNCTIONA svc_main) {
	SERVICE_TABLE_ENTRYA ServiceTable[2]{ 0 };

	const size_t _size = (svc_name.length() + 1) * (sizeof(CHAR));
	LPSTR sname = (LPSTR)calloc(_size, 1);
	if (!sname) return false;
#ifdef _MSVC_LANG
	strcpy_s(sname, _size, svc_name.c_str());
#else
	strcpy(sname, svc_name.c_str());
#endif
	ServiceTable[0].lpServiceName = sname;
	ServiceTable[0].lpServiceProc = svc_main;

	// 启动服务的控制分派机线程
	BOOL ret = StartServiceCtrlDispatcherA(ServiceTable);
	free(sname);
	return ret;
}

bool MyWriteApplicationEvent(LPCWSTR string, DWORD id, WORD type, void* lpRawData,
	DWORD dwDataSize, WORD wCategory, PSID lpUserSid, HANDLE hEventLog) {
	bool $ = false;
	hEventLog ? 0 : $ = hEventLog = RegisterEventSourceW(NULL, L"Application");
	if (!hEventLog) return false;
	LPCWSTR strs[1] = { string };
	bool _ = ::ReportEventW(hEventLog, type, wCategory, id, 
		lpUserSid, 1, dwDataSize, strs, lpRawData);
	if($) ::DeregisterEventSource(hEventLog);
	return _;
}

void FullScreenWindow(HWND hWindow, PFullScreenWindowRestoreInfo winfo) {
	if (winfo) {
		winfo->style = GetWindowLong(hWindow, GWL_STYLE); //存储上次的窗口风格，退出时使用
		GetWindowRect(hWindow, &winfo->wrect);//存储上次的窗口大小，退出时使用
	}

	LONG l_WinStyle = GetWindowLong(hWindow, GWL_STYLE);   /* 获取窗口信息 */
	SetWindowLongPtr(hWindow, GWL_STYLE, (l_WinStyle | WS_POPUP | WS_MAXIMIZE)
		& ~WS_CAPTION & ~WS_THICKFRAME & ~WS_BORDER);

	SetWindowPos(hWindow, 0, 0, 0, GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN), SWP_NOACTIVATE | SWP_NOZORDER);
}

void UnFullScreenWindow(HWND hWindow, PFullScreenWindowRestoreInfo winfo) {
	SetWindowLongPtr(hWindow, GWL_STYLE, winfo->style);
	SetWindowPos(hWindow, NULL, winfo->wrect.left, winfo->wrect.top,
		winfo->wrect.right - winfo->wrect.left, winfo->wrect.bottom - winfo->wrect.top,
		SWP_NOZORDER);
	ShowWindow(hWindow, SW_SHOW);
	UpdateWindow(hWindow);
}

void CenterWindow(HWND hWnd, HWND parent) {
	RECT rcParent{}; if (parent) GetWindowRect(parent, &rcParent);

	//取得窗口尺寸
	RECT rect; GetWindowRect(hWnd, &rect);
	// 获得窗口大小
	auto w = rect.right - rect.left,
		h = rect.bottom - rect.top;
	//重新设置rect里的值
	if (parent) {
		auto w2 = rcParent.right - rcParent.left,
			h2 = rcParent.bottom - rcParent.top;
		rect.left = rcParent.left + w2/2 - w/2;
		rect.top = rcParent.top + h2/2 - h/2;
	}
	else {
		rect.left = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
		rect.top = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
	}
	//移动窗口到指定的位置
	SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top,
		1, 1, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}

string cppGetUserNameA() {
	DWORD pcbBuffer = 32 + 1;
	char* buffer = (char*)calloc(pcbBuffer, sizeof(char));
	if (!buffer) return "";
	GetUserNameA(buffer, &pcbBuffer);
	string _u(buffer);
	free(buffer);
	return _u;
}

std::wstring cppGetUserNameW() {
	DWORD pcbBuffer = 32 + 1;
	WCHAR* buffer = (WCHAR*)calloc(pcbBuffer, sizeof(WCHAR));
	if (!buffer) return L"";
	GetUserNameW(buffer, &pcbBuffer);
	wstring _u(buffer);
	free(buffer);
	return _u;
}

BOOL IsCurrentUserInBuiltinGroup(WELL_KNOWN_SID_TYPE SidType) { //判断是否在组
	BOOL fInGroup = FALSE;
	HANDLE hToken = NULL;
	HANDLE hTokenToCheck = NULL;
	DWORD cbSize = 0;

	OSVERSIONINFO osver = {0};
	osver.dwOSVersionInfoSize = sizeof(osver);

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_QUERY | TOKEN_DUPLICATE, &hToken)) goto Cleanup;
	//if (!GetVersionEx(&osver)) goto Cleanup; // GetVersionEx is deprecated
	if (IsWindowsVistaOrGreater()) {
		TOKEN_ELEVATION_TYPE elevType;
		AutoZeroMemory(elevType);
		if (!GetTokenInformation(hToken, TokenElevationType, &elevType,
			sizeof(elevType), &cbSize)) goto Cleanup;
		if (TokenElevationTypeLimited == elevType) {
			if (!GetTokenInformation(hToken, TokenLinkedToken,
				&hTokenToCheck, sizeof(hTokenToCheck), &cbSize))
				goto Cleanup;
		}
	}

	if (!hTokenToCheck)
	{
		if (!DuplicateToken(hToken, SecurityIdentification, &hTokenToCheck))
			goto Cleanup;
	}

	BYTE adminSID[SECURITY_MAX_SID_SIZE];
	AutoZeroMemory(adminSID);
	cbSize = sizeof(adminSID);
	if (!CreateWellKnownSid(SidType, NULL, &adminSID, &cbSize))
		goto Cleanup;
	if (!CheckTokenMembership(hTokenToCheck, &adminSID, &fInGroup)) 
		goto Cleanup;

Cleanup:
	if (hToken)    CloseHandle(hToken);
	if (hTokenToCheck) CloseHandle(hTokenToCheck);
	return fInGroup;
}

BOOL IsUserInBuiltinGroup(HANDLE hProcess, WELL_KNOWN_SID_TYPE SidType) {
	BOOL fInGroup = FALSE;
	HANDLE hToken = NULL;
	HANDLE hTokenToCheck = NULL;
	DWORD cbSize = 0;

	OSVERSIONINFO osver = {0};
	osver.dwOSVersionInfoSize = sizeof(osver);

	if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken)) goto Cleanup;
	//if (!GetVersionEx(&osver)) goto Cleanup; // GetVersionEx is deprecated
	if (IsWindowsVistaOrGreater()) {
		TOKEN_ELEVATION_TYPE elevType;
		AutoZeroMemory(elevType);
		if (!GetTokenInformation(hToken, TokenElevationType, &elevType,
			sizeof(elevType), &cbSize)) goto Cleanup;
		if (TokenElevationTypeLimited == elevType) {
			if (!GetTokenInformation(hToken, TokenLinkedToken,
				&hTokenToCheck, sizeof(hTokenToCheck), &cbSize))
				goto Cleanup;
		}
	}

	if (!hTokenToCheck)
	{
		if (!DuplicateToken(hToken, SecurityIdentification, &hTokenToCheck))
			goto Cleanup;
	}

	BYTE adminSID[SECURITY_MAX_SID_SIZE];
	AutoZeroMemory(adminSID);
	cbSize = sizeof(adminSID);
	if (!CreateWellKnownSid(SidType, NULL, &adminSID, &cbSize))
		goto Cleanup;
	if (!CheckTokenMembership(hTokenToCheck, &adminSID, &fInGroup)) 
		goto Cleanup;

Cleanup:
	if (hToken)    CloseHandle(hToken);
	if (hTokenToCheck) CloseHandle(hTokenToCheck);
	return fInGroup;
}

HANDLE GetProcessTokenById(DWORD pid) {
	if (pid == 0) return FALSE;
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32W pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == 0 || hProcessSnap == INVALID_HANDLE_VALUE) return FALSE;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32)) {
		do {
			if (pe32.th32ProcessID == pid) {
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE, pe32.th32ProcessID);
				HANDLE hToken = NULL;
				OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				CloseHandle(hProcessSnap);
				::CloseHandle(hProcess);
				return hToken;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	SetLastError(ERROR_NOT_FOUND);
	CloseHandle(hProcessSnap);
	return NULL;
}

size_t GetProcessTokenByName(std::vector<HANDLE>& buffer, LPCTSTR name) {
	if (name == 0) return FALSE;
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32W pe32 = { 0 };
	size_t foundCount = 0;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == 0 || hProcessSnap == INVALID_HANDLE_VALUE) return FALSE;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32)) {
		do {
#ifdef UNICODE
			if (0 == wcscmp(pe32.szExeFile, name)) {
#else
			if (0 == strcmp(pe32.szExeFile, name)) {
#endif
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION,
					FALSE, pe32.th32ProcessID);
				HANDLE hToken = NULL;
				OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				//CloseHandle(hProcessSnap);
				buffer.push_back(hToken);
				foundCount++;
				::CloseHandle(hProcess);
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);
	return foundCount;
}

PVOID GetModuleBaseAddrByPID(DWORD pid, TSTRING module_name) {
	if (module_name.empty()) {
		bool closeAfterUse = false;
		if (Process.isEmpty()) closeAfterUse = true;
		module_name = Process.find(pid).name();
		if (closeAfterUse) Process.close();
	}
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	if (!hSnap) return nullptr;
	MODULEENTRY32 md32; AutoZeroMemory(md32);
	md32.dwSize = sizeof md32;
	void* result = nullptr;
	LPCTSTR mdName = module_name.c_str();

	if (Module32First(hSnap, &md32)) {
		do {
			if (lstrcmpi(mdName, md32.szModule) == 0) {
				result = md32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnap, &md32));
		if (!result) SetLastError(ERROR_NOT_FOUND);
	}

	::CloseHandle(hSnap);
	return result;
}

namespace mariusbancila {
/* https://mariusbancila.ro/blog/2008/10/20/writing-utf-8-files-in-c/ */
std::string to_utf8(const wchar_t* buffer, DWORD len) {
	int nChars = ::WideCharToMultiByte(
		CP_UTF8, 0,
		buffer, len,
		NULL, 0, NULL, NULL);
	if (nChars == 0) return "";

	string newbuffer;
	newbuffer.resize(nChars);
	::WideCharToMultiByte(
		CP_UTF8, 0,
		buffer, len,
		const_cast<char*>(newbuffer.c_str()),
		nChars,
		NULL, NULL);

	return newbuffer;
}
}

namespace s7 {

#define _myrclassf(key, _default) \
	if(content. ## key) wcex. ## key = content. ## key;\
	else wcex. ## key = _default;
#define _myrclassc \
	_myrclassf(style, CS_HREDRAW | CS_VREDRAW);\
	_myrclassf(lpfnWndProc, WndProc);\
	_myrclassf(cbClsExtra, 0);\
	_myrclassf(cbWndExtra, 0);\
	_myrclassf(hInstance, NULL);\
	_myrclassf(hIcon, NULL);\
	_myrclassf(hIconSm, NULL);\
	_myrclassf(lpszMenuName, NULL);\
	_myrclassf(hCursor, LoadCursor(nullptr, IDC_ARROW));\
	_myrclassf(hbrBackground, (HBRUSH)(COLOR_WINDOW + 1));\
	_myrclassf(lpszClassName, className);\

#pragma warning(push)
#pragma warning(disable: 4302)
decltype(RegisterClassW(0)) 
MyRegisterClassW(PCWSTR className, WNDPROC WndProc, WNDCLASSEXW content) {
	WNDCLASSEXW wcex{ 0 };

	wcex.cbSize = sizeof(wcex);

	_myrclassc

	return RegisterClassExW(&wcex);
}
decltype(RegisterClassA(0)) 
MyRegisterClassA(PCSTR className, WNDPROC WndProc, WNDCLASSEXA content) {
	WNDCLASSEXA wcex{ 0 };

	wcex.cbSize = sizeof(wcex);

	_myrclassc

	return RegisterClassExA(&wcex);
}
#pragma warning(pop)
#undef _myrclassc
#undef _myrclassf

BOOL AdjustPrivilege(
	ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled
) {
	typedef NTSTATUS(WINAPI*pRtlAdjustPrivilege)
		(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
	pRtlAdjustPrivilege RtlAdjustPrivilege = NULL;
	HMODULE ntdll = GetModuleHandle(_T("ntdll.dll"));
	if (!ntdll) return FALSE;
	RtlAdjustPrivilege = (pRtlAdjustPrivilege)
		GetProcAddress(ntdll, "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege) return FALSE;
	BOOLEAN _tmp0 = 0;
	NTSTATUS result = RtlAdjustPrivilege(
		Privilege, Enable, CurrentThread, Enabled ? Enabled : &_tmp0);
	BOOL success = NT_SUCCESS(result);
	return success;
	/*
	SeCreateTokenPrivilege 0x2
	SeAssignPrimaryTokenPrivilege 0x3
	SeLockMemoryPrivilege 0x4
	SeIncreaseQuotaPrivilege 0x5
	SeUnsolicitedInputPrivilege 0x0
	SeMachineAccountPrivilege 0x6
	SeTcbPrivilege 0x7
	SeSecurityPrivilege 0x8
	SeTakeOwnershipPrivilege 0x9
	SeLoadDriverPrivilege 0xa
	SeSystemProfilePrivilege 0xb
	SeSystemtimePrivilege 0xc
	SeProfileSingleProcessPrivilege 0xd
	SeIncreaseBasePriorityPrivilege 0xe
	SeCreatePagefilePrivilege 0xf
	SeCreatePermanentPrivilege 0x10
	SeBackupPrivilege 0x11
	SeRestorePrivilege 0x12
	SeShutdownPrivilege 0x13
	SeDebugPrivilege 0x14
	SeAuditPrivilege 0x15
	SeSystemEnvironmentPrivilege 0x16
	SeChangeNotifyPrivilege 0x17
	SeRemoteShutdownPrivilege 0x18
	SeUndockPrivilege 0x19
	SeSyncAgentPrivilege 0x1a
	SeEnableDelegationPrivilege 0x1b
	SeManageVolumePrivilege 0x1c
	SeImpersonatePrivilege 0x1d
	SeCreateGlobalPrivilege 0x1e
	SeTrustedCredManAccessPrivilege 0x1f
	SeRelabelPrivilege 0x20
	SeIncreaseWorkingSetPrivilege 0x21
	SeTimeZonePrivilege 0x22
	SeCreateSymbolicLinkPrivilege 0x23
	*/
}

#if 0
BOOL AdjustPrivilege(
	LPCTSTR Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled
) {
	ULONG privid = 0; LUID luid{ 0 };
	if (!LookupPrivilegeValue(_T(""), Privilege, &luid)) return FALSE;
	privid = luid.LowPart;
	return AdjustPrivilege(privid, Enable, CurrentThread, Enabled);
}
#endif

NTSTATUS CallNtUnmapViewOfSection(HANDLE ProcessHandle, PVOID BaseAddress) {
	HMODULE ntdll = GetModuleHandle(_T("ntdll.dll"));
	if (!ntdll) return (NTSTATUS)-1;
	typedef NTSTATUS(WINAPI*tNtUnmapViewOfSection)(HANDLE, PVOID);
	tNtUnmapViewOfSection NtUnmapViewOfSection =
		(tNtUnmapViewOfSection)GetProcAddress(ntdll, "NtUnmapViewOfSection");
	if (!NtUnmapViewOfSection) return (NTSTATUS)-1;
	return NtUnmapViewOfSection(ProcessHandle, BaseAddress);
}

NTSTATUS __stdcall CallNtTerminateProcess(HANDLE ProcessHandle, NTSTATUS ExitStatus) {
	HMODULE ntdll_dll = GetModuleHandle(_T("ntdll.dll"));
	if (!ntdll_dll) return -1;
	typedef NTSTATUS(__stdcall* __NtTerminateProcess)(HANDLE, NTSTATUS);
	__NtTerminateProcess NtTerminateProcess = (__NtTerminateProcess)
		GetProcAddress(ntdll_dll, "NtTerminateProcess");
	return NtTerminateProcess(ProcessHandle, ExitStatus);
}

}

#pragma warning(push)
#pragma warning(disable: 6101)
BOOL CreateProcessInSession(_In_ DWORD dwSessionId,
	_In_opt_ LPCTSTR lpApplicationName,
	_Inout_opt_ LPTSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCTSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFO lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
) {
	auto& si = *lpStartupInfo;         // 
	//auto& pi = *lpProcessInformation;  // 
	//HANDLE hUserToken = NULL;          // 当前登录用户的令牌  
	HANDLE hUserTokenDup = NULL;       // 复制的用户令牌  
	HANDLE hPToken = NULL;             // 进程令牌

	//// 不需要获取用户token,子进程以父进程权限运行
	////WTSQueryUserToken(dwSessionId, &hUserToken);
	dwCreationFlags |= NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

	WCHAR lpDesktop[] = L"winsta0\\default";
	si.lpDesktop = lpDesktop;
	//指定创建进程的窗口站，Windows下唯一可交互的窗口站就是WinSta0\Default  

	TOKEN_PRIVILEGES tp{ 0 };
	LUID luid;

	//打开进程令牌  
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES |
		TOKEN_QUERY | TOKEN_DUPLICATE |
		TOKEN_ASSIGN_PRIMARY |
		TOKEN_ADJUST_SESSIONID |
		TOKEN_READ | TOKEN_WRITE, &hPToken)) {
		return FALSE;
	}

	//查找DEBUG权限的UID  
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hPToken);
		return FALSE;
	}

	//设置令牌信息  
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	//复制当前用户的令牌  
	if (!DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL,
		SecurityIdentification, TokenPrimary, &hUserTokenDup)) {
		CloseHandle(hPToken);
		return FALSE;
	}

	//设置当前进程的令牌信息  
	if (!SetTokenInformation(hUserTokenDup, TokenSessionId,
		(void*)&dwSessionId, sizeof(DWORD))) {
		CloseHandle(hUserTokenDup);
		CloseHandle(hPToken);
		return FALSE;
	}

	//应用令牌权限  
	if (!AdjustTokenPrivileges(hUserTokenDup, FALSE, &tp,
		sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		CloseHandle(hUserTokenDup);
		CloseHandle(hPToken);
		return FALSE;
	}

	//创建进程环境块，保证环境块是在用户桌面的环境下  
	LPVOID pEnv = NULL;
	if (CreateEnvironmentBlock(&pEnv, hUserTokenDup, TRUE)) {
		dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
	}

	//创建用户进程  
	if (!CreateProcessAsUser(hUserTokenDup, lpApplicationName, lpCommandLine,
		lpProcessAttributes, lpThreadAttributes, bInheritHandles,
		dwCreationFlags, lpEnvironment ? lpEnvironment : pEnv,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation))
	{
		CloseHandle(hUserTokenDup);
		CloseHandle(hPToken);
		return FALSE;
	}

	//关闭句柄
	//CloseHandle(pi.hProcess);
	//CloseHandle(pi.hThread);
	if (pEnv) DestroyEnvironmentBlock(pEnv);
	//if (hUserToken) CloseHandle(hUserToken);
	if (hUserTokenDup) CloseHandle(hUserTokenDup);
	if (hPToken) CloseHandle(hPToken);

	return TRUE;
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable: 4311)
#pragma warning(disable: 4302)
signed char StackOverflow::swallow_redpill() {
	unsigned char m[2+4]{};
	unsigned char rpill[] = "\x0f\x01\x0d\x00\x00\x00\x00\xc3";
	*((unsigned*)&rpill[3]) = (unsigned)m;
	//DWORD dw = 0;
	PVOID paddr = VirtualAlloc(
		NULL, sizeof(rpill),
		MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!paddr) return -1;
	RtlCopyMemory(paddr, &rpill, sizeof(rpill));
	((void(*)())paddr)();
	return (m[5] > 0xd0) ? 1 : 0;
}
#pragma warning(pop)


signed char __stdcall FileDeleteTreeW(std::wstring szPath, bool __internal_is_first_call__) {
	signed char status = 0;
	if (__internal_is_first_call__) {
		if (szPath.find(L"/") != wstring::npos) str_replace(szPath, L"/", L"\\");
		if (!szPath.ends_with(L"\\")) szPath += L"\\";
	}
	WIN32_FIND_DATAW findd{};
	HANDLE hFind = FindFirstFileW((szPath + L"*").c_str(), &findd);
	if (!hFind || hFind == INVALID_HANDLE_VALUE) {
		status = -1;
		return status;
	}
	do {
		if (wcscmp(findd.cFileName, L".") == 0 ||
			wcscmp(findd.cFileName, L"..") == 0) continue;
		wstring wstrFileName;
		wstrFileName.assign(szPath);
		//wstrFileName.append(L"\\");
		wstrFileName.append(findd.cFileName);
		if (findd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			wstrFileName.append(L"\\");
			auto result0 = FileDeleteTreeW(wstrFileName, false);
			if (result0 != 0) status = (status == -1) ? -1 : 1;
		} else {
			if (findd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				SetFileAttributesW(wstrFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
			if (!DeleteFileW(wstrFileName.c_str()))
				status = (status == -1) ? -1 : 1;
		}
	} while (FindNextFileW(hFind, &findd));
	FindClose(hFind);
	if (!RemoveDirectoryW(szPath.c_str()))
		status = (status == -1) ? -1 : 1;
	return status;
}


signed char __stdcall CopyFileTreeW(
	std::wstring src, std::wstring dest, bool bFailIfExists
) {
	if (!src.ends_with(L"\\")) src += L"\\";
	if (!dest.ends_with(L"\\")) dest += L"\\";
	signed char status = 0;
	WIN32_FIND_DATAW findd{};
	HANDLE hFind = FindFirstFileW((src + L"*").c_str(), &findd);
	if (!hFind || hFind == INVALID_HANDLE_VALUE) return -1;
	if (!file_exists(dest)) {
		if (!CreateDirectoryW(dest.c_str(), NULL)) return -1;
	}
	do {
		if (wcscmp(findd.cFileName, L".") == 0 ||
			wcscmp(findd.cFileName, L"..") == 0) continue;
		wstring wstrFileName = src + findd.cFileName;
		wstring wstrDest = dest + findd.cFileName;

		if ((findd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			auto result0 = CopyFileTreeW(wstrFileName, wstrDest, bFailIfExists);
			if (result0 != 0) status = (status == -1) ? -1 : 1;
		}
		else if (!CopyFileW(wstrFileName.c_str(), wstrDest.c_str(), bFailIfExists)) {
			status = (status == -1) ? -1 : 1;
		}
	} while (FindNextFileW(hFind, &findd));
	FindClose(hFind);
	return status;
}



bool DoesUserUsesChinese() {
	static auto langID = PRIMARYLANGID(GetUserDefaultUILanguage());
#pragma warning(push)
#pragma warning(disable: 6287)
	return langID == LANG_CHINESE ||
		langID == LANG_CHINESE_SIMPLIFIED ||
		langID == LANG_CHINESE_TRADITIONAL;
#pragma warning(pop)
}

std::string ConvertUTF16ToUTF8(PCWSTR utf16Str) {
	if (utf16Str == nullptr) return "";
	int utf16Len = static_cast<int>(wcslen(utf16Str));
	int utf8Len = WideCharToMultiByte(CP_UTF8, 0, utf16Str,
		utf16Len,	nullptr, 0, nullptr, nullptr);
	std::string utf8Str(utf8Len, 0);  // 创建一个足够大的字符串来容纳UTF-8字符串
	WideCharToMultiByte(CP_UTF8, 0, utf16Str, utf16Len, &utf8Str[0],
		utf8Len, nullptr, nullptr);
	return utf8Str;
}

std::wstring ConvertUTF8ToUTF16(PCSTR utf8Str) {
	if (utf8Str == nullptr) return L"";
	int utf8Len = static_cast<int>(strlen(utf8Str));
	int utf16Len = MultiByteToWideChar(CP_UTF8, 0, utf8Str,
		utf8Len, nullptr, 0);
	std::wstring utf16Str(utf16Len, 0);  // 创建一个足够大的wstring来容纳UTF-16字符串
	MultiByteToWideChar(CP_UTF8, 0, utf8Str, utf8Len,
		&utf16Str[0], utf16Len);
	return utf16Str;
}




// 查询注册表值并返回结果字符串  
bool MyQueryRegistryValue(
	HKEY hkeyinput, const std::wstring& subKeyPath,
	const std::wstring& dataName, std::wstring& value
) {
	DWORD type;
	DWORD dataSize = 0;
	LONG result;
	HKEY hKey;
	bool closeIfFail = false;

	// 打开指定的注册表键  
	if (!subKeyPath.empty()) {
		result = RegOpenKeyExW(hkeyinput, subKeyPath.c_str(), 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS) {
			return false;
		}
		closeIfFail = true;
	}
	else hKey = hkeyinput;

	// 查询值的大小  
	result = RegQueryValueExW(hKey, dataName.c_str(), NULL, &type, NULL, &dataSize);
	if (result != ERROR_SUCCESS) {
		if (closeIfFail) RegCloseKey(hKey);
		return false;
	}

	// 分配足够的内存来存储值  
	//std::vector<wchar_t> dataBuffer(dataSize / sizeof(wchar_t));
	wchar_t* dataBuffer = new wchar_t[dataSize / sizeof(wchar_t)];

	// 查询值本身  
	result = RegQueryValueExW(hKey, dataName.c_str(), NULL, &type,
		reinterpret_cast<LPBYTE>(dataBuffer), &dataSize);
	if (result != ERROR_SUCCESS) {
		delete[] dataBuffer;
		if (closeIfFail) RegCloseKey(hKey);
		return false;
	}

	// 将查询到的值转换为wstring  
	if (type == REG_SZ || type == REG_EXPAND_SZ) {
		value = (dataBuffer);
	}
	else {
		delete[] dataBuffer;
		if (closeIfFail) RegCloseKey(hKey);
		return false;
	}

	// 关闭注册表键句柄  
	if (closeIfFail) RegCloseKey(hKey);
	delete[] dataBuffer;
	return true;
}

// 设置注册表值  
bool MySetRegistryValue(
	HKEY hkeyinput, const std::wstring& subKeyPath,
	const std::wstring& dataName, const std::wstring& value
) {
	LONG result;
	DWORD dataSize = DWORD(
		(value.size() + 1) * sizeof(wchar_t)); // +1 for null terminator

	HKEY hKey;
	bool closeIfFail = false;
	// 打开指定的注册表键  
	if (!subKeyPath.empty()) {
		result = RegOpenKeyExW(hkeyinput, subKeyPath.c_str(),
			0, KEY_READ | KEY_WRITE, &hKey);
		if (result != ERROR_SUCCESS) {
			return false;
		}
		closeIfFail = true;
	}
	else hKey = hkeyinput;

	// 设置注册表值  
	const wchar_t* data = value.c_str();
	result = RegSetValueExW(hKey, dataName.c_str(), 0,
		REG_SZ, reinterpret_cast<const BYTE*>(data), dataSize);
	if (result != ERROR_SUCCESS) {
		if (closeIfFail) RegCloseKey(hKey);
		return false;
	}

	if (closeIfFail) RegCloseKey(hKey);
	return true;
}


static void SafeCloseHandleWork(HANDLE* handle) {
	__try {
		CloseHandle(*handle);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {}
}
static void SafeCloseHandle(HANDLE& handle) {
	SafeCloseHandleWork(&handle);
	handle = nullptr;
}



namespace GetProcessStdOutputWithExitCodeEnhancedNs {
	static std::atomic<size_t> session_id_generator;
	class Data {
	public:
		HANDLE hPipe = NULL;
		HANDLE hClient = NULL;
		wstring* result = nullptr;
		void(__stdcall* callback_fn)(std::wstring&, PVOID) = nullptr;
		PVOID callback_paramter = NULL;

		HANDLE event_pipeReady;

		Data() {
			event_pipeReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		~Data() {
			if (event_pipeReady) {
				SafeCloseHandle(event_pipeReady);
			}
		}
		Data(const Data&) = delete;
		Data& operator=(const Data&) = delete;
		Data(Data&& origin) = default;
		Data& operator=(Data&&) = default;
	};
	static std::map<size_t, Data> mapping;
	constexpr SIZE_T buffer_size = 16384;

	static DWORD WINAPI pipe_server(LPVOID lpParam) {
		size_t sessionId = (size_t)lpParam;
		Data* data = NULL;
		HANDLE hPipe;
		try {
			data = &mapping.at(sessionId);
			if (!data) throw - 1;
			hPipe = data->hPipe;
			if (!hPipe) throw - 1;
		}
		catch (...) {
			return ERROR_INVALID_PARAMETER;
		}

		SetEvent(data->event_pipeReady);

		while (1) {
			if (ConnectNamedPipe(hPipe, NULL)) break;
			
			if (GetLastError() == ERROR_PIPE_CONNECTED) break;
			if (GetLastError() == ERROR_PIPE_LISTENING) {
				Sleep(100); continue;
			}

			CloseHandle(hPipe);
			return GetLastError();
		}

		char* buffer = (char*)VirtualAlloc(0, buffer_size, 
			MEM_COMMIT, PAGE_READWRITE);
		if (!buffer) {
			CloseHandle(hPipe);
			return ERROR_OUTOFMEMORY;
		}

#if 0
		DWORD bytesRead = 0;
		data->result->clear();
		while (1) {
			memset(buffer, 0x00, buffer_size);
			if (!ReadFile(
				hPipe, buffer, buffer_size, &bytesRead, NULL
			)) {
				DWORD code = GetLastError();
				//MessageBoxW(0, LastErrorStrW().c_str(), 
				// to_wstring(code).c_str(), 0);
				break;
			}
#else
		DWORD bytesRead = 0;
		data->result->clear();
		wstring str;
		while (1) {
			memset(buffer, 0x00, buffer_size);
			if (!ReadFile(
				hPipe, buffer, buffer_size, &bytesRead, NULL
			)) {
				if (GetLastError() == ERROR_MORE_DATA) {
					DWORD memNeeded = 0;
					PeekNamedPipe(hPipe, NULL, NULL, NULL, NULL, &memNeeded);
					if (!memNeeded) break;
					char* buffer = (char*)calloc(memNeeded, sizeof(char));
					if (!buffer) break;
					if (ReadFile(
						hPipe, buffer, memNeeded, &bytesRead, NULL
					)) {
						str = s2ws(string(buffer, bytesRead));
					}
					free(buffer);
				}
				else break;
			}
			else str = s2ws(string(buffer, bytesRead));

#endif

			//wstring str = s2ws(buffer);
			if (data->callback_fn) {
				data->callback_fn(str, data->callback_paramter);
			}
			data->result->append(str);

		}

		CloseHandle(hPipe);
		//CloseHandle(data->hClient);
		data->hClient = NULL;
		VirtualFree(buffer, 0, MEM_RELEASE);
		return 0;
	}
}
bool GetProcessStdOutputWithExitCodeEnhanced(
	std::wstring cmd_line, DWORD* dwExitCode, std::wstring& output,
	bool NoWindow, 
	void(__stdcall* callback)(std::wstring&, PVOID), PVOID callbackParamter
) {
	using namespace GetProcessStdOutputWithExitCodeEnhancedNs;

	wstring tempFile = L"\\\\.\\pipe\\" + GenerateUUIDW();
	size_t sessionId = ++session_id_generator;
	mapping.insert(make_pair(sessionId, Data()));
	Data& data = mapping.at(sessionId);
	data.callback_fn = callback;
	data.callback_paramter = callbackParamter;
	data.result = &output;

	HANDLE hPipe;
	{
		SECURITY_ATTRIBUTES sa{
			.nLength = sizeof(sa),
			.bInheritHandle = TRUE,
		};
		hPipe = CreateNamedPipeW(
			tempFile.c_str(), PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			1, 65536, 65536, 0, &sa);
		if (hPipe == INVALID_HANDLE_VALUE || !hPipe) {
			mapping.erase(sessionId);
			return false;
		}
		
		data.hPipe = hPipe;
		//CloseHandle(hPipe);
	}
	const auto cleanPipe = [&] {
		//HANDLE hFile = CreateFileW(tempFile.c_str(), GENERIC_READ,
		//	0, 0, OPEN_EXISTING, 0, 0);
		//if (hFile && hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		mapping.erase(sessionId);
	};

	SECURITY_ATTRIBUTES sa{
		.nLength = sizeof(sa),
		.bInheritHandle = TRUE,
	};
	HANDLE hWrite = CreateFileW(tempFile.c_str(), GENERIC_ALL, 0,
		&sa, OPEN_EXISTING, 0, 0);
	if (!hWrite || hWrite == INVALID_HANDLE_VALUE) {
		cleanPipe();
		CloseHandle(hPipe);
		return false;
	}
	DWORD dwMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
	SetNamedPipeHandleState(hWrite, &dwMode, NULL, NULL);
	data.hClient = hWrite;

	HANDLE hThread = CreateThread(NULL, 0, pipe_server,
		(LPVOID)sessionId, 0, NULL);
	if (hThread == NULL) {
		mapping.erase(sessionId);
		CloseHandle(hPipe);
		return false;
	}
	//WaitForSingleObject(data.event_pipeReady, 30000);

	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = hWrite;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW |
		STARTF_USESTDHANDLES;
	if (!CreateProcessW(NULL, (LPWSTR)(cmd_line.c_str())
		, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		CloseHandle(hWrite);
		CloseHandle(hPipe);
		cleanPipe();
		return false;
	}
	ResumeThread(pi.hThread);

	WaitForSingleObject(pi.hProcess, INFINITE);
	if (dwExitCode) GetExitCodeProcess(pi.hProcess, dwExitCode);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hWrite);

	WaitForSingleObject(hThread, 60000); // 等待数据全部读完
	CloseHandle(hThread);
	cleanPipe();

	return true;
}


#if 0
#pragma region 以下是尝试使用异步的废代码
namespace GetProcessStdOutputWithExitCodeEnhancedNs {
	static std::atomic<size_t> session_id_generator;
	constexpr SIZE_T buffer_size = 16384;
	class Data {
	public:
		HANDLE hPipe = NULL;
		wstring* result = nullptr;
		void(__stdcall* callback_fn)(std::wstring&, PVOID) = nullptr;
		PVOID callback_paramter = NULL;
		OVERLAPPED overlapped;
		DWORD bytesRead = 0;
		char* buffer = 0;
		//bool overlappedDone = false;
		//HANDLE event_readFinished;
		HANDLE event_namedpipeReady;

		Data() {
			ZeroMemory(&overlapped, sizeof(overlapped));
			overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			//event_readFinished = CreateEvent(NULL, TRUE, FALSE, NULL);
			event_namedpipeReady = CreateEvent(NULL, TRUE, FALSE, NULL);
			buffer = (char*)VirtualAlloc(0, buffer_size,
				MEM_COMMIT, PAGE_READWRITE);
		}
		~Data() {
			VirtualFree(buffer, 0, MEM_RELEASE);
			buffer = nullptr;
			CloseHandle(overlapped.hEvent);
			//CloseHandle(event_readFinished);
			CloseHandle(event_namedpipeReady);
		}
		Data(const Data&) = delete;
	};
	static std::map<size_t, Data*> mapping;


#if 0
	// 完成例程，当异步读取完成时调用  
	static void CALLBACK PipeReadCompletionRoutine(
		DWORD dwErrorCode,
		DWORD dwNumberOfBytesTransferred,
		LPOVERLAPPED lpOverlapped
	) {
		Data* context = reinterpret_cast<Data*>(lpOverlapped);
		context->bytesRead = dwNumberOfBytesTransferred;
		if (dwErrorCode == ERROR_SUCCESS) {
			// 数据读取成功，准备发送（这里假设callback是线程安全的）  
			std::string data(context->buffer, context->bytesRead);
			wstring wide = s2ws(data);
			if (context->callback_fn)
				context->callback_fn(wide, context->callback_paramter);
			(*context->result) += wide;
		}

		context->overlappedDone = true;
		SetEvent(context->overlapped.hEvent); // 通知主线程读取已完成  
	}
#endif

	// 异步读取管道数据的函数  
	static BOOL StartAsyncPipeRead(HANDLE hPipe, Data* context) {
		DWORD flags = 0;
		if (!ReadFile(hPipe, context->buffer,
			buffer_size - 1, &context->bytesRead,
			&context->overlapped
		)) {
			if (GetLastError() == ERROR_IO_PENDING) {
				WaitForSingleObject(context->overlapped.hEvent, INFINITE);
				ResetEvent(context->overlapped.hEvent);
			}
			else {
				// 读取失败且不是挂起状态，处理错误
				LOG_ERROR << "pipe read error: " << GetLastError();
				return FALSE;
			}
		}

		// 读取已完
		std::string data(context->buffer, context->bytesRead);
		wstring wide = s2ws(data);
		if (context->callback_fn)
			context->callback_fn(wide, context->callback_paramter);
		(*context->result) += wide;

		return TRUE;
	}

	static DWORD WINAPI pipe_server(LPVOID lpParam) {
		LOG_ERROR << "pipe server started";

		size_t sessionId = (size_t)lpParam;
		Data* data = NULL;
		HANDLE hPipe;
		try {
			data = mapping.at(sessionId);
			if (!data) throw - 1;
			hPipe = data->hPipe;
			if (!hPipe) throw - 1;
		}
		catch (...) {
			LOG_ERROR << "pipe server died: eip";
			return ERROR_INVALID_PARAMETER;
		}

		{
			OVERLAPPED overlapped{};
			overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (!overlapped.hEvent) {
				LOG_ERROR << "pipe server died: failed to create event";
				CloseHandle(hPipe);
				return GetLastError();
			}
			SetEvent(data->event_namedpipeReady);
			if (!ConnectNamedPipe(hPipe, &overlapped)) {
				DWORD error = GetLastError();
				if (error == ERROR_IO_PENDING) {
					LOG_ERROR << "pipe server wait connect";
					while (!HasOverlappedIoCompleted(&overlapped)) {
						WaitForSingleObject(overlapped.hEvent, INFINITE);
						ResetEvent(overlapped.hEvent);
					}
					LOG_ERROR << "pipe server wait connect done";
				}
				else if (error != ERROR_PIPE_CONNECTED) {
					CloseHandle(overlapped.hEvent);
					CloseHandle(hPipe);
					LOG_ERROR << "pipe server died: connect";
					return GetLastError();
				}
			}
			CloseHandle(overlapped.hEvent);
		}

		LOG_ERROR << "pipe server alive L2720";
		DWORD bytesRead = 0;
		data->result->clear();
		while (1) {
#if 0
			memset(buffer, 0x00, buffer_size);
			if (!ReadFile(
				hPipe, buffer, buffer_size, &bytesRead, NULL
			)) break;

			wstring str = s2ws(buffer);
			data->result->append(str);

			if (data->callback_fn) {
				data->callback_fn(str, data->callback_paramter);
			}
#else
			// 不断启动异步读取
			if (!StartAsyncPipeRead(hPipe, data)) {
				break; // 读取失败，退出循环  
			}
#endif
		}

		LOG_ERROR << "pipe server exit 0";
		CloseHandle(hPipe);
		return 0;
	}
}
bool GetProcessStdOutputWithExitCodeEnhanced(
	std::wstring cmd_line, DWORD* dwExitCode, std::wstring& output,
	bool NoWindow,
	void(__stdcall* callback)(std::wstring&, PVOID), PVOID callbackParamter
) {
	using namespace GetProcessStdOutputWithExitCodeEnhancedNs;

	wstring tempFile = L"\\\\.\\pipe\\" + GenerateUUIDW();
	size_t sessionId = ++session_id_generator;
	Data* ptrData = new Data;
	mapping.insert(make_pair(sessionId, ptrData));
	Data& data = *mapping.at(sessionId);
	data.callback_fn = callback;
	data.callback_paramter = callbackParamter;
	data.result = &output;

	HANDLE hPipe;
	{
		SECURITY_ATTRIBUTES sa{
			.nLength = sizeof(sa),
			.bInheritHandle = TRUE,
		};
		hPipe = CreateNamedPipeW(
			tempFile.c_str(), PIPE_ACCESS_DUPLEX |
			FILE_FLAG_FIRST_PIPE_INSTANCE | FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
			2, 65536, 65536, 0, &sa);
		if (hPipe == INVALID_HANDLE_VALUE) {
			mapping.erase(sessionId);
			delete ptrData;
			return false;
		}

		data.hPipe = hPipe;
		//CloseHandle(hPipe);
	}
	const auto cleanPipe = [&] {
		HANDLE hFile = CreateFileW(tempFile.c_str(), GENERIC_READ,
			0, 0, OPEN_EXISTING, 0, 0);
		if (hFile && hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		CloseHandle(hPipe);
		mapping.erase(sessionId);
		delete ptrData;
		};

	SECURITY_ATTRIBUTES sa{
		.nLength = sizeof(sa),
		.bInheritHandle = TRUE,
	};
	HANDLE hWrite = CreateFileW(tempFile.c_str(), GENERIC_ALL, 0,
		&sa, OPEN_EXISTING, 0, 0);
	if (!hWrite || hWrite == INVALID_HANDLE_VALUE) {
		cleanPipe();
		return false;
	}
	DWORD dwMode = PIPE_READMODE_MESSAGE | PIPE_NOWAIT;
	SetNamedPipeHandleState(hWrite, &dwMode, NULL, NULL);
	{
		HANDLE hThread = CreateThread(NULL, 0, pipe_server,
			(LPVOID)sessionId, 0, NULL);
		if (hThread == NULL) {
			mapping.erase(sessionId);
			CloseHandle(hPipe);
			CloseHandle(hWrite);
			delete ptrData;
			return false;
		}
		CloseHandle(hThread);
	}
	WaitForSingleObject(data.event_namedpipeReady, 60000);

	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = hWrite;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW |
		STARTF_USESTDHANDLES;
	if (!CreateProcessW(NULL, (LPWSTR)(cmd_line.c_str())
		, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		cleanPipe();
		CloseHandle(hWrite);
		return false;
	}
	ResumeThread(pi.hThread);

	WaitForSingleObject(pi.hProcess, INFINITE);
	if (dwExitCode) GetExitCodeProcess(pi.hProcess, dwExitCode);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hWrite);
	cleanPipe();

	return true;
}
#pragma endregion
#pragma region 以下是能用的同步读取代码

namespace GetProcessStdOutputWithExitCodeEnhancedNs {
	static std::atomic<size_t> session_id_generator;
	struct Data
	{
		HANDLE hPipe = NULL;
		wstring* result = nullptr;
		void(__stdcall* callback_fn)(std::wstring&, PVOID) = nullptr;
		PVOID callback_paramter = NULL;
	};
	static std::map<size_t, Data> mapping;
	constexpr SIZE_T buffer_size = 16384;

	static DWORD WINAPI pipe_server(LPVOID lpParam) {
		size_t sessionId = (size_t)lpParam;
		Data* data = NULL;
		HANDLE hPipe;
		try {
			data = &mapping.at(sessionId);
			if (!data) throw - 1;
			hPipe = data->hPipe;
			if (!hPipe) throw - 1;
		}
		catch (...) {
			return ERROR_INVALID_PARAMETER;
		}

		if (!ConnectNamedPipe(hPipe, NULL)) {
			if (GetLastError() != ERROR_PIPE_CONNECTED) {
				CloseHandle(hPipe);
				return GetLastError();
			}
		}

		char* buffer = (char*)VirtualAlloc(0, buffer_size,
			MEM_COMMIT, PAGE_READWRITE);
		if (!buffer) {
			CloseHandle(hPipe);
			return ERROR_OUTOFMEMORY;
		}

		DWORD bytesRead = 0;
		data->result->clear();
		while (1) {
			memset(buffer, 0x00, buffer_size);
			if (!ReadFile(
				hPipe, buffer, buffer_size, &bytesRead, NULL
			)) break;

			wstring str = s2ws(buffer);
			data->result->append(str);

			if (data->callback_fn) {
				data->callback_fn(str, data->callback_paramter);
			}
		}

		CloseHandle(hPipe);
		VirtualFree(buffer, 0, MEM_RELEASE);
		return 0;
	}
}
bool GetProcessStdOutputWithExitCodeEnhanced(
	std::wstring cmd_line, DWORD* dwExitCode, std::wstring& output,
	bool NoWindow,
	void(__stdcall* callback)(std::wstring&, PVOID), PVOID callbackParamter
) {
	using namespace GetProcessStdOutputWithExitCodeEnhancedNs;

	wstring tempFile = L"\\\\.\\pipe\\" + GenerateUUIDW();
	size_t sessionId = ++session_id_generator;
	mapping.insert(make_pair(sessionId, Data()));
	Data& data = mapping.at(sessionId);
	data.callback_fn = callback;
	data.callback_paramter = callbackParamter;
	data.result = &output;

	HANDLE hPipe;
	{
		SECURITY_ATTRIBUTES sa{
			.nLength = sizeof(sa),
			.bInheritHandle = TRUE,
		};
		hPipe = CreateNamedPipeW(
			tempFile.c_str(), PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			1, 65536, 65536, 0, &sa);
		if (hPipe == INVALID_HANDLE_VALUE) {
			mapping.erase(sessionId);
			return false;
		}

		data.hPipe = hPipe;
		HANDLE hThread = CreateThread(NULL, 0, pipe_server,
			(LPVOID)sessionId, 0, NULL);
		if (hThread == NULL) {
			mapping.erase(sessionId);
			CloseHandle(hPipe);
			return false;
		}

		CloseHandle(hThread);
		//CloseHandle(hPipe);
	}
	const auto cleanPipe = [&] {
		HANDLE hFile = CreateFileW(tempFile.c_str(), GENERIC_READ,
			0, 0, OPEN_EXISTING, 0, 0);
		if (hFile && hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
		CloseHandle(hPipe);
		mapping.erase(sessionId);
		};

	SECURITY_ATTRIBUTES sa{
		.nLength = sizeof(sa),
		.bInheritHandle = TRUE,
	};
	HANDLE hWrite = CreateFileW(tempFile.c_str(), GENERIC_ALL, 0,
		&sa, OPEN_EXISTING, 0, 0);
	if (!hWrite || hWrite == INVALID_HANDLE_VALUE) {
		cleanPipe();
		return false;
	}

	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = hWrite;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite;
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESHOWWINDOW |
		STARTF_USESTDHANDLES;
	if (!CreateProcessW(NULL, (LPWSTR)(cmd_line.c_str())
		, NULL, NULL, TRUE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
		cleanPipe();
		CloseHandle(hWrite);
		return false;
	}
	ResumeThread(pi.hThread);

	WaitForSingleObject(pi.hProcess, INFINITE);
	if (dwExitCode) GetExitCodeProcess(pi.hProcess, dwExitCode);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hWrite);
	cleanPipe();

	return true;
}
#pragma endregion

#endif




namespace pipeutil {
	static std::atomic<size_t> session_id_generator;
	class Data {
	public:
		HANDLE hPipe = NULL;
		string* result = nullptr;
		void(__stdcall* callback_fn)(std::string&, PVOID) = nullptr;
		PVOID callback_paramter = NULL;

		HANDLE event_pipeReady;

		Data() {
			event_pipeReady = CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		~Data() {
			if (event_pipeReady) {
				SafeCloseHandle(event_pipeReady);
			}
		}
		Data(const Data&) = delete;
		Data& operator=(const Data&) = delete;
		Data(Data&& origin) = default;
		Data& operator=(Data&&) = default;
	};
	static std::map<size_t, Data> mapping;
	constexpr SIZE_T buffer_size = 16384;

	static DWORD WINAPI pipe_server(LPVOID lpParam) {
		size_t sessionId = (size_t)lpParam;
		Data* data = NULL;
		HANDLE hPipe;
		try {
			data = &mapping.at(sessionId);
			if (!data) throw - 1;
			hPipe = data->hPipe;
			if (!hPipe) throw - 1;
		}
		catch (...) {
			return ERROR_INVALID_PARAMETER;
		}

		SetEvent(data->event_pipeReady);

		while (1) {
			if (ConnectNamedPipe(hPipe, NULL)) break;

			if (GetLastError() == ERROR_PIPE_CONNECTED) break;
			if (GetLastError() == ERROR_PIPE_LISTENING) {
				Sleep(100); continue;
			}

			CloseHandle(hPipe);
			return GetLastError();
		}

		using mychar = char;
		mychar* buffer = (mychar*)VirtualAlloc(0, buffer_size,
			MEM_COMMIT, PAGE_READWRITE);
		if (!buffer) {
			CloseHandle(hPipe);
			return ERROR_OUTOFMEMORY;
		}

		DWORD bytesRead = 0;
		data->result->clear();
		string str;
		while (1) {
			memset(buffer, 0x00, buffer_size);
			if (!ReadFile(
				hPipe, buffer, buffer_size, &bytesRead, NULL
			)) {
				if (GetLastError() == ERROR_MORE_DATA) {
					DWORD memNeeded = 0;
					PeekNamedPipe(hPipe, NULL, NULL, NULL, NULL, &memNeeded);
					if (!memNeeded) break;
					mychar* buffer = (mychar*)calloc(memNeeded, sizeof(mychar));
					if (!buffer) break;
					if (ReadFile(
						hPipe, buffer, memNeeded, &bytesRead, NULL
					)) {
						str = string(buffer, bytesRead / sizeof(mychar));
					}
					free(buffer);
				}
				else break;
			}
			// pipe client should use wstring mode
			else str = string(buffer, bytesRead / sizeof(mychar));

#if 0
			if (bytesRead == 8 && buffer[0] == 0xdd && buffer[1] == 0x53) {
				char serial[8]{ 
					(char)0xdd, (char)0x53, (char)0x46, (char)0x16,
					(char)0xc8, (char)0x44, (char)0xac, (char)0x99
				};
				DWORD d[1]{}, n = 1, pn = 0;
				for (int i = 0; i < 8; ++i) if (buffer[i] != serial[i]) goto d;
				WriteFile(hPipe, d, n, &pn, 0);
				break; d:;
			}
#endif

			if (data->callback_fn) {
				// allowing callback to modify the content
				data->callback_fn(str, data->callback_paramter);
			}
			data->result->append(str);
		}

		CloseHandle(hPipe);
		VirtualFree(buffer, 0, MEM_RELEASE);
		mapping.erase(sessionId);
		return 0;
	}

	bool CreateAndReadPipeAsync(
		wstring pipeName, string* result, HANDLE& hEvent,
		void(__stdcall* callback)(std::string&, PVOID),
		PVOID callbackParamter
	) {
		size_t sessionId = ++session_id_generator;
		{
			std::pair<size_t, Data> myPair;
			myPair.first = sessionId;
			mapping.insert(move(myPair));
		}
		Data& data = mapping.at(sessionId);
		data.callback_fn = callback;
		data.callback_paramter = callbackParamter;
		data.result = result;

		HANDLE hPipe;
		hPipe = CreateNamedPipeW(
			pipeName.c_str(), PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			1, 16384, 16384, 0, NULL);
		if (hPipe == INVALID_HANDLE_VALUE || !hPipe) {
			mapping.erase(sessionId);
			return false;
		}
		data.hPipe = hPipe;
		const auto cleanPipe = [&] {
			HANDLE hFile = CreateFileW(pipeName.c_str(), GENERIC_READ,
				0, 0, OPEN_EXISTING, 0, 0);
			if (hFile && hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
			mapping.erase(sessionId);
		};


		HANDLE hThread = CreateThread(NULL, 0, pipe_server,
			(LPVOID)sessionId, 0, NULL);
		if (hThread == NULL) {
			mapping.erase(sessionId);
			CloseHandle(hPipe);
			return false;
		}
		//WaitForSingleObject(data.event_pipeReady, 10000);
		WaitForSingleObject(data.event_pipeReady, 50);

		//WaitForSingleObject(hThread, 60000); // 等待数据全部读完
		//CloseHandle(hThread);
		hEvent = hThread;
		//cleanPipe();

		return true;

	}
}



LONG MyVerifyBinarySignature(LPCWSTR pwszSourceFile) {
    LONG lStatus;

    // 初始化 WINTRUST_FILE_INFO 结构
    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = pwszSourceFile;
    FileData.hFile = NULL;
    FileData.pgKnownSubject = NULL;

    // 初始化 WINTRUST_DATA 结构
    WINTRUST_DATA WinTrustData;
    memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WINTRUST_DATA);
    WinTrustData.pPolicyCallbackData = NULL;
    WinTrustData.pSIPClientData = NULL;
    WinTrustData.dwUIChoice = WTD_UI_NONE;
	WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    WinTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    WinTrustData.hWVTStateData = NULL;
    WinTrustData.pwszURLReference = NULL;
    WinTrustData.dwProvFlags = WTD_SAFER_FLAG;
    WinTrustData.dwUIContext = 0;
    WinTrustData.pFile = &FileData;

	// 使用 WINTRUST_ACTION_GENERIC_VERIFY_V2 策略验证文件签名
	GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

    lStatus = WinVerifyTrust(
        NULL,
        &WVTPolicyGUID,
        &WinTrustData);

    // 清理
    WinTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);

    return (lStatus == 0) ? 0 : lStatus;
}








