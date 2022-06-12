#pragma once
#ifndef PACKETSTRUCTS_H
#define PACKETSTRUCTS_H

#ifndef _WIN32
	#include <netinet/in.h>
#else

#endif

#include <string>

#include "PacketType.h"
#include "Packet.h"

namespace PS //Packet Structures Namespace
{
	class Message
	{
	public:
		Message(std::string);
		Packet toPacket(PacketType _packettype); //Converts ChatMessage to packet
	private:
		std::string message;
	};

}

#endif