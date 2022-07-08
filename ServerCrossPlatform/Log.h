#pragma once
#include <filesystem>

#include "general.h"

namespace fs = std::filesystem;

enum class LogLevel {
	Info,
	Warning,
	Error
};

class Log
{
private:
	std::string logFolder;

public:
	Log();
	void Write(LogLevel& level);
};
