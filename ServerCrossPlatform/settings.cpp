#include "settings.h"

std::vector<std::string> Settings::onconnect;

Settings::Settings()
{
	std::fstream fs;
	fs.open(ON_NEW_SHEEP_FILE_NAME, std::fstream::in | std::fstream::ate);
	if (!fs.is_open()) { //There is no settings file
		General::outputMsg("Couldn't open onconnect.txt", 2);
		return;
	}

	int length = fs.tellg();
	fs.seekg(0, fs.beg);

	char* buffer = new char[length];

	fs.read(buffer, length);
	fs.close();

	Settings::onconnect = General::getLines(buffer);
}
