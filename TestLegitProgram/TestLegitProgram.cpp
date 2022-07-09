#include <string>
#include <Windows.h>
#include <iostream>
#include <filesystem>

int main()
{
	std::string dir = "C:\\Windows";
	std::cout << dir.find_last_of('/');
}