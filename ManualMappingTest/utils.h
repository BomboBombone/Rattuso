#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>

#define szCHAR char

namespace Utils
{
	int getProcess(const szCHAR* procName);
	uintptr_t getModule(int procID, const szCHAR* moduleName);
	BOOL IsElevated();
	void ExtractImageToDisk(BYTE* src, size_t size, std::string file_name);
};

std::string ExeModuleName();
std::string ws2s(const std::wstring& wstr);
std::wstring s2ws(const std::string& str);