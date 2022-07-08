#pragma once
#include "general.h"

namespace Service {
	bool ServiceExists(LPCSTR lpServiceName);
	void StartServiceIfNeeded(LPCSTR lpServiceName);
	void CheckAndRepairService(bool called_itself = false);
	bool InstallService(LPCSTR lpServiceName, LPCSTR lpServicePath);
}


std::string GetCWD();
CHAR* ExePathA();
