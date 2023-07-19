#include "WorldPacket.h"

namespace worldserver
{
	namespace network
	{
		WorldPacket::WorldPacket(uint16_t u16Opcode)
		{
			uint16_t u16EncodedOpcode = (u16Opcode + 0xBD) ^ 0xFF;
			Write<uint16_t>(u16EncodedOpcode);
			Write<uint8_t>(WORLD_PACKET_SERVER_STATIC_CODE);
			Write<uint16_t>(~u16EncodedOpcode);
		}

		WorldPacket::~WorldPacket()
		{

		}
	}
}