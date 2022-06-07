#include "pch.h"
#include "general.h"

bool FileExists(const char* pFilePath)
{
	std::ifstream f(pFilePath);
	return f.good();
}
