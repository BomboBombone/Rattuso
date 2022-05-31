#include "injection.h"

const char szDllFile[] = "C:\\Windows\\SysWOW64\\kernel32.dll";
const char szProc[] = "Discord.exe";

using WriteConsole_t = BOOL(WINAPI*)(
	_In_             HANDLE  hConsoleOutput,
	_In_       const VOID* lpBuffer,
	_In_             DWORD   nNumberOfCharsToWrite,
	_Out_opt_        LPDWORD lpNumberOfCharsWritten,
	_Reserved_       LPVOID  lpReserved
);

using CreateThread_t = HANDLE (__stdcall*) (
	_In_opt_					LPSECURITY_ATTRIBUTES   lpThreadAttributes,
	_In_						SIZE_T                  dwStackSize,
	_In_						LPTHREAD_START_ROUTINE  lpStartAddress,
	_In_opt_  __drv_aliasesMem	LPVOID					lpParameter,
	_In_						DWORD                   dwCreationFlags,
	_Out_opt_					LPDWORD                 lpThreadId
);

void loop();

int main() {
	HANDLE hProc = GetCurrentProcess();
	if (!hProc) {
		DWORD Err = GetLastError();
		printf("OpenProcess failed: 0x%X\n", GetLastError());
		system("PAUSE");
		return 0;
	}

	auto pKernel = ManualMap(hProc, szDllFile);
	printf("kernel32.dll mapped at: %X\n", pKernel);
	auto pUser32 = ManualMap(hProc, "C:\\Windows\\SysWOW64\\user32.dll");
	printf("user32.dll mapped at: %X\n", pUser32);
	auto pNtDll = ManualMap(hProc, "C:\\Windows\\SysWOW64\\ntdll.dll");
	printf("ntdll.dll mapped at: %X\n", pNtDll);

	if (!pKernel || !pUser32 || !pNtDll) {
		CloseHandle(hProc);
		printf("Something went wrong...");
		system("PAUSE");
		return 0;
	}
	CloseHandle(hProc);

	auto pWriteConsole = (WriteConsole_t)ResolveFunctionPtr(pKernel, L"WriteConsoleW");
	std::cout << "Address of WriteConsole: " << pWriteConsole << std::endl;
	pWriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), L"HI\n", 3, nullptr, nullptr);

	auto pCreateThread = (CreateThread_t)ResolveFunctionPtr(pKernel, L"CreateThread");
	std::cout << "Address of CreateThread: " << pCreateThread << std::endl;
	pCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)loop, nullptr, 0, NULL);
	
	Sleep(-1);
	return 0;
}

void loop() {
	while (1) {
		printf("Ayo, im in a loop\n");
		Sleep(1000);
	}
}