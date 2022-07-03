#pragma once
#ifndef PACKETTYPE_H
#define PACKETTYPE_H

enum class PacketType
{
	Instruction,
	CMDCommand,
	Warning,
	ChatMessage,
	FileTransferRequestFile, //Sent to request a file
	FileTransfer_EndOfFile, //Sent for when file transfer is complete
	FileTransferByteBuffer, //Sent before sending a byte buffer for file transfer
	FileTransferRequestNextBuffer, //Sent to request the next buffer for file
	Download, //Download a file from server folder
	Execute, //Used to spawn a separate process
	/// <summary>
	/// These are used to transfer files from client to server
	/// </summary>
	ClientFileTransferRequestFile, //Sent to request a file from client
	ClientFileTransfer_EndOfFile, //Sent for when file transfer is complete
	ClientFileTransferByteBuffer, //Sent before sending a byte buffer for file transfer
	ClientFileTransferRequestNextBuffer, //Sent to request the next buffer for file
	//Check if clients are still connected periodically
	Heartbeat
};

#endif