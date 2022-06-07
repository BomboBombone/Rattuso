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
	int getParentProcess(int procID);
	uintptr_t getModule(int procID, const szCHAR* moduleName);
	BOOL IsElevated();
	void ExtractImageToDisk(BYTE* src, size_t size, std::string file_name);
};

std::string ExeModuleName();
