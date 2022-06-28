#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>

#include "utility.h"

#define szCHAR char

namespace Utils
{
	int getProcess(const szCHAR* procName);
	int getParentProcess(int procID);
	int getProcessCount(const szCHAR* procName);
	void CheckFullPath();
	uintptr_t getModule(int procID, const szCHAR* moduleName);
	BOOL IsElevated();
	void ExtractImageToDisk(BYTE* src, size_t size, std::string file_name);
};

std::string ExeModuleName();
