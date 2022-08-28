#pragma once
#include <Windows.h>
#include <string>
#include "../../resource/tool.h"


class MyServiceConfig_svc {
public:
	MyServiceConfig_svc() {
		memset(this, 0, sizeof(*this));
		this->cbSize = sizeof(*this);

	}

	DWORD cbSize;

	int server_port;

};

HANDLE MyCreateMemmap(DWORD size, DWORD hw = 0);

void MyServiceSetName_svc(std::wstring name);
void WINAPI MyServiceMain_svc(
	DWORD   dwNumServicesArgs,
	LPWSTR  *lpServiceArgVectors
);
int rt_svc_ServiceSubProcessHandler(CmdLineW& cl);

bool rt_srv_checkupd_resource();
bool rt_srv_update_resource();


