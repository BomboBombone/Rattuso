//This file is used to declare some general usage functions that could be useful in multiple projects inside the solutions, included future projects

#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

#include "folders.h"

void CreateDebugConsole();
BOOL IsElevated();

__forceinline bool FolderExist(const std::string& s)
{
	struct stat buffer;
	return (stat(s.c_str(), &buffer) == 0);
}

__forceinline CHAR* ExePathA() {
    CHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return buffer;
}

__forceinline WCHAR* ExePathW() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return buffer;
}

__forceinline std::string GetCWD() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring ws(buffer);
    std::string file_path(ws.begin(), ws.end());
    std::wstring::size_type pos = file_path.find_last_of("\\/");
    return file_path.substr(0, pos + 1);
}
__forceinline bool FileExists(const char* pFilePath)
{
	std::ifstream f(pFilePath);
	return f.good();
}

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
