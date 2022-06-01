#pragma once
#include <Windows.h>

namespace Utils
{
	int getProcess(const char* procName);
	DWORD getModule(int procID, const char* moduleName);
};