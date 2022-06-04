#pragma once
#include <Windows.h>

#ifdef _WIN64
#define szCHAR wchar_t
#else
#define szCHAR char
#endif

namespace Utils
{
	int getProcess(const szCHAR* procName);
	uintptr_t getModule(int procID, const szCHAR* moduleName);
};