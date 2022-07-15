#include "./utility.h"

void CreateDebugConsole() {
#ifdef DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
}

//Checks current executable privileges, useful to test any new modules that need privileges to run
BOOL IsElevated() {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

void PauseExecution() {
#ifdef DEBUG
    MessageBoxA(NULL, "Paused", "Paused", MB_OK);
#endif
}

void PauseAndExit(int exitCode, const char* mes) {
#ifndef DEBUG
    Sleep(2000);
    if (mes) {
        MessageBoxA(NULL, mes, "Paused", MB_OK);
    }
    else {
        MessageBoxA(NULL, "Paused before exit", "Paused", MB_OK);
    }
#else
    Sleep(2000);
#endif
    ExitProcess(exitCode);
}
