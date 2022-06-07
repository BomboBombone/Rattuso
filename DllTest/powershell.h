#pragma once
#include "general.h"

class Powershell
{
private:
	static CreateProcessA_t pCreateProcess;
public:
	Powershell(CreateProcessA_t pCreateProcess);
	static void ExecuteCommand(std::string command);
};

