#include "injection.h"
#include "utils.h"
#include <windows.h>
#include <string>
#include <iostream>

std::string ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos + 1);
}

#define JOIN(x, y) x ## y
#ifdef _WIN64
#define FULL_PATH(x) JOIN("C:\\Windows\\System32\\", x)
#else
#define FULL_PATH(x) JOIN("C:\\Windows\\SysWOW64\\", x)
#endif

const char* szProc = "Discord.exe";

int main() {
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Utils::getProcess(szProc));

	if (!hProc) {
		DWORD Err = GetLastError();
		printf("OpenProcess failed: 0x%X\n", GetLastError());
		system("PAUSE");
		return 0;
	}

	std::string DllName = "DllTest.dll";
	auto pKernel = ManualMap(hProc, (ExePath() + DllName).c_str());
	printf("kernel32.dll mapped at: %X\n", pKernel);

	if (!pKernel) {
		CloseHandle(hProc);
		printf("Something went wrong...");
		system("PAUSE");
		return 0;
	}
	CloseHandle(hProc);
	return 0;
}