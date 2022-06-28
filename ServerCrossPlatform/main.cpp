#include "general.h"
#include "server.h"
#include "settings.h"


int main()
{
	Settings s;

	Server MyServer(LISTENING_PORT, true);
	MyServer.ListenForNewConnection();

	MyServer.HandleInput();

	//Just wait I guess
	while (true) {
		_sleep(1000);
	}
}