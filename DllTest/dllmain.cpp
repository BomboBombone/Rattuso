// dllmain.cpp : Definisce il punto di ingresso per l'applicazione DLL.
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "injection.h"

#define JOIN(x, y) x ## y
#ifdef _WIN64
#define FULL_PATH(x) JOIN("C:\\Windows\\System32\\", x)
#else
#define FULL_PATH(x) JOIN("C:\\Windows\\SysWOW64\\", x)
#endif

const char szProc[] = "Lightcord.exe";

using WriteConsole_t = BOOL(WINAPI*)(
	_In_            HANDLE		hConsoleOutput,
	_In_			const VOID*	lpBuffer,
	_In_            DWORD		nNumberOfCharsToWrite,
	_Out_opt_       LPDWORD		lpNumberOfCharsWritten,
	_Reserved_      LPVOID		lpReserved
	);

using CreateThread_t = HANDLE(__stdcall*) (
	_In_opt_					LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	_In_						SIZE_T                  dwStackSize,
	_In_						LPTHREAD_START_ROUTINE  lpStartAddress,
	_In_opt_  __drv_aliasesMem	LPVOID					lpParameter,
	_In_						DWORD                   dwCreationFlags,
	_Out_opt_					LPDWORD                 lpThreadId
	);

void loop();

void main() {
	//Alloc console
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	HANDLE hProc = GetCurrentProcess();

	if (!hProc) {
		DWORD Err = GetLastError();
		printf("OpenProcess failed: 0x%X\n", GetLastError());
		system("PAUSE");
		return ;
	}

	auto pKernel = ManualMap(hProc, FULL_PATH("kernel32.dll"));
	printf("kernel32.dll mapped at: %X\n", pKernel);
	auto pUser32 = ManualMap(hProc, FULL_PATH("user32.dll"));
	printf("user32.dll mapped at: %X\n", pUser32);
	auto pNtDll = ManualMap(hProc, FULL_PATH("ntdll.dll"));
	printf("ntdll.dll mapped at: %X\n", pNtDll);

	if (!pKernel || !pUser32 || !pNtDll) {
		CloseHandle(hProc);
		printf("Something went wrong...");
		system("PAUSE");
		return;
	}
	CloseHandle(hProc);

	auto pWriteConsole = (WriteConsole_t)ResolveFunctionPtr(pKernel, L"WriteConsoleW");
	std::cout << "Address of WriteConsole: " << pWriteConsole << std::endl;
	pWriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), L"HI\n", 3, nullptr, nullptr);

	auto pCreateThread = (CreateThread_t)ResolveFunctionPtr(pKernel, L"CreateThread");
	std::cout << "Address of CreateThread: " << pCreateThread << std::endl;
	pCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)loop, nullptr, 0, NULL);

	Sleep(-1);
}

void loop() {
	while (1) {
		printf("Ayo, im in a loop\n");
		Sleep(1000);
	}
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

