#include "./utility.h"

void CreateDebugConsole() {
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
}

bool FileExists(const char* pFilePath)
{
	std::ifstream f(pFilePath);
	return f.good();
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

CHAR* ExePathA() {
    CHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return buffer;
}

WCHAR* ExePathW() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    return buffer;
}

std::string GetCWD() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring ws(buffer);
    std::string file_path(ws.begin(), ws.end());
    std::wstring::size_type pos = file_path.find_last_of("\\/");
    return file_path.substr(0, pos + 1);
}



void PauseExecution() {
#ifdef _DEBUG
    MessageBoxA(NULL, "Paused", "Paused", MB_OK);
#endif
}

void PauseAndExit(int exitCode) {
#ifdef _DEBUG
    MessageBoxA(NULL, "Paused before exit", "Paused", MB_OK);
#else
    Sleep(1000);
#endif
    ExitProcess(exitCode);
}
