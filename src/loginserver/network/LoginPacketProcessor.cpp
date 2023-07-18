#include "LoginPacketProcessor.h"
#include "LoginConnection.h"

#include "../database/AccountsDAO.h"
#include "../database/AuthKeysDAO.h"
#include "../database/WorldServersDAO.h"

#include <shared/utilities/Logger.h>
#include <shared/utilities/StringUtility.h>

#include <openssl/rsa.h>
#include <openssl/rand.h>

namespace loginserver
{
	namespace network
	{
		LoginPacketProcessor sLoginPacketProcessor;

		LoginPacketProcessor::LoginPacketProcessor()
			: shared::network::PacketProcessor(1) // 1 thread for Login.
		{

		}

		void LoginPacketProcessor::ProcessPacket(shared::network::Packet* pPacket)
		{
			uint8_t u8Opcode = pPacket->Read<uint8_t>();
			LoginConnection* pConnection = (LoginConnection*)pPacket->GetConnection();

			switch (u8Opcode)
			{
			case 0x07: // GameGuard Authentication.
			{
				if (pConnection->GetConnectionState() != LoginConnection::ConnectionState::Connected)
					return pConnection->CloseConnection();

				uint32_t u32SessionId = pPacket->Read<uint32_t>();

				if (u32SessionId != pConnection->GetSessionId())
				{
					// Wrong SessionID received from client.
					// Either this is an encryption issue (however should not happen),
					// or there is an issue client side.

					sLogger.Error("LoginPacketProcessor::ProcessPacket(0x07) > GameGuard Authentication: invalid session ID (received %u, expected %u, client IP: %s",
						u32SessionId, pConnection->GetSessionId(), pConnection->GetClientAddress().c_str());

					return SendPacket_LoginFailed(pConnection, 1); // System Error.
				}

				pConnection->SetConnectionState(LoginConnection::ConnectionState::GameGuardOK);

				// Send GameGuard Auth OK Packet.
				shared::network::Packet* pResponse = new shared::network::Packet();

				pResponse->Write<uint8_t>(0x0B);
				pResponse->Write<uint32_t>(pConnection->GetSessionId());

				// 35 dummy bytes.
				pResponse->WriteZeroes(35);

				pConnection->SendPacket(pResponse);

				break;
			}
			case 0x0B:
			{
				if (pConnection->GetConnectionState() != LoginConnection::ConnectionState::GameGuardOK)
					return pConnection->CloseConnection();

				if (pPacket->GetSize() < 132)
				{
					sLogger.Error("LoginPacketProcessor::ProcessPacket(0x0B) > Invalid Login Packet Size (Expected at least 132, got %llu) - client IP: %s", pPacket->GetSize(),
						pConnection->GetClientAddress().c_str());

					return SendPacket_LoginFailed(pConnection, 1); // System Error.
				}

				uint8_t aDecrypted[128] = { 0 };
				if (RSA_private_decrypt(128, pPacket->GetDataPtr() + pPacket->GetPosition() + 4, aDecrypted, pConnection->GetRSAKeyPair().GetRSA(), RSA_NO_PADDING) != 128)
				{
					sLogger.Error("LoginPacketProcessor::ProcessPacket(0x0B) > RSA Decryption Failed - client IP: %s", pConnection->GetClientAddress().c_str());
					return SendPacket_LoginFailed(pConnection, 1); // System Error.
				}

				char* szAccountName = (char*)&aDecrypted[64];
				char* szPassword = (char*)&aDecrypted[96];

				// Authenticate user and retrieve account ID.
				std::string strPasswordHash = shared::utilities::StringUtility::HashStringSHA1(szPassword);

				uint32_t u32AccountId = database::AccountsDAO::Authenticate(szAccountName, strPasswordHash);

				if (u32AccountId == 0)
				{
					sLogger.Error("LoginPacketProcessor::ProcessPacket(0x0B) > Authentication failed for account '%s' - client IP: %s", szAccountName, pConnection->GetClientAddress().c_str());
					return SendPacket_LoginFailed(pConnection, 2); // ID or Password Invalid.
				}

				pConnection->SetAccountId(u32AccountId);

				// Generate Login Ticket
				uint32_t u32LoginTicket = 0;
				RAND_bytes((uint8_t*) &u32LoginTicket, 4);

				pConnection->SetLoginTicket(u32LoginTicket);

				pConnection->SetConnectionState(LoginConnection::ConnectionState::LoginOK);

				// Send Login Success Packet.
				shared::network::Packet* pResponse = new shared::network::Packet();

				pResponse->Write<uint8_t>(0x03);
				pResponse->Write<uint32_t>(u32AccountId); // AccountId
				pResponse->Write<uint32_t>(u32LoginTicket); // LoginOK
				pResponse->WriteZeroes(8);
				pResponse->Write<uint32_t>(0x3ea);
				pResponse->WriteZeroes(51);

				pConnection->SendPacket(pResponse);

				break;
			}
			case 0x05:
			{
				if (pConnection->GetConnectionState() != LoginConnection::ConnectionState::LoginOK)
					return pConnection->CloseConnection();

				// Server List Request.
				uint32_t u32AccountId = pPacket->Read<uint32_t>();
				uint32_t u32LoginTicket = pPacket->Read<uint32_t>();

				if (u32AccountId != pConnection->GetAccountId() || u32LoginTicket != pConnection->GetLoginTicket())
				{
					sLogger.Error("LoginPacketProcessor::ProcessPacket(0x05) > Invalid AccountID (%u-%u) / LoginTicket (%u-%u) - client IP: %s",
						u32AccountId, pConnection->GetAccountId(), u32LoginTicket, pConnection->GetLoginTicket(), pConnection->GetClientAddress().c_str());
					return pConnection->CloseConnection();
				}

				// Retrieve list of World Servers.
				std::vector<models::WorldServer> vWorldServers = database::WorldServersDAO::GetWorldServers();

				if (vWorldServers.size() == 0)
				{
					return SendPacket_LoginFailed(pConnection, 6); // No Game Server registered.
				}

				// Send Server List Packet.
				shared::network::Packet* pResponse = new shared::network::Packet();

				pResponse->Write<uint8_t>(0x04);
				pResponse->Write<uint8_t>((uint8_t)vWorldServers.size()); // Number of Servers
				pResponse->Write<uint8_t>(0); // TODO Last Server ID

				uint8_t u8MaxId = 0;
				for (models::WorldServer& rWorldServer : vWorldServers)
				{
					if (rWorldServer.u8ID > u8MaxId)
						u8MaxId = rWorldServer.u8ID;

					pResponse->Write<uint8_t>(rWorldServer.u8ID); // Server ID
					
					std::vector<uint8_t> vIPAddress = shared::utilities::StringUtility::IPv4StringToByteArray(rWorldServer.strAddress);

					pResponse->WriteBytes(vIPAddress.data(), 4);
					pResponse->Write<uint32_t>(rWorldServer.u16Port);
					pResponse->Write<uint8_t>(0x00); // Age Limit
					pResponse->Write<uint8_t>(0x01); // PvP Mode
					pResponse->Write<uint16_t>((uint16_t)rWorldServer.u32CurrentPlayers); // Current Players
					pResponse->Write<uint16_t>((uint16_t)rWorldServer.u32MaxPlayers); // Max Players
					pResponse->Write<uint8_t>(rWorldServer.bOnline ? 1 : 0); // IsOnline ?
					pResponse->Write<uint32_t>(1);
					pResponse->Write<uint8_t>(0); // Brackets ??
				}

				pResponse->Write<uint16_t>(u8MaxId + 1); // Max ID + 1
				pResponse->Write<uint8_t>(0x01);

				for (uint8_t i = 1; i <= u8MaxId; i++)
				{
					pResponse->Write<uint8_t>(2); // Number of Characters on Server
				}

				pConnection->SendPacket(pResponse);

				pConnection->SetConnectionState(LoginConnection::ConnectionState::ServerListSent);

				break;
			}
			case 0x02:
			{
				if (pConnection->GetConnectionState() != LoginConnection::ConnectionState::ServerListSent)
					return pConnection->CloseConnection();

				// Select Server for Play.
				uint32_t u32AccountId = pPacket->Read<uint32_t>();
				uint32_t u32LoginTicket = pPacket->Read<uint32_t>();
				uint8_t u8ServerID = pPacket->Read<uint8_t>();

				if (u32AccountId != pConnection->GetAccountId() || u32LoginTicket != pConnection->GetLoginTicket())
				{
					sLogger.Error("LoginPacketProcessor::ProcessPacket(0x02) > Invalid AccountID (%u-%u) / LoginTicket (%u-%u) - client IP: %s",
						u32AccountId, pConnection->GetAccountId(), u32LoginTicket, pConnection->GetLoginTicket(), pConnection->GetClientAddress().c_str());
					return pConnection->CloseConnection();
				}

				if (!database::WorldServersDAO::IsWorldServerAvailable(u8ServerID))
				{
					return SendPacket_PlayFailed(pConnection, 15); // Too many players (also covers the case where requested ServerID doesn't exist).
				}

				// Generate Game Login Key, and store to database.
				uint64_t u64GameAuthKey = 0;
				RAND_bytes((uint8_t*)&u64GameAuthKey, 8);

				if (!database::AuthKeysDAO::SetAuthKeys(pConnection->GetAccountId(), pConnection->GetLoginTicket(), u64GameAuthKey))
				{
					return SendPacket_LoginFailed(pConnection, 1); // System Error.
				}

				// Send Game Auth Keys.
				shared::network::Packet* pResponse = new shared::network::Packet();

				pResponse->Write<uint8_t>(0x07);
				pResponse->Write<uint64_t>(u64GameAuthKey);
				pResponse->Write<uint8_t>(u8ServerID);

				pResponse->WriteZeroes(0x0E);

				pConnection->SendPacket(pResponse);

				pConnection->SetConnectionState(LoginConnection::ConnectionState::ServerSelected);

				break;
			}
			default:
			{
				sLogger.Warning("PacketProcessor::OnPacketReceived > Unknown Packet Opcode %x", u8Opcode);
				break;
			}
			}
		}

		void LoginPacketProcessor::SendPacket_LoginFailed(LoginConnection* pConnection, uint8_t u8Reason)
		{
			shared::network::Packet* pPacket = new shared::network::Packet();

			pPacket->Write<uint8_t>(0x01);
			pPacket->Write<uint32_t>(u8Reason);

			pConnection->SendPacket(pPacket, true);
		}

		void LoginPacketProcessor::SendPacket_PlayFailed(LoginConnection* pConnection, uint8_t u8Reason)
		{
			shared::network::Packet* pPacket = new shared::network::Packet();

			pPacket->Write<uint8_t>(0x06);
			pPacket->Write<uint32_t>(u8Reason);

			pConnection->SendPacket(pPacket);
		}
	}
}