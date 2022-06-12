#include "general.h"

bool General::cmdMode = false;

void General::outputMsg(std::string message, int msgType)
{
	switch (msgType)
	{
	case 1:
		printf(GREEN);
		std::cout << message << std::endl;
		break;
	case 2:
		printf(RED);
		std::cout << "[ERROR] " << message << std::endl;
		break;
	case 3:
		std::cout << message;
		break;
	}
	printf(NC);
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
