#include <string>
#include <Windows.h>
#include <iostream>

#include "inline.h"

int main()
{
	std::string dir = "C:\\Windows";
	std::cout << dir.find_last_of('/');
	Exit();

}