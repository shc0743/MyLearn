#pragma once

#include <Windows.h>
#include <iostream>


struct rt_user_recovery {
	bool recoveryEnabled;

	constexpr static size_t nRecoveryInfoMax = 5;

	size_t recoveryInfoLength;

	struct recovery_info {
		enum recovery_method_t {
			RECOVERY_ERROR = 0,
			RECOVERY_EMAIL = 1,
			RECOVERY_SAFETY_QUESTION = 2,
			RECOVERY_PHONE_NUMBER = 3,
		} recovery_method;

		union recovery_data_t {
			unsigned long phone;
			char email[256];
			struct {
				wchar_t q[128], a[128];
			} question;
		} recovery_data;
	};

	recovery_info recoveryInfo[nRecoveryInfoMax];

};

class rt_userinfo {
public:
	rt_userinfo() {
		memset(this, 0, sizeof(*this));
		this->cbSize = sizeof(*this);
	}

	DWORD cbSize;

public:
	bool valid;
	bool enabled;

	using istr/*infostr*/ = WCHAR[256];

	istr username;
	istr password; // [Deprecated]: Please use password_encrypted.
	istr password_encrypted;

	bool two_step_logon;

	enum priv_level_t {
		PRIVILEGE_LEVEL_ERROR = 0,
		PRIVILEGE_LEVEL_GUEST = 1,
		PRIVILEGE_LEVEL_LIMITED = 2,
		PRIVILEGE_LEVEL_USER = 3,
		PRIVILEGE_LEVEL_ADMIN = 4,
		PRIVILEGE_LEVEL_SUPER = 5,
		PRIVILEGE_LEVEL_ROOT = 6,
	};
	priv_level_t priv_level;

	rt_user_recovery recovery;

};

extern rt_userinfo* gSrvUsers;


size_t rt_GetUserCount();
bool rt_srv_load_users(HANDLE*, HANDLE*);
rt_userinfo rt_LogonUser(std::wstring username, std::wstring password_enc);



