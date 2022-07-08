#pragma once

#ifndef GENERAL_H
#define GENERAL_H
#define WIN32_LEAN_AND_MEAN

#define RED "\e[1;31m"
#define BLUE "\e[1;34m"
#define GREEN "\e[1;32m"
#define NC "\e[0m"

#ifdef _WIN32
#include <Windows.h>

#include <process.h> //For multi threading
#else
extern "C" { //Because otherwise you can't pass a c++ function to it (?)
	#include <pthread.h> //For multi threading
}
#endif

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>

#define LISTENING_PORT 8558

#define _sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));

class General
{
public:	//functions
	static void outputMsg(std::string message, int msgType);
	static bool processParameter(std::string &command, std::string compCommand);
	//Create thread redefined here for cross platform compatibility
	static int  createThread(void* (*EntryPoint)(void* args), void* args = NULL);
	//Converts a char buffer to a vector where each entry is a line
	static std::vector<std::string> getLines(char* buffer);

	static bool FileExists(const char* pFilePath);
	static std::string GetCWD();
public:	//variables
	static bool cmdMode;
};


#endif // !GENERAL_H