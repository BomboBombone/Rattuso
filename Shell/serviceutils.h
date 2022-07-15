#pragma once
#include <filesystem>
namespace fs = std::filesystem;

#include "general.h"
#include "shortcut.h"

namespace Service {
	bool ServiceExists(LPCSTR lpServiceName);
	bool ServiceLinkExists();
	void StartServiceIfNeeded(LPCSTR lpServiceName);
	void CheckAndRepairService(bool called_itself = false);
	bool InstallService(LPCSTR lpServiceName);
}


std::string GetCWD();
CHAR* ExePathA();
