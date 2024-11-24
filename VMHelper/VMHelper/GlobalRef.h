#pragma once

#include <Windows.h>
#include <string>
#include "../../resource/tool.h"



#pragma region Global Refers


int AppHelp();

#pragma region Virtual Machine

int __stdcall vmexec(CmdLineW& cl);

#pragma endregion
#pragma region Host Machine
int __stdcall vm_startvmware(std::wstring auth_token, std::wstring app_path);
int __stdcall vm_runapp(std::wstring auth_token, std::wstring app_path);
#pragma endregion



#pragma endregion



