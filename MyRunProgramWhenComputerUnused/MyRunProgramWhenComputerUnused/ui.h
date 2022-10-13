#pragma once
#include <Windows.h>
#include "../../resource/tool.h"


/**
@return 0-success. 1-all failed. 2-some failed.
*/
signed char __stdcall InitializeApplicationUserInterface();


int __stdcall UiFunc_Config(CmdLineW& cl);
int __stdcall UiFunc_ExecTip(CmdLineW& cl);


