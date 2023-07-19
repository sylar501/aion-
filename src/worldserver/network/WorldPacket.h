#ifndef AION_WORLDSERVER_NETWORK_WORLDPACKET_H
#define AION_WORLDSERVER_NETWORK_WORLDPACKET_H

#include <shared/network/Packet.h>

#define WORLD_PACKET_SERVER_STATIC_CODE		0x54

namespace worldserver
{
	namespace network
	{
		class WorldPacket : public shared::network::Packet
		{
		public:
			WorldPacket(uint16_t u16Opcode);

			virtual ~WorldPacket();
		};
	}
}

#endif