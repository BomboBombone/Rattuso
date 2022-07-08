#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

std::string ExeModuleName() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring ws(buffer);
    std::string file_path(ws.begin(), ws.end());
    std::wstring::size_type pos = file_path.find_last_of("\\/");
    return file_path.substr(pos + 1, ws.length());
}

int main(int argc, char* argv[])
{
    char command[256] = "start /min cmd /c del ";
    strcat_s(command, ExeModuleName().c_str());

    printf("Attempting to delete self...\n");

    system(command);
    return 0;
}