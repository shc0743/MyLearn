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

};

void MyServiceSetName_svc(std::wstring name);
void MyServiceSetConfig_svc(std::wstring cfg);
void WINAPI MyServiceMain_svc(
	DWORD   dwNumServicesArgs,
	LPWSTR* lpServiceArgVectors
); 


int __stdcall MyServiceChildMain_svc(CmdLineW& cl);



