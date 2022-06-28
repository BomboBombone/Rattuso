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
	Heartbeat = 10
};

#endif