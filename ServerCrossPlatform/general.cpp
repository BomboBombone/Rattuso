#include "general.h"

bool General::cmdMode = false;

void General::outputMsg(std::string message, int msgType)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	
	switch (msgType)
	{
	case 1:
#ifdef _WIN32
		SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
#else
		printf(GREEN);
#endif
		std::cout << message << std::endl;
		break;
	case 2:
#ifdef _WIN32
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
#else
		printf(RED);
#endif
		std::cout << "[WARNING] " << message << std::endl;
		break;
	case 3:
		std::cout << message;
		break;
	}
#ifdef _WIN32
	SetConsoleTextAttribute(hConsole, 0x09);
#else
	printf(NC);
#endif
	if(!General::cmdMode)
		std::cout << "RATtuso console => ";
}

int General::createThread(void* (*EntryPoint)(void* args), void* args)
{
#ifdef _WIN32
	return _beginthreadex(NULL, NULL, (_beginthreadex_proc_type)EntryPoint, args, NULL, NULL);
#else 
	pthread_t thread;
	return pthread_create(&thread, NULL, EntryPoint, args);
#endif
}


bool General::processParameter(std::string &command, std::string compCommand)
{
	std::string::size_type i = command.find(compCommand);
	if (i != std::string::npos)
	{
		command.erase(i, compCommand.length() + 1);
		return true;
	}
	else
		return false;
}
