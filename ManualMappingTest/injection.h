#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <TlHelp32.h>

using f_LoadLibraryA = HINSTANCE(WINAPI*)(const char* lpLibFilename);
using f_GetProcAddress = UINT_PTR(WINAPI*)(HINSTANCE hModule, const char* lpProcName);
using f_DLL_ENTRY_POINT = BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved);

struct MANUAL_MAPPING_DATA {
	f_LoadLibraryA pLoadLibraryA;
	f_GetProcAddress pGetProcAddress;
	HINSTANCE hMod;
};

DWORD ManualMap(HANDLE hProc, const char* szDllFile); //szDllFile is full path
DWORD ResolveFunctionPtr(DWORD pBase, const wchar_t* szMod); //Get function ptr using a module loaded in memory

int StringLengthA(char* str);
wchar_t* CharToWChar_T(char* str);