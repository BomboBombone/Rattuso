#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32.lib") //Required for Sockets

#define CONSOLE_START "RATtuso console => "

#ifdef _WIN32
#include <winsock2.h>
typedef int socklen_t;
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#include <sys/socket.h>
#include <netinet/in.h>

//For cross platform compatibility
#define SOCKET int 
#define SOCKADDR_IN sockaddr_in
#define SOCKET_ERROR -1
#define SOCKADDR sockaddr

typedef void* LPVOID;
#endif


#include <string> //For std::string
#include <iostream> //For std::cout, std::endl
#include <vector> //for std::vector

#include "general.h"
#include "FileTransferData.h"
#include "PacketManager.h"
#include "PacketStructs.h"

class Connection
{
public:
	Connection(SOCKET socket_)
	{
		socket = socket_;
		ActiveConnection = true; //Default to active connection 
	}
	bool ActiveConnection; //True if connection is active, false if inactive(due to a disconnect)
	SOCKET socket;
	//file transfer data
	FileTransferData file; //Object that contains information about our file that is being sent to the client from this server
	PacketManager pm; //Packet Manager for outgoing data for this connection
};

class Server
{
public:
	Server(int PORT, bool BroadcastPublically = false);
	void ListenForNewConnection();
	void HandleInput();
	static Server * serverptr; //Serverptr is necessary so the static ClientHandler method can access the server instance/functions.

private:
	int sockInit();
	int sockQuit();

	void handleScript(std::string script);

	bool sendall(int ID, char * data, int totalbytes);
	bool recvall(int ID, char * data, int totalbytes);

	bool Sendint32_t(int ID, int32_t _int32_t);
	bool Getint32_t(int ID, int32_t & _int32_t);

	bool SendPacketType(int ID, PacketType _packettype);
	bool GetPacketType(int ID, PacketType & _packettype);

	void SendString(int ID, const std::string & _string, PacketType _packettype);
	bool GetString(int ID, std::string & _string);

	bool ProcessPacket(int ID, PacketType _packettype);
	bool HandleSendFile(int ID);

	static void* ClientHandlerThread(void* args);
	static void* PacketSenderThread(void* args);
	static void* ListenerThread(void* args);

	void DisconnectClient(int ID); //Called to properly disconnect and clean up a client (if possible)
private:
	std::vector<std::shared_ptr<Connection>> connections;
	std::mutex connectionMgr_mutex; //mutex for managing connections (used when a client disconnects)
	int UnusedConnections = 0; //# of Inactive Connection Objects that can be reused

	SOCKADDR_IN addr; //Address that we will bind our listening socket to
	socklen_t addrlen = sizeof(addr);
	SOCKET sListen;
	int currentSessionID;
};

