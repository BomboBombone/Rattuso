//This file is used to declare some general usage functions that could be useful in multiple projects inside the solutions, included future projects

#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>

#include "folders.h"

void CreateDebugConsole();
bool FileExists(const char* pFilePath);
BOOL IsElevated();
CHAR* ExePathA();
WCHAR* ExePathW();
std::string GetCWD();

void PauseExecution();
void PauseAndExit(int exitCode, const char* mes = nullptr);

//Function definitions for manually mapped modules

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
