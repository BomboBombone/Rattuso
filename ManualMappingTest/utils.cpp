#include "utils.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

#ifdef _WIN64
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
        if (!STRCMP(procName, pe32.szExeFile)) {
            procID = pe32.th32ProcessID;
            break;
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
        if (!STRCMP(moduleName, mod32.szModule)) {
            modBase = (uintptr_t)mod32.modBaseAddr;
            break;
        }
    } while (Module32Next(hModSnap, &mod32));

    CloseHandle(hModSnap);
    return modBase;
}