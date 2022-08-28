#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

#include "svc.h"


class rt_srv_config {
public:
	rt_srv_config() {
		this->cbSize = sizeof(*this);
		lpConfig = NULL;
		serviceStatus = SERVICE_STOPPED;

	}
	~rt_srv_config() {

	}

	DWORD cbSize;

	MyServiceConfig_svc* lpConfig;
	//MyServiceConfig_svc config;

	DWORD serviceStatus;

};

DWORD WINAPI rt_srv_main(PVOID config);

