#pragma once
#include <Windows.h>

#define szCHAR char

namespace Utils
{
	int getProcess(const szCHAR* procName);
	uintptr_t getModule(int procID, const szCHAR* moduleName);
	BOOL IsElevated();
};