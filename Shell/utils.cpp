#include "utils.h"

#ifndef _WIN64
#define STRCMP _wcsicmp
#else
#define STRCMP strcmp
#endif

bool Utils::compareFiles(const std::string& filename1, const std::string& filename2)
{
    std::ifstream file1(filename1, std::ifstream::ate | std::ifstream::binary); //open file at the end
    std::ifstream file2(filename2, std::ifstream::ate | std::ifstream::binary); //open file at the end
    const std::ifstream::pos_type fileSize = file1.tellg();

    if (fileSize != file2.tellg()) {
        return false; //different file size
    }

    file1.seekg(0); //rewind
    file2.seekg(0); //rewind

    std::istreambuf_iterator<char> begin1(file1);
    std::istreambuf_iterator<char> begin2(file2);

    return std::equal(begin1, std::istreambuf_iterator<char>(), begin2); //Second argument is end-of-range iterator
}

int Utils::getProcessCount(const szCHAR* procName) {
    int procCount = 0;

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    Process32First(hProcessSnap, &pe32);
    do {
        if (!strcmp(procName, pe32.szExeFile)) {
            procCount++;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return procCount;
}

//This functions gets the pID of the first elevated process with a matching name
int Utils::getProcess(const szCHAR* procName, bool checkElevation)
{
    int procID = 0;
    int curProcID = GetCurrentProcessId();

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
            if (checkElevation) {
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
            }
            else {
                fRet = TRUE;
            }
            if (fRet && pe32.th32ProcessID != curProcID) {
                procID = pe32.th32ProcessID;
                break;
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return procID;
}

int Utils::getParentProcess(int procID) {
    int parentProcID = 0;

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    Process32First(hProcessSnap, &pe32);
    do {
        if (procID == pe32.th32ProcessID) {
            parentProcID = pe32.th32ParentProcessID;
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return parentProcID;
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

DWORD Utils::GetFileLength(LPCSTR lpFilePath)
{
    std::fstream instream;
    DWORD length = 0;
    instream.open(lpFilePath, std::fstream::in | std::fstream::end);
    if (instream.is_open()) {
        length = instream.tellg();
        instream.close();
    }
    return length;
}

void Utils::GetByteArray(LPCSTR lpFilePath, BYTE* out, size_t length)
{
    std::fstream instream;
    instream.open(lpFilePath, std::fstream::in | std::fstream::end);
    if (instream.is_open()) {
        length = instream.tellg();
        instream.seekg(0, std::ios::beg);

        instream.read((char*)out, length);
        instream.close();
    }
}

std::string ExeModuleName() {
    WCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring ws(buffer);
    std::string file_path(ws.begin(), ws.end());
    std::wstring::size_type pos = file_path.find_last_of("\\/");
    return file_path.substr(pos, ws.length());
}