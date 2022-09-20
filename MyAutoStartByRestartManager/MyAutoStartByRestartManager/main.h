#pragma once
#ifndef __cplusplus
#error C++ is required
#endif
#include <Windows.h>
#include <string>
#include "../../resource/tool.h"


constexpr auto nMyVersion = mkversion(1, 0, 1);

template<typename str>
class _myItem {
public:
	_myItem() {

	}

	bool operator==(const _myItem& b) const {
		const _myItem& a = *this;
		return
			a.enabled == b.enabled &&
			scmp(a.name, b.name) &&
			scmp(a.program, b.program) &&
			a.progShowType == b.progShowType &&
			a.progStartType == b.progStartType;
	};

	bool enabled = false;
	bool is_locked = false;
	str name[256]{};

	str program[1024]{};
	enum class StartType {
		undefined = 0,
		Create_Process = 1,
		Shell_Execute = 2,
	} progStartType = StartType::undefined;
	int progShowType = SW_HIDE;

protected:

	bool scmp(const str* s1, const str* s2) const {
		size_t i = 0;
		do {
			if (s1[i] != s2[i]) return false;
		} while (s1[i] != 0 && s2[i] != 0);
		return true;
	}

};

using t_ItemCount = unsigned long long;
using t_FileVersion = decltype(nMyVersion);
using t_Item = _myItem<WCHAR>;

/**
@brief Load config from file
@param szFile File path
@param buffer Item buffer. If (this is empty and (*countAndMaxBufferSize) is NOT 0) or countAndMaxBufferSize is NULL, the action is undefined.
@param countAndMaxBufferSize buffer's size. If (*countAndMaxBufferSize) is 0, the function alloc a memory (big enough). You should call mFreeConfig.
@return If (*countAndMaxBufferSize) is 0, returns alloced memory. Else, return buffer (success) or NULL (failed). Call GetLastError() to get error code.
*/
t_Item* __stdcall mLoadConfig(std::wstring szFile, t_Item* buffer, t_ItemCount* countAndMaxBufferSize);
bool __stdcall mSaveConfig(std::wstring szFile, t_Item* buffer, t_ItemCount count);
bool __stdcall mFreeConfig(t_Item*);
inline bool __stdcall mLockItem(t_Item* it) { 
	if(it->is_locked)return(false);else{return(it->is_locked = true);}; 
};
inline bool __stdcall mUnlockItem(t_Item* it) { 
	if(!it->is_locked)return(false);else{return!(it->is_locked = false);}; 
};
bool __stdcall mRegisterRestartManager(std::wstring szConfigFile);
int __stdcall mRunAutostart(t_Item* item, t_ItemCount count, t_FileVersion ver);



