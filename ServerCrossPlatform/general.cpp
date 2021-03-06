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

std::vector<std::string> General::getLines(char* buffer)
{
	int cpos = 0;
	int sEnd = 0; //End of last found line
	char cchar = *buffer; //Current character
	char string_holder[MAX_PATH]{0};
	std::vector<std::string> output;

	//While current char != null terminator
	while (*(buffer + cpos)) {
		if (*(buffer + cpos) == '\n') { //Character is carriage ret
			memcpy(string_holder, buffer + sEnd, cpos - sEnd);
			output.push_back(string_holder);
			//cpos++; //Skip newline char
			sEnd = cpos + 1;
		}
		cpos++;
	}
	return output;
}

bool General::FileExists(const char* pFilePath)
{
	std::ifstream f(pFilePath);
	return f.good();
}

std::string General::GetCWD()
{
	WCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	std::wstring ws(buffer);
	std::string file_path(ws.begin(), ws.end());
	std::wstring::size_type pos = file_path.find_last_of("\\/");
	return file_path.substr(0, pos + 1);
}


bool General::processParameter(std::string &command, std::string compCommand)
{
	std::string::size_type i = command.rfind(compCommand);
	if (i == 0)
	{
		command.erase(i, compCommand.length() + 1);
		return true;
	}
	else
		return false;
}