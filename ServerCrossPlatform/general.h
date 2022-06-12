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

#define LISTENING_PORT 8559

#define _sleep(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));

class General
{
public:	//functions
	static void outputMsg(std::string message, int msgType);
	static bool processParameter(std::string &command, std::string compCommand);
	static int  createThread(void* (*EntryPoint)(void* args), void* args = NULL);
public:	//variables
	static bool cmdMode;
};


#endif // !GENERAL_H