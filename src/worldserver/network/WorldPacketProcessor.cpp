#include "WorldPacketProcessor.h"
#include "WorldConnection.h"

#include <shared/utilities/Logger.h>

namespace worldserver
{
	namespace network
	{
		WorldPacketProcessor sWorldPacketProcessor;

		WorldPacketProcessor::WorldPacketProcessor()
			: shared::network::PacketProcessor(4) // 1 thread for World.
		{

		}

		void WorldPacketProcessor::ProcessPacket(shared::network::Packet* pPacket)
		{
			uint8_t u8Opcode = pPacket->Read<uint8_t>();
			WorldConnection* pConnection = (WorldConnection*)pPacket->GetConnection();

			switch (u8Opcode)
			{
			default:
			{
				sLogger.Warning("PacketProcessor::OnPacketReceived > Unknown Packet Opcode %x", u8Opcode);
				break;
			}
			}
		}
	}
}