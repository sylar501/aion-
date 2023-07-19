#include "WorldPacketProcessor.h"
#include "WorldConnection.h"

#include "../configuration/WorldServerConfiguration.h"

#include <shared/utilities/Logger.h>

namespace worldserver
{
	namespace network
	{
		WorldPacketProcessor sWorldPacketProcessor;

		WorldPacketProcessor::WorldPacketProcessor()
			: shared::network::PacketProcessor(4) // 4 threads for World.
		{

		}

		void WorldPacketProcessor::ProcessPacket(shared::network::Packet* pPacket)
		{
			std::shared_ptr<WorldConnection> pConnection = std::dynamic_pointer_cast<WorldConnection>(pPacket->GetConnection());

			if (!pConnection->IsOpen()) return;

			uint16_t u16Opcode = pPacket->Read<uint16_t>();
			
			switch (u16Opcode)
			{
			case 0x81: // Time Synchronization.
			{
				uint32_t u32ClientTime = pPacket->Read<uint32_t>();
				
				WorldPacket* pResponse = new WorldPacket(0x27);

				pResponse->Write<uint32_t>((uint32_t)time(NULL));
				pResponse->Write<uint32_t>(u32ClientTime);

				pConnection->SendPacket(pResponse);

				break;
			}
			case 0xF1: // Client Soft Disconnection.
			{
				pConnection->ShutdownConnection();
				break;
			}
			case 0xF3: // Server Information Request.
			{
				if (pConnection->GetConnectionState() != WorldConnection::ConnectionState::Connected)
				{
					sLogger.Error("Received Server Information Request while connection is in invalid state - client IP: %s",
						pConnection->GetClientAddress().c_str());
					return pConnection->CloseConnection();
				}

				WorldPacket* pResponse = new WorldPacket(0x00);

				pResponse->Write<uint8_t>(0);
				pResponse->Write<uint8_t>((uint8_t)configuration::sWorldServerConfiguration.WORLDSERVER_ID);
				pResponse->Write<uint32_t>(0x01B1A3);
				pResponse->Write<uint32_t>(0x01B1A3);
				pResponse->Write<uint32_t>(0);
				pResponse->Write<uint32_t>(0x01B1A3);
				pResponse->Write<uint32_t>(0x4E9E4A);
				pResponse->Write<uint8_t>(0);
				pResponse->Write<uint8_t>(2); // Country Code
				pResponse->Write<uint8_t>(0);
				
				uint8_t u8CharacterCreationFlag = 2 * 0x10; // 2 characters max. TODO implement options here.
				pResponse->Write<uint8_t>(u8CharacterCreationFlag);

				pResponse->Write<uint32_t>((uint32_t)time(NULL));
				pResponse->Write<uint16_t>(0x015E);
				pResponse->Write<uint16_t>(0x0A01);
				pResponse->Write<uint16_t>(0x0A01);
				pResponse->Write<uint16_t>(0x0370A);
				pResponse->Write<uint8_t>(2);
				pResponse->Write<uint8_t>(0);
				pResponse->Write<uint8_t>(30); // Reconnection Time in seconds.
				pResponse->Write<uint8_t>(0); // Christmas Mode.
				pResponse->Write<uint32_t>(0);

				pResponse->Write<uint16_t>(1);
				pResponse->Write<uint8_t>(0);
				uint8_t aChatServerIP[4] = { 127, 0, 0, 1 };
				pResponse->WriteBytes(aChatServerIP, 4);
				pResponse->Write<uint16_t>(14020); // Chat Server Port

				pConnection->SendPacket(pResponse);

				break;
			}
			case 0x104: // Authentication.
			{
				uint64_t u64GameAuthKey = pPacket->Read<uint64_t>();
				uint32_t u32AccountId = pPacket->Read<uint32_t>();
				uint32_t u32LoginTicket = pPacket->Read<uint32_t>();

				sLogger.Info("Game Auth: %lu - %u - %u", u64GameAuthKey, u32AccountId, u32LoginTicket);
			}
			default:
			{
				sLogger.Warning("PacketProcessor::OnPacketReceived > Unknown Packet Opcode %x", u16Opcode);
				break;
			}
			}
		}
	}
}