#include "general.h"

int main()
{
#if _DEBUG
	Server MyServer(8443, true);
#else
	Server MyServer(8443, true);
#endif
	MyServer.ListenForNewConnection();

	MyServer.HandleInput();

	system("pause");
	return 0;
}