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
十年生死两茫茫，写程序，到天亮，
千行代码，Bug何处藏。
纵使上线又怎样，朝令改，夕断肠。
领导每天新想法，天天改，日日忙。
相顾无言，惟有泪千行。
每晚灯火阑珊处，程序员，加班狂。
*/
#pragma once
#ifndef __cplusplus
#error "tool.h is a C++ header file"
#endif

#include<iostream>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<vector>
#include<random>
#include<string>
#include<xstring>
#include<deque>
#include<queue>
#include<map>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
#include<errno.h>
#include<algorithm>
#include<signal.h>
#include<limits.h>
#include<locale.h>
#include<tchar.h>
#include<ctime>
#include<typeinfo>
#include<assert.h>
#include<thread>
#include<version>
#include<chrono>
#include<mutex>
#include<any>
#if defined(_MSVC_LANG) && (_MSVC_LANG > 201703L)
#include<numbers>
#include<source_location>
#include<span>
#endif

#ifdef _WIN32  // Windows headers
#ifndef _WINDOWS_
#include<Windows.h>
#include<WindowsX.h>
#endif
#include<TlHelp32.h>
#include<ShObjIdl.h>
#include<winsvc.h>
#include<VersionHelpers.h>
#include<Shlwapi.h>
#include <WtsApi32.h>
#include <userenv.h>
#include <wintrust.h>
#include <softpub.h>
#endif // #ifdef _WIN32  // Windows headers

#if 0
#include<format>
#endif
#ifdef _WIN32
#include<Rpc.h>
#pragma comment(lib,"Rpcrt4.lib")
#else
//编译链接时，需要加入 -luuid
#include <uuid/uuid.h>
#endif

#ifdef _MSVC_LANG
#if 0 && !(_MSVC_LANG > 201703L)
#error "The tool.h needs C++20 or later."
#endif
#elif defined __cplusplus
#if !(__cplusplus > 201703L) && 0
#error "The tool.h needs C++20 or later."
#endif
#else
#error "No C++ supported"
#endif

//Main
#if (!(defined(_TOOL_SHC0743_MAIN)))
#define _TOOL_SHC0743_MAIN

#if 0
using namespace std;
#endif

/*MFC Error Method*
* Debug
uafxcwd.lib;LIBCMTD.lib;
LIBCMTD.lib;uafxcwd.lib;
* Release
uafxcw.lib;LIBCMT.lib;
LIBCMT.lib;uafxcw.lib;
*End*/

//Define Reds
#if defined(UNICODE)
using TSTRING = std::wstring;
#define _C_AUTOTEXT(text) L ## text
#define STRCMP lstrcmpW
#define strcpyt lstrcpyW
#define strlen_t lstrlenW
#define tostr ws2s
#else
using TSTRING = std::string;
#define _C_AUTOTEXT(text) text
#define STRCMP strcmp
#define strcpyt strcpy
#define strlen_t strlen
#define tostr 
#endif

#if defined(UNICODE)
#define CF_AUTOTEXT CF_UNICODETEXT
#define _AUTOTEXT(text) L ## text
using C_TSTR = std::wstring;
using FSTREAM_T = std::wfstream;
using BASSTRING = wchar_t;
#define atoi_t(x) atoi(ws2s(x).c_str())
#else
#define CF_AUTOTEXT CF_TEXT
#define _AUTOTEXT(text) text
using C_TSTR = std::string;
using FSTREAM_T = std::fstream;
using BASSTRING = char;
#define atoi_t(x) atoi(x)
#endif

#ifndef debugger
//Pause and wait for keyboard if IsDebuggerPresent
#define debugger if (IsDebuggerPresent()) { fflush(stdin);\
				 puts("Debugger Paused.\nPress any key to continue...");\
				 ((void)(_getch())); _putch('\n'); }
#endif

#ifndef fcreate
//Create a file
#define fcreate(fileName) fclose(fopen(fileName,"wb+"))
#endif

#ifndef codecat
#define codecat(a,b) a ## b
#endif

#ifndef codetostr
#define codetostr(c) #c
#endif

#ifndef force_cast
#define force_cast(type,var) (*reinterpret_cast<type*>(&(var)))
#endif
#ifndef force_castr
#define force_castr(type,var) (reinterpret_cast<type>(var))
#endif

#ifndef echo
#define echo std::cout << 
#endif

#ifndef pause
#define pause {puts("Press any key to continue...");(void)_getch();}
#endif

#ifndef _PWARNING_PUSH
#define _PWARNING_PUSH __pragma(warning(push))
#endif

#ifndef _DISABLE_WARNING
#define _DISABLE_WARNING(id) __pragma(warning(disable: id))
#endif

#ifndef _PWARNING_POP
#define _PWARNING_POP __pragma(warning(pop))
#endif

#ifndef ThisInst
#define ThisInst (GetModuleHandle(NULL))
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)
#endif

#ifndef ICC_ALL_CLASSES
#define ICC_ALL_CLASSES \
ICC_LISTVIEW_CLASSES |\
ICC_TREEVIEW_CLASSES |\
ICC_BAR_CLASSES |\
ICC_TAB_CLASSES |\
ICC_UPDOWN_CLASS |\
ICC_PROGRESS_CLASS |\
ICC_HOTKEY_CLASS |\
ICC_ANIMATE_CLASS |\
ICC_WIN95_CLASSES |\
ICC_DATE_CLASSES |\
ICC_USEREX_CLASSES |\
ICC_COOL_CLASSES |\
ICC_INTERNET_CLASSES |\
ICC_PAGESCROLLER_CLASS |\
ICC_NATIVEFNTCTL_CLASS |\
ICC_STANDARD_CLASSES |\
ICC_LINK_CLASS
#endif

#if 0
#ifndef AND
#define AND &
#endif
#ifndef OR
#define OR |
#endif
#ifndef NOT
#define NOT ~
#endif
#ifndef XOR
#define XOR ^
#endif
#ifndef LAND
#define LAND &&
#endif
#ifndef LOR
#define LOR ||
#endif
#ifndef LNOT
#define LNOT !
#endif
#endif

/**
@brief 替换字符串|Replace String
@param strBase 待修改的字符串|String to be modified
@param strSrc 要替换的内容|What to replace
@param strDes 替换后的内容|Replaced content
@return strBase
*/
std::string& str_replace(std::string& strBase, 
	const std::string strSrc, const std::string strDes);
/**
@brief 替换字符串|Replace String
@param strBase 待修改的字符串|String to be modified
@param strSrc 要替换的内容|What to replace
@param strDes 替换后的内容|Replaced content
@return strBase
*/
std::wstring& str_replace(std::wstring& strBase,
	const std::wstring strSrc, const std::wstring strDes);

//Parameter 1: std::string to be split; Parameter 2: character as separator
//参数1：要分割的字符串；参数2：作为分隔符的字符
std::vector<std::string>& str_split(
	const std::string& src, 
	const std::string separator,
	std::vector<std::string>& dest);
//Parameter 1: std::string to be split; Parameter 2: character as separator
//参数1：要分割的字符串；参数2：作为分隔符的字符
std::vector<std::wstring>& str_split(
	const std::wstring& src, 
	const std::wstring separator, 
	std::vector<std::wstring>& dest);

// std::string to std::wstring
std::wstring s2ws(const std::string);
// std::wstring to std::string
std::string ws2s(const std::wstring);
// std::string to wchar_t*
#define s2wc(x) (s2ws(x).c_str())
// std::wstring to char*
#define ws2c(x) (ws2s(x).c_str())

#ifdef UNICODE
inline std::wstring to__str(const std::string& str) { return s2ws(str); }
inline std::wstring to__str(const std::wstring& str) { return str; }
#else
inline std::string to__str(const std::string& str) { return str; }
inline std::string to__str(const std::wstring& str) { return ws2s(str); }
#endif

extern "C" {
int WINAPI MessageBoxTimeoutA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
	UINT uType, WORD wLanguageId, DWORD dwMilliseconds);
int WINAPI MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption,
	UINT uType, WORD wLanguageId, DWORD dwMilliseconds);
};

// constexpr auto null = nullptr;

inline bool file_exists (const std::string& name) {
	struct stat buffer;   
	return (stat(name.c_str(), &buffer) == 0); 
}
inline bool file_exists (const std::wstring& name) {
	struct _stat64i32 stat;
	return (_wstat(name.c_str(), &stat) == 0); 
}

inline bool SetConsoleColor(WORD wAttr) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE) return FALSE;
	return SetConsoleTextAttribute(hConsole, wAttr);
}
/**
	第一个对应于背景，第二个对应于前景。
	0 = 黑色       8 = 灰色
	1 = 蓝色       9 = 淡蓝色
	2 = 绿色       A = 淡绿色
	3 = 浅绿色     B = 淡浅绿色
	4 = 红色       C = 淡红色
	5 = 紫色       D = 淡紫色
	6 = 黄色       E = 淡黄色
	7 = 白色       F = 亮白色

	==========================

	The first corresponds to the background and the second corresponds to the foreground.
	0 = black           8 = gray
	1 = Blue            9 = light blue
	2 = green           A = light green
	3 = light green     B = light green
	4 = Red             C = light red
	5 = purple          D = Lavender
	6 = yellow          E = light yellow
	7 = white           F = bright white
*/
#define SetConsColor_(ColorVal) SetConsoleColor(0x ## ColorVal)
#define SetConsColor(ColorVal) SetConsColor_(ColorVal)

//class colorset {
//public:
//	colorset() = delete;
//	colorset(const colorset&) = delete;
//	colorset(colorset&&) = delete;
//	~colorset() = default;
//public:
//	static void r() { SetConsColor(04); }
//	static void g() { SetConsColor(02); }
//	static void b() { SetConsColor(01); }
//	static void rst() { SetConsColor(07); }
//};

[[nodiscard("Ignoring the return value will make the function useless")]]
bool IsRunAsAdmin();
[[nodiscard("Ignoring the return value will make the function useless")]]
std::string GetProgramDir();
[[nodiscard("Ignoring the return value will make the function useless")]]
std::string GetProgramPath();
[[nodiscard("Ignoring the return value will make the function useless")]]
std::wstring GetProgramDirW();
[[nodiscard("Ignoring the return value will make the function useless")]]
std::wstring GetProgramPathW();
using ProgramInfo = struct{
	std::string dir;
	std::string path;
	std::string name;
};
[[nodiscard("Ignoring the return value will make the function useless")]]
ProgramInfo GetProgramInfo();

#ifdef _WIN32
/*
函数功能：释放资源文件

		  DWORD dwResName   指定要释放的资源ID号，如IDR_EXE
		  std::string  lpResType 指定释放的资源的资源类型
		  std::wstring lpFilePathName 指定释放后的目标文件名

返回值：成功则返回TRUE,失败返回FALSE  

Function function: release resource file
		DWORD  dwresname      specifies the ID number of the resource to be released,
							  such as IDR_EXE
		std::wstring  lprestype     specifies the resource type of the released resource
		std::wstring lpfilepathname specifies the destination file name after release
Return value: true for success and false for failure
*/
bool FreeResFile(DWORD dwResName, std::wstring lpResType,
	std::wstring lpFilePathName, HMODULE hInst = nullptr);
inline bool FreeResFile(DWORD dwResName, std::string lpResType,
	std::wstring lpFilePathName, HMODULE hInst = nullptr) {
	return FreeResFile(dwResName, s2ws(lpResType), lpFilePathName, hInst);
}
inline bool FreeResFile(DWORD dwResName, std::string lpResType,
	std::string lpFilePathName, HMODULE hInst = nullptr) {
	return FreeResFile(dwResName, s2ws(lpResType), s2ws(lpFilePathName), hInst);
}
#endif

////Restart current process
//void __stdcall ReStart(int _Code = 0);

////Detect whether it exists
////检测自身是否存在
//bool CheckSelfProcessExists();
////Detect the number of specified processes
////检测指定进程存在数量
//size_t CheckProcessCount(TSTRING pname);

#if 0
//ChineseProgramming
#ifdef _CHINESEPROG
#ifndef _NOCHINESEPROG
#define _NOCHINESEPROG
using 整数 = int;
using 小数 = float;
using 浮点数 = double;
typedef char 字符;
typedef char* 字符串指针;
using 字符数组=char[];
using 字符串=std::string;
template <class T,int size>
using 数组=std::array<T,size>;
template <class T>
using 向量数组=std::vector<T>;
template <class T>
using 队列=std::deque<T>;
#endif
#endif
//EndCP
#endif

//Clear Display
#ifdef _WIN32
//Clear Windows Console
#define cls() (system("cls"))
#else defined __linux__
//Clear Linux Console
#define cls() (system("clear"))
#endif

#if ST_ENABLE_Sleep_sleep
//Sleep
#ifdef __linux__
#define Sleep(int_time_millsecond) (sleep((int_time_millsecond)/1000))
#elif _WIN32
#define sleep(int_time_second) (Sleep((DWORD)((int_time_second)*1000)))
#endif
#endif

//using str_vector = std::vector<std::string>;

template <class T>
using var = T;
template <class T>
using CreateVar = T;

//#define StartCreateObjArea() ;
#define CreateObjectNew(TYPENAME,VARNAME) ;TYPENAME* VARNAME = new TYPENAME;
#define EndObjectNew(__VAR__NAME__) ;delete __VAR__NAME__;
//#define EndCreateObjArea() ;

#define CreateMFCDialogNoModal(MFCClass,DlgVar,CreateFrom) ;\
CreateObjectNew(MFCClass,DlgVar)\
DlgVar->Create(CreateFrom);\
DlgVar->ShowWindow(SW_NORMAL);
#define CreateMFCDialogModal(MFCClass,DlgVar) ;\
MFCClass DlgVar;\
DlgVar.DoModal();



template<typename str>
class CmdLine_t {
public:
	CmdLine_t() {
		this->data = new std::vector<str>;
	};
	CmdLine_t(str src) {
		this->data = new std::vector<str>;
		this->parse(src);
	};
	CmdLine_t(const CmdLine_t<str>& cp) {
		this->data = new std::vector<str>(*(cp.data));
	};
	CmdLine_t(CmdLine_t<str>&& mv) {
		this->data = mv->data;
		mv->data = nullptr;
	};
	~CmdLine_t(){
		delete this->data;
	};
	CmdLine_t<str>& operator=(const CmdLine_t<str>&cp) {
		if (this->data) delete this->data;
		this->data = new std::vector<str>(*(cp.data));
		return *this;
	};
	using value_type = str;
public:
	//Parse Command-Line
	bool parse(str t) {
		CmdLine_t<str>& willbe = *this;
		const unsigned int len = (unsigned int)(t.length()); bool inquote = false;
		if (typeid(str) == typeid(std::wstring)) {
			std::wstring t2 = *reinterpret_cast<std::wstring*>(&t);
			std::wstring dat = L"";
			for (unsigned int i = 0; i < len; ++i) {
				if (t2[i] == L'"') { inquote = !inquote; continue; }
				if (!inquote && t2[i] == L' ' && dat.length() > 0) {
					willbe.data->push_back(*reinterpret_cast<str*>(&dat));
					dat = L""; continue;
				}
				if (!inquote && t2[i] == L' ') { dat = L""; continue; }
				dat += t2[i];
			}
			std::wstring dat2 = (dat);
			if (str_replace(dat2, L" ", L"").length() > 0)
				willbe.data->push_back(*reinterpret_cast<str*>(&dat));
		}
		else if (typeid(str) == typeid(std::string)) {
			std::string t2 = *reinterpret_cast<std::string*>(&t);
			std::string dat = "";
			for (unsigned int i = 0; i < len; ++i) {
				if (t2[i] == '"') { inquote = !inquote; continue; }
				if (!inquote && t2[i] == ' ' && dat.length() > 0) {
					willbe.data->push_back(*reinterpret_cast<str*>(&dat));
					dat = ""; continue;
				}
				if (!inquote && t2[i] == ' ') { dat = ""; continue; }
				dat += t2[i];
			}
			std::string dat2 = (dat);
			if (str_replace(dat2, " ", "").length() > 0)
				willbe.data->push_back(*reinterpret_cast<str*>(&dat));
		}
		else return false;
		return true;
	};
	/**
	@brief getopt
	@param opt Command line parameter name
	@param resBuffer Resolved params. If it cannot be resolved or no any param,\
	it is not changed.
	@return 0-Failed. 1-Parsing succeeded with parameters. 2-Parsing succeeded \
	without parameters.
	*/
	signed char getopt(str opt, str& resBuffer) {
#pragma warning(push)
#pragma warning(disable: 6276)
		str s, s2, stmp;
		if (typeid(str) == typeid(std::string))  
			s = reinterpret_cast<const str::value_type*>("-");
		if (typeid(str) == typeid(std::wstring)) 
			s = reinterpret_cast<const str::value_type*>(L"-");
		if (typeid(str) == typeid(std::string)) 
			s2 = reinterpret_cast<const str::value_type*>("=");
		if (typeid(str) == typeid(std::wstring)) 
			s2 = reinterpret_cast<const str::value_type*>(L"=");
		size_t datasz = data->size();
		if (opt.length() == 1) {
			for (size_t i = 0; i < datasz; ++i) {
				if (data->at(i).substr(0, 2) == s + opt) {
					stmp = data->at(i);
					resBuffer = stmp.erase(0, 2);
					return 1;
				}
			}
			return 0;
		}
		else {
			for (size_t i = 0; i < datasz; ++i) {
				if (data->at(i).substr(0, opt.length() + 3) == s + s + opt + s2) {
					stmp = data->at(i);
					resBuffer = stmp.erase(0, opt.length() + 3);
					return 1;
				}
				stmp = s + s; stmp += opt;
				if (data->at(i) == stmp) return 2;
			}
			return 0;
		}
		return 0;
	};
	/**
	@brief The version of 'getopt' without the resbuffer parameter.
	@param opt Command line parameter name
	@see int getopt(str opt, str& resBuffer)
	*/
	signed char getopt(str opt) {
		str tmp = str(); return getopt(opt, tmp);
	}
#pragma warning(pop)

	size_t argc(){ return this->data->size(); };
	size_t size(){ return this->data->size(); };
	size_t length(){ return this->data->size(); };
	operator size_t() { return this->data->size(); };
public:
	str& operator[](size_t zIndex) { 
		return this->data->operator[](zIndex); 
	};
	const str at(size_t zIndex) const { 
		return this->data->at(zIndex); 
	};
	str& at(size_t zIndex, str newData) { 
		return this->data->at(zIndex) = newData; 
	};
public:
	using v_it = typename std::vector<str>::iterator;
	using r_it = typename std::reverse_iterator<v_it>;
	using c_it = typename std::vector<str>::const_iterator;
	v_it begin() { return data->begin(); };
	v_it end() { return data->end(); };
	r_it rbegin() { return data->rbegin(); };
	r_it rend() { return data->rend(); };
	c_it cbegin() const { return data->cbegin(); };
	c_it cend() const { return data->cend(); };

	std::vector<str>& __get_data__() {
		return *data;
	};
protected:
	std::vector<str>* data;
};
using CmdLineW = CmdLine_t<std::wstring>;
using CmdLineA = CmdLine_t< std::string>;
#ifdef UNICODE
using CmdLine = CmdLineW;
#else
using CmdLine = CmdLineA;
#endif

//Get process handle by process ID
//PROCESS_ALL_ACCESS get all permissions
//PROCESS_QUERY_INFORMATION obtains the token, exit code, priority 
// and other information of the process
// -
//通过进程ID获取进程句柄
//PROCESS_ALL_ACCESS 获取所有权限
//PROCESS_QUERY_INFORMATION 获取进程的令牌、退出码和优先级等信息
HANDLE GetProcessHandle(DWORD nID);

//通过进程名（带后缀.exe）获取进程句柄
//Get the process handle through the process name (with suffix .exe)
HANDLE GetProcessHandle(LPCTSTR lpName);

//通过进程名（带后缀.exe）获取进程id 
//Get the process id through the process name (with suffix .exe)
DWORD GetProcessIdW(std::wstring lpName);
//通过进程名（带后缀.exe）获取进程id
//Get the process id through the process name (with suffix .exe)
inline DWORD GetProcessIdA(std::string lpName) {
	return GetProcessIdW(s2ws(lpName));
};
#ifdef UNICODE
#define GetProcessId GetProcessIdW
#else
#define GetProcessId GetProcessIdA
#endif
//通过进程名（带后缀.exe）获取进程id
//Get the process id through the process name (with suffix .exe)
std::vector<DWORD>* GetProcessIdFromAll(LPCTSTR lpName);

/***************************************************
	Function name:        EnableDebugPrivilege
	Function Description: administrator privilege lifting
	Method:               public
	Input parameter:      void
	Output parameter:     BOOL
	Return value:         true - authorization successful, 
	                      false - authorization failed
	Author:				  Unknown
	Created on:           November 5, 2018
	remarks:
***************************************************/
/***************************************************
	函数名称: EnableDebugPrivilege
	函数说明: 管理员权限提权
	方    式: public
	输入参数: void
	输出参数: BOOL
	返 回 值: true-提权成功，false-提权失败
	作    者: 
	创建时间: 2018-11-5
	备    注: 
***************************************************/
BOOL EnableDebugPrivilege();

// Enable All Privileges
BOOL EnableAllPrivileges();

//create a process (ShellExecuteEx) (DEPRECATED)
//void StartExecute(const TSTRING& cmd);

#if 0
class StringEx : public std::string {
public:
	StringEx() {};
	StringEx(std::string str) { *this = str; }
	StringEx(std::wstring str) { *this = /*(StringEx)*/ws2s(str); }
	operator char* () { return (char*)(this->c_str()); }
	operator const char* () { return (this->c_str()); }
//  StringEx& operator=(std::string str) 
// { *this = (StringEx)str; return *this; };
//  StringEx& operator=(std::string&);
	StringEx& operator=(StringEx&);
	StringEx& operator=(std::wstring&);
	StringEx& operator=(const char*);
	StringEx operator+(std::string&);
	StringEx operator+(StringEx&);
	StringEx operator+(std::wstring&);
	StringEx operator+=(std::string& argv) 
	{ return ((std::string)*this) += argv; };
	StringEx operator+=(StringEx& argv) 
	{ return ((std::string)*this) += (std::string)argv; };
	StringEx operator+=(std::wstring& argv) 
	{ return ((std::string)*this) += ws2s(argv); };
};
#endif

template<typename str,typename fs>
class _TempFile : public fs {
protected:
	str FilePathName;
	str WorkPath;
	bool NoRemove;
public:
	static std::wstring GetRdmFileNameW();
	static  std::string GetRdmFileNameA();
public:
	_TempFile();
	_TempFile(str fileName);
	~_TempFile();
public:
	void open(str fileName);
	void close() {
		return fs::close();
	};
public:
	str GetFilePathName();
};
using TempFileW = _TempFile<std::wstring, std::wfstream>;
using TempFileA = _TempFile< std::string, std:: fstream>;
#ifdef UNICODE
using TempFile = TempFileW;
#else
using TempFile = TempFileA;
#endif

int ExecBatch(TSTRING Batch);
int ExecBatch(std::vector<TSTRING>& Batch);

inline std::string  LastErrorStrA();
template<typename T> int AssertEx_(T d,
	int Line, 
	const char* File,
	bool AutoExec,
	bool bForce = false
) {
	bool b = (bool)d;
	if (b) return IDOK;
	SetConsColor(47);
	std::cerr << "AssertionError: File " << File << ",Line " << Line;
	SetConsColor(07);
	if (bForce) exit(IDABORT);
	std::stringstream ss; ss.str("");
	ss << "AssertionError!\n";
	ss << "The data is false." << std::endl;
	ss << std::endl << "At " << Line << ":" << File << ", AssertionError";
	ss << std::endl << "GetLastError == " << GetLastError() << ", " << LastErrorStrA();
	auto r = ::MessageBoxA(NULL, ss.str().c_str(),
		("AssertionError"), MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON1);
	if (AutoExec) {
		if (r == IDABORT) exit(IDABORT);
		return r;
	}
	else return r;
}
// be like assert(data), but can use in Release
#define AssertEx(data) (AssertEx_(data,__LINE__,__FILE__,false))
// be like assert(data), but can use in Release
#define AssertEx_AutoHandle(data) (AssertEx_(data,__LINE__,__FILE__,true))
 
#if 0
//class ServiceManager_;
class ServiceManager_{
protected:
	SC_HANDLE schSCManager;

public:
	bool InitSch();
	bool FreeSch();

public:
	//using std::literals::string_literals::operator ""s;
#pragma warning(push)
#pragma warning(disable: 26812)
	enum STARTUP_TYPE { 
		Undefined = 0,
		Auto = SERVICE_AUTO_START,
		Manual = SERVICE_DEMAND_START,
		Disabled = SERVICE_DISABLED};
#pragma warning(pop)
public:
	unsigned   Start(std::string  ServiceName);
	unsigned ReStart(std::string  ServiceName);
	unsigned   Stop (std::string  ServiceName);
	unsigned   Query(LPCTSTR ServiceName);
	unsigned Pause   (std::string ServiceName);
	unsigned Continue(std::string ServiceName);

	unsigned New(std::string name, std::string bin, STARTUP_TYPE stype = Manual,
		std::string display = std::string(""), std::string des = std::string(""),
		DWORD type = SERVICE_WIN32);
	unsigned New(std::wstring name, std::wstring bin, STARTUP_TYPE stype = Manual,
		std::wstring display = std::wstring(L""), std::wstring des = std::wstring(L""),
		DWORD type = SERVICE_WIN32);

	unsigned Remove(std::string name);
public:
	static constexpr unsigned OK                        = 0;
	static constexpr unsigned ERROR_UNKNOWN             = 1;
	static constexpr unsigned ERROR_OPEN_SCMANAGER      = 2;
	static constexpr unsigned ERROR_OPEN_SERVICE        = 3;
	static constexpr unsigned ERROR_QUERY_SERVICESTATUS = 4;
	static constexpr unsigned ERROR_STOP_SERVICE        = 5;
	static constexpr unsigned ERROR_START_SERVICE       = 6;
	static constexpr unsigned ERROR_IS_START            = 6;
	static constexpr unsigned ERROR_IS_STOP             = 7;
	static constexpr unsigned ERROR_CLOSE_SCMANAGER     = 8;
	static constexpr unsigned ERROR_CONTINUE_SERVICE    = 9;
	static constexpr unsigned ERROR_PAUSE_SERVICE       = 10;
	static constexpr unsigned ERROR_NEW_SERVICE         = 11;
	static constexpr unsigned ERROR_DEL_SERVICE         = 12;
public:
	static constexpr unsigned STATUS_BASE               = 64;
	static constexpr unsigned STATUS_START              = STATUS_BASE + 1;
	static constexpr unsigned STATUS_PAUSE              = STATUS_BASE + 2;
	static constexpr unsigned STATUS_STOP               = STATUS_BASE + 3;
	static constexpr unsigned STATUS_STARTING           = STATUS_BASE + 4;
	static constexpr unsigned STATUS_PAUSE_PENDING      = STATUS_BASE + 5;
	static constexpr unsigned STATUS_CONTINUE_PENDING   = STATUS_BASE + 6;
	static constexpr unsigned STATUS_STOPPING           = STATUS_BASE + 7;
};
extern ServiceManager_ ServiceManager;
#endif

class Process_t;
class Process_t {
public:
	Process_t() {
		LastFlush = 0;
		ntdll = LoadLibrary(_T("ntdll.dll"));
		//assert(ntdll);
		//EnableDebugPrivilege();
		ZwSuspendProcess = ZwResumeProcess = nullptr;
		_NtQueryInformationProcess = nullptr;
		_NtSetInformationProcess = nullptr;
		current_process_id = ::GetCurrentProcessId();
	};
	~Process_t() {
		this->close();
		prs.~vector();
		FreeLibrary(ntdll);
	};
public:
	typedef DWORD(WINAPI* SUSPENDPROCESS)(HANDLE);
	typedef DWORD(WINAPI*  RESUMEPROCESS)(HANDLE);
	using NTSTATUS = LONG;
	using ProcessHandle = void*;
	using PROCESSINFOCLASS = PROCESS_INFORMATION_CLASS;
	typedef struct
	{
		DWORD ExitStatus; // 接收进程终止状态
		DWORD PebBaseAddress; // 接收进程环境块地址
		DWORD AffinityMask; // 接收进程关联掩码
		DWORD BasePriority; // 接收进程的优先级类
		ULONG UniqueProcessId; // 接收进程ID
		ULONG InheritedFromUniqueProcessId; //接收父进程ID
	} PROCESS_BASIC_INFORMATION;
	using _NtTerminateProcess = DWORD(CALLBACK*)(HANDLE, UINT);
	using __NtSetInformationProcess = NTSTATUS(NTAPI*)(
		HANDLE           ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength);
	using __NtQueryInformationProcess = NTSTATUS(NTAPI*)(
		HANDLE           ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength,
		PULONG           ReturnLength);
	static constexpr PROCESSINFOCLASS ProcessBasicInformation      = (PROCESSINFOCLASS)0;
	static constexpr PROCESSINFOCLASS ProcessDebugPort             = (PROCESSINFOCLASS)7;
	static constexpr PROCESSINFOCLASS ProcessWow64Information      = (PROCESSINFOCLASS)26;
	static constexpr PROCESSINFOCLASS ProcessImageFileName         = (PROCESSINFOCLASS)27;
	static constexpr PROCESSINFOCLASS ProcessBreakOnTermination    = (PROCESSINFOCLASS)29;
	static constexpr PROCESSINFOCLASS ProcessSubsystemInformation  = (PROCESSINFOCLASS)75;
public:
	class ProcessInfo {
	public:
		ProcessInfo(
			TSTRING name = _T(""),
			DWORD id = 0, 
			DWORD parent = 0, 
			HANDLE handle = nullptr) :
			m_name(name),
			m_id(id),
			m_par(parent),
			handle(handle) {};
		ProcessInfo(const ProcessInfo& cpy) :
			m_name(cpy.m_name),
			m_id(cpy.m_id),
			m_par(cpy.m_par),
			handle(cpy.handle) {};
		ProcessInfo(const HANDLE hdl):m_name(_T("")),m_id(0),m_par(0),handle(hdl) {};
		~ProcessInfo() {
			if (this->handle) ([](HANDLE hObject) {
				__try {
					CloseHandle(hObject);
				}
				__except (EXCEPTION_EXECUTE_HANDLER) {}
			}) (this->handle);
		};
		ProcessInfo& operator=(ProcessInfo& cpy) {
			this->m_name = cpy.m_name;
			this->m_id = cpy.m_id;
			this->m_par = cpy.m_par;
			this->handle = cpy.handle;
			return cpy;
		};
	private:
		TSTRING m_name;
		DWORD  m_id;
		DWORD  m_par;
		HANDLE handle;
	public:
		friend class Process_t;
	public:
		operator HANDLE()  const  { return this->handle; };
		operator HANDLE()         { return this->Handle(); };
		operator DWORD()   const  { return this->m_id; };
		operator TSTRING()  const { return this->m_name; };
		bool operator!()   const  { return !((bool)(this->handle)); }
	public:
		bool valid() const {
			return (this->handle) ? (this->handle != INVALID_HANDLE_VALUE) : false;
		};
		const TSTRING& name() const { return m_name; };
		DWORD id() const { return this->m_id; };
		DWORD parent() const { return this->m_par; };
		HANDLE Handle();
	};
	class thread_t {
	public:
		friend class Process_t;
	private:
		thread_t() {};
	public:
		void exit(DWORD dwExitCode = 0) {
			return ExitThread(dwExitCode);
		};
		HANDLE open(DWORD thrd_id, DWORD access = THREAD_ALL_ACCESS) {
			return ::OpenThread(access, FALSE, thrd_id);
		};
		bool kill(HANDLE hThread, DWORD dwExitCode = 0) {
#pragma warning(push)
#pragma warning(disable: 6258)
			return TerminateThread(hThread, dwExitCode);
#pragma warning(pop)
		};
		DWORD suspend(HANDLE hThread) {
			return SuspendThread(hThread);
		};
		DWORD  resume(HANDLE hThread) {
			return  ResumeThread(hThread);
		};
	} thread;
protected:
	void checkFlushTime();
public:

	// Flush process list (recommended)
	bool flush(void);

	// Close handles and reset
	void close();

	// Get all processes and store in <dest>
	void find(std::vector<Process_t::ProcessInfo>& dest);
	// Find (a) process(es) named <pname>, and store in <dest>
	void find(TSTRING pname, std::vector<Process_t::ProcessInfo>& dest);
	// Find a process with process-id <pid>, and return it
	// If not exists, return NULL
	Process_t::ProcessInfo find(DWORD pid);
	Process_t::ProcessInfo GetCurrentProcess(void);
	DWORD GetCurrentProcessId() { return current_process_id; };

	// Find a process's windows and store in <dest>
	void findWindows(ProcessInfo process, std::vector<HWND>& dest);

	DWORD GetParentProcessId(DWORD dwProcessId);

	TSTRING GetProcessNameById(DWORD dwProcessId);
	TSTRING GetProcessFullPathById(DWORD dwProcessId);

	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR  Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;
	typedef struct _OBJECT_ATTRIBUTES {
		ULONG           Length;
		HANDLE          RootDirectory;
		PUNICODE_STRING ObjectName;
		ULONG           Attributes;
		PVOID           SecurityDescriptor;
		PVOID           SecurityQualityOfService;
	} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
	typedef struct _CLIENT_ID {
		 PVOID UniqueProcess;
		 PVOID UniqueThread;
	} CLIENT_ID, *PCLIENT_ID;


	NTSTATUS NtOpenProcess(
		PHANDLE ProcessHandle,
		ACCESS_MASK DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		PCLIENT_ID ClientId
	);
	NTSTATUS NtClose(HANDLE Handle);

	// NtTerminateProcess
	bool kill(const Process_t::ProcessInfo& exe, UINT exitCode = 0);
	// Kill a process with process-id <pid> with exit-code <exitCode>
	inline bool kill(const DWORD pid, UINT exitCode = 0) {
		return kill(find(pid), exitCode);
	};
	// Kill processes named <name>
	bool killall(TSTRING name, UINT code = 0);

	// call ::exit function (useless)
	inline void exit(int _Code = 0) {
		return ::exit(_Code);
	};

	[[nodiscard("Ignoring the return value will made C6335. "
		"If you want to only start a process, use \"StartOnly\".")]]
	// Create a process with command-line <cmd> with STARTUPINFO <si>
	PROCESS_INFORMATION Start (const TSTRING& cmd, STARTUPINFO& si);
	[[nodiscard("Ignoring the return value will made C6335. "
		"If you want to only start a process, use \"StartOnly\".")]]
	// Create a process with command-line <cmd>
	inline PROCESS_INFORMATION Start(const TSTRING& cmd) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		return Start(cmd, si);
	};
	[[nodiscard("Ignoring the return value will made C6335. "
		"If you want to only start a process, use \"StartOnly\".")]]
	// Create a process with command-line <cmd>
	// and you cannot see its window
	inline PROCESS_INFORMATION Start_HiddenWindow(const TSTRING& cmd) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		return Start(cmd, si);
	};

	[[nodiscard("Ignoring the return value will made C6335. "
		"If you want to only start a process, use \"StartOnly\".")]]
	// Create a suspended process with command-line <cmd> with STARTUPINFO <si>
	PROCESS_INFORMATION Start_Suspended (const TSTRING& cmd, STARTUPINFO& si);
	[[nodiscard("Ignoring the return value will made C6335. "
		"If you want to only start a process, use \"StartOnly\".")]]
	// Create a process with command-line <cmd>
	inline PROCESS_INFORMATION Start_Suspended(const TSTRING& cmd) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		return Start_Suspended(cmd, si);
	};

	// Create a process with command-line <cmd> with STARTUPINFO <si> and wait for it
	DWORD StartAndWait(const TSTRING& cmd, STARTUPINFO& si) {
		PROCESS_INFORMATION pi = Start_Suspended(cmd, si);
		if (!pi.hProcess) return -0xE;
		CloseHandle(pi.hThread);
		DWORD ret = 0;
		this->resume(pi.hProcess);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &ret);
		CloseHandle(pi.hProcess);
		return ret;
	}
	// Create a process with command-line <cmd> and wait for it
	inline DWORD StartAndWait(const TSTRING& cmd) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		return StartAndWait(cmd, si);
	};

	// Create a process with command-line <cmd> with STARTUPINFO <si>
	bool StartOnly (const TSTRING& cmd, STARTUPINFO& si);
	// Create a process with command-line <cmd>
	inline bool StartOnly(const TSTRING& cmd) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		return StartOnly(cmd, si);
	};
	// Create a process with command-line <cmd>
	// and you cannot see its window
	inline bool StartOnly_HiddenWindow(const TSTRING& cmd) {
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		return StartOnly(cmd, si);
	};

	[[nodiscard]] _Success_(return)
	BOOL StartAsActiveUserT(_In_opt_ LPCTSTR lpApplicationName,
		_Inout_opt_ LPTSTR lpCommandLine,
		_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
		_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
		_In_ BOOL bInheritHandles,
		_In_ DWORD dwCreationFlags,
		_In_opt_ LPVOID lpEnvironment,
		_In_opt_ LPCTSTR lpCurrentDirectory,
		_In_ LPSTARTUPINFO lpStartupInfo,
		_Out_ LPPROCESS_INFORMATION lpProcessInformation);

	[[nodiscard]] _Success_(return)
	BOOL StartAsUserT(DWORD dwSessionId,
		_In_opt_ LPCTSTR lpApplicationName,
		_Inout_opt_ LPTSTR lpCommandLine,
		_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
		_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
		_In_ BOOL bInheritHandles,
		_In_ DWORD dwCreationFlags,
		_In_opt_ LPVOID lpEnvironment,
		_In_opt_ LPCTSTR lpCurrentDirectory,
		_In_ LPSTARTUPINFO lpStartupInfo,
		_Out_ LPPROCESS_INFORMATION lpProcessInformation);

	// ReStart a process
	inline PROCESS_INFORMATION ReStart(const ProcessInfo& process_name) {
		kill(process_name);
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		return Start(process_name, si);
	};

	// Close <h>.hProcess and <h>.hThread
	BOOL CloseProcessHandle(PROCESS_INFORMATION h) {
		__try {
			bool a = false, b = false;
			if (h.hProcess) a = ::CloseHandle(h.hProcess);
			if (h.hThread) b = ::CloseHandle(h.hThread);
			return a && b;
		} __except (EXCEPTION_EXECUTE_HANDLER) { return FALSE; }
	}

	void CloseProcessHandles(std::vector<ProcessInfo>& processes) {
		for (auto& i : processes) ::CloseHandle(i);
	}

	// total process count
	size_t count();
	// process named <cmd> count
	size_t count(const TSTRING& paras);

	// Suspend a process
	DWORD suspend(HANDLE hProcess);
	// Resume a process that suspended
	DWORD  resume(HANDLE hProcess);

	/**
	@brief Close a process's window(s) 
	@param pi      Process Id
	@param wait    and wait for it to quit.
	@param waittime Max waiting time
	*/
	bool CloseAllWindows(ProcessInfo pi, bool wait = false, DWORD waittime = 2048);

	// See
	// http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20
	// Functions%2FNT%20Objects%2FProcess%2FNtSetInformationProcess.html
	NTSTATUS NtSetInformationProcess(
		HANDLE                    ProcessHandle,
		PROCESS_INFORMATION_CLASS ProcessInformationClass,
		PVOID                     ProcessInformation,
		ULONG                     ProcessInformationLength);
	// See
	// http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20
	// Functions%2FNT%20Objects%2FProcess%2FNtQueryInformationProcess.html
	__kernel_entry NTSTATUS NtQueryInformationProcess(
		HANDLE           ProcessHandle,
		PROCESSINFOCLASS ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength,
		PULONG           ReturnLength);

	// Let a process crash.
	// (using CreateRemoteThread)
	bool crash(HANDLE hProcess, bool wait = false);

	bool isEmpty() { return prs.size() == 0; };

protected:
	bool InitSuspend();
protected:
	SUSPENDPROCESS ZwSuspendProcess;
	RESUMEPROCESS  ZwResumeProcess;
	__NtSetInformationProcess _NtSetInformationProcess;
	__NtQueryInformationProcess _NtQueryInformationProcess;
private:
	__time64_t LastFlush;
	DWORD current_process_id;
	HMODULE ntdll;
	std::vector<Process_t::ProcessInfo> prs;
};
#ifndef _not_usingProcess_t_info
using ProcessInfo = Process_t::ProcessInfo;
#endif
extern Process_t Process;

inline void CloseHandles(std::vector<HANDLE>& handles) {
	for (auto i : handles) ::CloseHandle(i);
}

namespace std {
	using sstream    =  stringstream;
	using isstream   =  istringstream;
	using osstream   =  ostringstream;
	using wsstream   =  wstringstream;
	using wisstream  =  wistringstream;
	using wosstream  =  wostringstream;
};

template<typename err_type>
bool ExceptionPrintToStdErr(
	const err_type& err,
	const size_t LINE = 0,
	const char* FILE = "",
	bool(__stdcall* CallNextErrorHandle)(const err_type&) = nullptr) {
	try {
		const std::exception* exc;
		exc = dynamic_cast<const std::exception*>(&err);
		if (exc == nullptr || exc == NULL) return false;;
	}
	catch (const std::bad_cast&) {
		return false;
	}
	std::cerr << "[exception]" << typeid(err).name() << ":" << err.what() << 
		"\n\tAt " << LINE << ":" << FILE << " ,Error " << 
		typeid(err).name() << std::endl;
	if(!CallNextErrorHandle) return true;
	return CallNextErrorHandle(err);
};

// Show or remove a window ON TASKBAR (NOT remove the window)
bool WindowShowOnTaskBar(HWND hWnd, bool bShow = true);

// Copy text
errno_t CopyText(const std::wstring& text, UINT uFormat = CF_UNICODETEXT);
// Copy text
inline errno_t CopyText(const std::string& text, UINT uFormat = CF_UNICODETEXT) {
	return CopyText(s2ws(text), uFormat);
};
// Translate CopyText's error code to std::string
std::string CopyText(errno_t err);

// Make a version id
constexpr unsigned long long mkversion(long long _Main, long long _1, long long _2) {
	return _2 + (_1 << 8) + (_Main << 16);
}
// Make a version id (longer than mkversion)
constexpr unsigned long long mkversionl(long long _Main, long long _1, long long _2) {
	return _2 + (_1 << 16) + (_Main << 32);
}

inline constexpr bool operator ""_b(unsigned long long d) {
	return static_cast<bool>(d);
}

/****************************************************************************
	函数功能：获取标准UUID，含中划线
	Function function: obtain standard UUID, including middle dash
	示例|Example: 70fe761d-ce3c-465e-a260-bb0422bfa670
	@param 无|none

	@return UUID
*****************************************************************************/
std::string GenerateUUID();
std::wstring GenerateUUIDW();
/****************************************************************************
	函数功能：获取UUID，不含中划线
	Function function: obtain standard UUID without middle dash
	示例|Example: 70fe761dce3c465ea260bb0422bfa670    共计32位字符|32 characters in total
	@param 无

	@return UUID
*****************************************************************************/
std::string GenerateUUIDWithoutDelim();
std::wstring GenerateUUIDWithoutDelimW();

// Print some text to stdout.
// However, the output is character by character, 
// and the delay can be specified in the <timeout> parameter
void PrintTextEachCharW(std::wstring text, DWORD timeout = 0);
// Print some text to stdout.
// However, the output is character by character, 
// and the delay can be specified in the <timeout> parameter
void PrintTextEachCharA( std::string text, DWORD timeout = 0);
#ifdef UNICODE
// Print some text to stdout.
// However, the output is character by character, 
// and the delay can be specified in the <timeout> parameter
inline void PrintTextEachChar(std::wstring text, DWORD timeout = 0) {
	return PrintTextEachCharW(text, timeout);
#else
// Print some text to stdout.
// However, the output is character by character, 
// and the delay can be specified in the <timeout> parameter
inline void PrintTextEachChar( std::string text, DWORD timeout = 0) {
	return PrintTextEachCharA(text, timeout);
#endif
}

std::string GetCurrentUserSecurityId();

// Translate a error-code such as GetLastError() to std::wstring
std::wstring ErrorCodeToStringW(DWORD ErrorCode);
// Translate a error-code such as GetLastError() to std::string
std::string  ErrorCodeToStringA(DWORD ErrorCode);
inline std::wstring LastErrorStrW() {
	auto error = GetLastError();
	auto result = ErrorCodeToStringW(error);
	SetLastError(error);
	return result;
}
inline std::string  LastErrorStrA() {
	auto error = GetLastError();
	auto result = ErrorCodeToStringA(error);
	SetLastError(error);
	return result;
}
#ifdef UNICODE
// Translate a error-code such as GetLastError() to std::wstring
inline std::wstring ErrorCodeToString(DWORD ErrorCode) {
	return ErrorCodeToStringW(ErrorCode);
}
inline std::wstring LastErrorStr() {
	return ErrorCodeToString(GetLastError());
}
#else
// Translate a error-code such as GetLastError() to std::string
inline std::string ErrorCodeToString(DWORD ErrorCode) {
	return ErrorCodeToStringA(ErrorCode);
}
inline std::string LastErrorStr() {
	return ErrorCodeToString(GetLastError());
}
#endif

// Call CreateFileW, but this function's arguments is simpler than CreateFileW
inline HANDLE CreateFileSimpleW(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwCreationDisposition,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL,
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
	HANDLE hTemplateFile = NULL
) {
	return CreateFileW(lpFileName,
		dwDesiredAccess, 
		dwShareMode, 
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
}

/**
Get a memory-block for buffer.
@param maxsize Max size for this buffer. If failed, it/=2, and try again until it==1
@return this memory block's start address. If failed, return nullptr,
*/
[[nodiscard("Ignore the return value will caugh Memory Leak")]] 
void* GetBufMemory(size_t& maxsize);

#define AutoZeroMemory(_v) memset(&(_v),0,sizeof(_v))

#define srand_with_time() srand((unsigned)time(0))

std::wstring GetProcessStdOutputW(std::wstring cmd_line, bool NoWindow = true);
inline std::string GetProcessStdOutputA(std::string cmd_line, bool NoWindow = true) {
	return ws2s(GetProcessStdOutputW(s2ws(cmd_line), NoWindow));
}

bool GetProcessStdOutputWithExitCodeW(std::wstring cmd_line, DWORD* dwExitCode,
	std::wstring& output, bool NoWindow = true,
	void(__stdcall*callback)(std::wstring&, PVOID) = nullptr,
	PVOID callbackParamter = nullptr
);


//Judge whether the content of this std::string is a number
inline bool is_number(std::string str) {
	for (auto i : str) {
		if (!isdigit(i)) return false;
	}
	return true;
}

// Get file size
ULONGLONG MyGetFileSizeW(std::wstring filename);
// Get file size
inline ULONGLONG MyGetFileSizeA(std::string filename) {
	return MyGetFileSizeW(s2ws(filename));
}

/**
IsFileOrDirectory (ANSI)
Check a path is file or directory
@param path The file or directory path.
@return -1 -> Directory    0  -> Not Exists    1  -> File
*/
signed char IsFileOrDirectory(std::string path);
/**
IsFileOrDirectory (Unicode)
检测某std::string是目录还是文件
@param path 文件(夹)路径
@return -1 -> 目录    0  -> No such file or directory    1  -> 文件
*/
signed char IsFileOrDirectory(std::wstring path);

bool MyStartAsServiceW(std::wstring svc_name, LPSERVICE_MAIN_FUNCTIONW svc_main);
bool MyStartAsServiceA( std::string svc_name, LPSERVICE_MAIN_FUNCTIONA svc_main);
#ifdef UNICODE
#define MyStartAsService MyStartAsServiceW
#else
#define MyStartAsService MyStartAsServiceA
#endif

#if 1
/**
MyWriteApplicationEvent (Unicode)
Write a message to system log.
@param type EVENTLOG_${YourEventType}_TYPE
@param hEventLog NULL: Use Application Eventlog
*/
bool MyWriteApplicationEvent(LPCWSTR string, DWORD id = 0, 
	WORD type = EVENTLOG_INFORMATION_TYPE,
	void* lpRawData = NULL, DWORD dwDataSize = 0, WORD wCategory = 1,
	PSID lpUserSid = NULL, HANDLE hEventLog = NULL);
/**
MyWriteApplicationEvent (ANSI)
Write a message to system log.
@param type EVENTLOG_${YourEventType}_TYPE
@param hEventLog NULL: Use Application Eventlog
*/
inline bool MyWriteApplicationEvent(LPCSTR string, DWORD id = 0, 
	WORD type = EVENTLOG_INFORMATION_TYPE,
	void* lpRawData = NULL, DWORD dwDataSize = 0, WORD wCategory = 1,
	PSID lpUserSid = NULL, HANDLE hEventLog = NULL) {
	return MyWriteApplicationEvent(s2wc(string), id, type, lpRawData, dwDataSize,
		wCategory, lpUserSid, hEventLog);
}
#endif

typedef struct {
	LONG style;
	RECT wrect;
} FullScreenWindowRestoreInfo, *PFullScreenWindowRestoreInfo;
/*Full Screen a window*/
void FullScreenWindow(HWND hWindow, PFullScreenWindowRestoreInfo winfo = nullptr);
/*UnFull Screen a window*/
void UnFullScreenWindow(HWND hWindow, PFullScreenWindowRestoreInfo winfo);

// Center a window in the screen
void CenterWindow(HWND hWnd, HWND parent = NULL);

// Get current user name (ANSI)
std::string cppGetUserNameA();
// Get current user name (UNICODE)
std::wstring cppGetUserNameW();

/*
@brief Check IsCurrentUserInBuiltinGroup
@param SidType <winnt.h>
*/
BOOL IsCurrentUserInBuiltinGroup(WELL_KNOWN_SID_TYPE SidType);
/*
@brief Check IsCurrentUserInBuiltinGroup
@param SidType <winnt.h>
*/
BOOL IsUserInBuiltinGroup(HANDLE hProcess, WELL_KNOWN_SID_TYPE SidType);

// Get Process Token By Process ID
HANDLE GetProcessTokenById(DWORD pid);
// Get Process Token By Process Name
size_t GetProcessTokenByName(std::vector<HANDLE>& buffer, LPCTSTR name);
// Get Process Token By Process Name
inline std::vector<HANDLE> GetProcessTokenByName(LPCTSTR name) {
	std::vector<HANDLE> h; GetProcessTokenByName(h, name); return h;
}

/*
@brief Get Module Base Address by Process ID
@param pid Process ID
@param module_name The name of the module to find.\
                   If it is _T(""), it will find the base address of <pid>.
@return The base address of specified module. If failed, it will return nullptr.
*/
PVOID GetModuleBaseAddrByPID(DWORD pid, TSTRING module_name = _T(""));

namespace mariusbancila {
/* https://mariusbancila.ro/blog/2008/10/20/writing-utf-8-files-in-c/ */
std::string to_utf8(const wchar_t* buffer, DWORD len);
inline std::string to_utf8(const std::wstring& str) {
	return to_utf8(str.c_str(), DWORD(str.size()));
}
}

namespace s7 {
decltype(RegisterClassW(0)) MyRegisterClassW(PCWSTR className, WNDPROC WndProc,
	WNDCLASSEXW content = WNDCLASSEXW{ 0 });
decltype(RegisterClassA(0))  MyRegisterClassA(PCSTR  className, WNDPROC WndProc,
	WNDCLASSEXA content = WNDCLASSEXA{ 0 });
template<typename type> using ptr = type*;
BOOL AdjustPrivilege(ULONG Privilege, BOOLEAN Enable,
	BOOLEAN CurrentThread, PBOOLEAN Enabled = NULL);
//BOOL AdjustPrivilege(LPCTSTR Privilege, BOOLEAN Enable,
//	BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef LONG NTSTATUS;
NTSTATUS CallNtUnmapViewOfSection(HANDLE ProcessHandle, PVOID BaseAddress);
}

inline bool IsPointInRect(POINT pt, RECT rc) {
	return (pt.x > rc.left && pt.x < rc.right) &&
		(pt.y > rc.top && pt.y < rc.bottom);
}

template<typename StdStringType>
StdStringType GenerateRandomString(size_t length, StdStringType source) {
	StdStringType buffer{};
	size_t nSourceLength = source.length();
	for (size_t i = 0; i < length; ++i) {
		buffer += source[rand() % nSourceLength];
	}
	return buffer;
}

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
	_Out_ LPPROCESS_INFORMATION lpProcessInformation);

namespace StackOverflow {
	// https://stackoverflow.com/questions/154163/
	signed char swallow_redpill();
}

namespace s7 {
	NTSTATUS __stdcall CallNtTerminateProcess(
		HANDLE ProcessHandle, NTSTATUS ExitStatus);
}

/**
@brief Delete tree
@param szPath Path to delete
@return -1 => Error ; 0 => OK ; 1 => Something's wrong
*/
signed char __stdcall FileDeleteTreeW(std::wstring szPath, bool __internal_is_first_call__ = true);

/**
@return -1 => Error ; 0 => OK ; 1 => Something's wrong
*/
signed char __stdcall CopyFileTreeW(std::wstring src, std::wstring dest, bool bFailIfExists);


bool DoesUserUsesChinese();


inline bool CloseHandleIfOk(HANDLE hObj) {
	if (hObj) return CloseHandle(hObj); return false;
}



std::string ConvertUTF16ToUTF8(PCWSTR utf16Str);
inline std::string ConvertUTF16ToUTF8(const std::wstring& utf16Str) {
	return ConvertUTF16ToUTF8(utf16Str.c_str());
}
inline bool ConvertUTF16ToUTF8(
	const std::wstring& utf16Str, std::string& receiver
) {
	return (receiver = ConvertUTF16ToUTF8(utf16Str.c_str())).length();
}

std::wstring ConvertUTF8ToUTF16(PCSTR utf8Str);
inline std::wstring ConvertUTF8ToUTF16(const std::string& utf8Str) {
	return ConvertUTF8ToUTF16(utf8Str.c_str());
}
inline bool ConvertUTF8ToUTF16(
	const std::string& utf8Str, std::wstring& receiver
) {
	return (receiver = ConvertUTF8ToUTF16(utf8Str.c_str())).length();
}



// 查询注册表值并返回结果字符串  
bool MyQueryRegistryValue(HKEY hKey, const std::wstring& subKeyPath,
	const std::wstring& dataName, std::wstring& value);

// 设置注册表值  
bool MySetRegistryValue(HKEY hKey, const std::wstring& subKeyPath,
	const std::wstring& dataName, const std::wstring& value);



inline size_t FindSpecifiedDataInMemory(
	const char* buffer, size_t bufferSize, char specifiedData
) {
	for (size_t i = 0; i < bufferSize; ++i) {
		if (buffer[i] == specifiedData) {
			return i;
		}
	}
	return size_t(-1);
}





#if 1
bool GetProcessStdOutputWithExitCodeEnhanced(std::wstring cmd_line, DWORD* dwExitCode,
	std::wstring& output, bool NoWindow = true,
	void(__stdcall* callback)(std::wstring&, PVOID) = nullptr,
	PVOID callbackParamter = nullptr
);
#endif


namespace pipeutil {
	/**
	@brief 创建并读取命名管道
	@param result A pointer to the result string. Do not \
	release the memory until hEvent is set.
	@param hEvent In fact it is a thread handle, please use WaitForSignleObject \
	to wait.
	@param callback Optional; when the pipe received data the callback is called.
	@return Whether the operation is success or not. 不代表读取已完成。
	*/
	bool CreateAndReadPipeAsync(
		std::wstring pipeName, std::string* result, HANDLE& hEvent,
		void(__stdcall* callback)(std::string&, PVOID) = nullptr,
		PVOID callbackParamter = nullptr
	);
}


LONG MyVerifyBinarySignature(LPCWSTR pwszSourceFile);




#endif // defined(_TOOL_SHC0743_MAIN)
