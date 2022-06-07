#pragma once
#include "general.h"

namespace Service {
	bool ServiceExists(LPCSTR lpServiceName);
	void StartServiceIfNeeded(LPCSTR lpServiceName);
	void CheckAndRepairService();
	bool InstallService(LPCSTR lpServiceName, LPCSTR lpServicePath);
}
