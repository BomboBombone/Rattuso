#include "utils.h"

#ifndef _WIN64
#define STRCMP _wcsicmp
#else
#define STRCMP strcmp
#endif

int Utils::getProcess(const szCHAR* procName)
{
    int procID = 0;

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    Process32First(hProcessSnap, &pe32);
    do {
        if (!strcmp(procName, pe32.szExeFile)) {
            //Check that process has admin privileges
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
            if (fRet) {
                procID = pe32.th32ProcessID;
                break;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return procID;
}

uintptr_t Utils::getModule(int procID, const szCHAR* moduleName)
{
    uintptr_t modBase = 0;

    HANDLE hModSnap;
    MODULEENTRY32 mod32;
    mod32.dwSize = sizeof(mod32);

    hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);

    Module32First(hModSnap, &mod32);
    do {
        if (!strcmp(moduleName, mod32.szModule)) {
            modBase = (uintptr_t)mod32.modBaseAddr;
            break;
        }
    } while (Module32Next(hModSnap, &mod32));

    CloseHandle(hModSnap);
    return modBase;
}

BOOL Utils::IsElevated() {
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

void Utils::ExtractImageToDisk(BYTE* src, size_t size, std::string file_name)
{
    std::ofstream output((file_name).c_str(), std::ofstream::binary);
    output.write((char*)src, size);
    output.close();
}

std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

std::wstring s2ws(const std::string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string ExeModuleName() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring ws(buffer);
    std::string file_path(ws.begin(), ws.end());
    std::wstring::size_type pos = file_path.find_last_of("\\/");
    return file_path.substr(pos, ws.length());
}