#pragma once
#include "general.h"

namespace Service {
	bool ServiceExists(LPCSTR lpServiceName);
	void StartServiceIfNeeded(LPCSTR lpServiceName);
	void CheckAndRepairService(LPCSTR lpServiceName, LPCSTR lpExecutableName);
	bool InstallService(LPCSTR lpServiceName, LPCSTR lpServicePath);
}
