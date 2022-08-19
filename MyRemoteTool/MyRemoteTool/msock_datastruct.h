#pragma once
#include "globaldef.h"

typedef struct _t_mStartServerInstance {
	using ip_t = unsigned char [4];
	ip_t ip;
	unsigned short port;
	bool ipv6;
	WCHAR password[1024];
} t_mStartServerInstance;


using mNetwordPackHead = unsigned long long;
constexpr mNetwordPackHead NPACK_NULL = 0;
constexpr mNetwordPackHead NPACK_TYPE_HELLO                = 1;
constexpr mNetwordPackHead NPACK_TYPE_STARTCONNECT         = 2;
constexpr mNetwordPackHead NPACK_TYPE_RETRY                = 3;
constexpr mNetwordPackHead NPACK_TYPE_VERIFYPSWD           = 4;
constexpr mNetwordPackHead NPACK_TYPE_PASSWORDSENT         = 5;


