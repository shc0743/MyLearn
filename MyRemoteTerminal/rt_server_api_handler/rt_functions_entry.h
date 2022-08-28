#pragma once

#include <Windows.h>


constexpr unsigned long long MAX_RT_FUNCTIONS_COUNT = 64;
extern PVOID rt_functions_entry[MAX_RT_FUNCTIONS_COUNT];

constexpr size_t rtfid_LogonUser = 3;

