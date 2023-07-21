#include "WorldPacketProcessor.h"
#include "WorldConnection.h"

#include "../configuration/WorldServerConfiguration.h"
#include "../database/TicketsDAO.h"

#include <shared/utilities/Logger.h>

#include <openssl/rand.h>

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
			pPacket->SetPosition(pPacket->GetPosition() + 3);
			
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
			case 0x9F: // Client Ping
			{
				WorldPacket* pResponse = new WorldPacket(0x8E);

				pResponse->Write<uint16_t>(0);

				pConnection->SendPacket(pResponse);

				break;
			}
			case 0xF1: // Client Soft Disconnection.
			{
				sLogger.Info("Soft Disconnection");
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
				
				uint8_t u8CharacterCreationFlag = (24 * 0x10) | 1; // 2 characters max. TODO implement options here.
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
			case 0xF6: // Client Quit Request.
			{
				bool bLogOut = pPacket->Read<uint8_t>() == 1;

				pConnection->SendPacket_Quit(false, !bLogOut);

				break;
			}
			case 0x104: // Authentication.
			{
				if (pConnection->GetConnectionState() != WorldConnection::ConnectionState::Connected)
				{
					sLogger.Error("Received Authentication Packet while connection is in invalid state - client IP: %s",
						pConnection->GetClientAddress().c_str());
					return pConnection->CloseConnection();
				}

				// Read the World Ticket into a byte array first, since AION client sends it in a weird way ...
				uint8_t aWorldTicketBytes[8];
				pPacket->ReadBytes(&aWorldTicketBytes[4], 4);
				pPacket->ReadBytes(aWorldTicketBytes, 4);

				uint64_t u64WorldTicket = *((uint64_t*)aWorldTicketBytes);
				
				uint32_t u32AccountId = pPacket->Read<uint32_t>();
				uint32_t u32LoginTicket = pPacket->Read<uint32_t>();

				if (!database::TicketsDAO::ValidateTickets(u32AccountId, u32LoginTicket, u64WorldTicket, configuration::sWorldServerConfiguration.WORLDSERVER_ID))
				{
					sLogger.Error("Failed to validate authentication tickets - client IP: %s", pConnection->GetClientAddress().c_str());
					return SendPacket_AuthenticationResponse(pConnection, false, "");
				}

				pConnection->SetAccountId(u32AccountId);
				pConnection->SetConnectionState(WorldConnection::ConnectionState::Authenticated);

				SendPacket_AuthenticationResponse(pConnection, true, "");

				break;
			}
			case 0x105: // Character List Request.
			{
				if (pConnection->GetConnectionState() != WorldConnection::ConnectionState::Authenticated)
				{
					sLogger.Error("Received Character List Request while connection is in invalid state - client IP: %s",
						pConnection->GetClientAddress().c_str());
					return pConnection->CloseConnection();
				}

				uint32_t u32Key = pPacket->Read<uint32_t>();

				// Send Character List.
				WorldPacket* pResponse = new WorldPacket(0xC8);

				pResponse->Write<uint32_t>(u32Key);

				pResponse->Write<uint8_t>(0); // Number of Characters.
				// foreach Player
				
				// endforeach

				pConnection->SendPacket(pResponse);

				break;
			}
			case 0x12a: // Select Server Request.
			{
				if (pConnection->GetConnectionState() != WorldConnection::ConnectionState::Authenticated)
				{
					sLogger.Error("Received Select Server Request while connection is in invalid state - client IP: %s",
						pConnection->GetClientAddress().c_str());
					return pConnection->CloseConnection();
				}

				// Generate a new Login Ticket.
				uint32_t u32LoginTicket;
				RAND_bytes((uint8_t*)&u32LoginTicket, 4);

				if (!database::TicketsDAO::SetLoginTicket(pConnection->GetAccountId(), u32LoginTicket))
				{
					return pConnection->CloseConnection();
				}

				WorldPacket* pPacket = new WorldPacket(0xFF);

				pPacket->Write<uint8_t>(0);
				pPacket->Write<uint32_t>(u32LoginTicket);

				pConnection->SendPacket(pPacket);

				break;
			}
			case 0x12c: // TODO Client MAC Address
			{
				break;
			}
			default:
			{
				sLogger.Warning("PacketProcessor::OnPacketReceived > Unknown Packet Opcode %x", u16Opcode);
				break;
			}
			}
		}

		void WorldPacketProcessor::SendPacket_AuthenticationResponse(std::shared_ptr<WorldConnection> pConnection, bool bResponse, const std::string& strAccountName)
		{
			WorldPacket* pPacket = new WorldPacket(0xC7);

			pPacket->Write<uint32_t>(bResponse ? 0x00 : 0x01);
			pPacket->WriteString(strAccountName);

			pConnection->SendPacket(pPacket, !bResponse);
		}
	}
}