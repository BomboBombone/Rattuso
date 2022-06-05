#include "conversion.h"

#include <Windows.h>
#include <atlbase.h>

LPTSTR Conversion::convStringToLPTSTR(std::string s)
{
	size_t origsize = s.size();
	const size_t newsize = MAX_PATH;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	mbstowcs_s(&convertedChars, wcstring, origsize, s.c_str(), _TRUNCATE);
	return wcstring;
}

std::wstring Conversion::convStringToWidestring(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}