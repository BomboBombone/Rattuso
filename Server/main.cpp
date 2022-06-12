#include "general.h"

int main()
{
	Server MyServer(8559, true);
	MyServer.ListenForNewConnection();

	MyServer.HandleInput();

	system("pause");
	return 0;
}