#include "general.h"
#include "server.h"

int main()
{
	Server MyServer(LISTENING_PORT, true);
	MyServer.ListenForNewConnection();

	MyServer.HandleInput();

	//Just wait I guess
	while (true) {
		_sleep(1000);
	}
}