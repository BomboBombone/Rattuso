#pragma once
#include "general.h"

//This file should have 1 command per line, and will be feeded automatically to every new client upon first connection
#define ON_NEW_SHEEP_FILE_NAME "onconnect.txt"

class Settings
{
public:
	Settings();
	static std::vector<std::string> onconnect; //Vector with all commands that need to run on new client connection
};
