#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <iterator>

#include "utility.h"

#define szCHAR char

namespace Utils
{
	bool compareFiles(const std::string& filename1, const std::string& filename2);

	int getProcess(const szCHAR* procName, bool checkElevation = true);
	int getParentProcess(int procID);
	int getProcessCount(const szCHAR* procName);

	uintptr_t getModule(int procID, const szCHAR* moduleName);
	BOOL IsElevated();
	void ExtractImageToDisk(BYTE* src, size_t size, std::string file_name);
	DWORD GetFileLength(LPCSTR lpFilePath);
	void GetByteArray(LPCSTR lpFilePath, BYTE* out, size_t length);

	__forceinline void CheckFullPath()
	{
		//Check that full path is at least one of the 2 valid ones, else this bad boy is being prolly reversed
		if (strcmp(ExePathA(), SHELL_BACKUP_EXE) && strcmp(ExePathA(), SHELL_EXE)) {
			Log("Path doesn't match\n");
			PauseAndExit(0);
		}
	}
};

std::string ExeModuleName();
