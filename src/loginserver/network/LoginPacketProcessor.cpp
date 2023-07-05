#include "LoginPacketProcessor.h"
#include "LoginConnection.h"

#include <shared/utilities/Logger.h>

#include <openssl/rsa.h>

LoginPacketProcessor sLoginPacketProcessor;

LoginPacketProcessor::LoginPacketProcessor()
: shared::network::PacketProcessor(1) // 1 thread for Login.
{

}

void LoginPacketProcessor::ProcessPacket(shared::network::Packet* pPacket)
{
	uint8_t u8Opcode = pPacket->Read<uint8_t>();
	LoginConnection* pConnection = (LoginConnection*) pPacket->GetConnection();

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
				return SendPacket_LoginFailed(pConnection, 2);
			}

			pConnection->SetConnectionState(LoginConnection::ConnectionState::GameGuardOK);

			// Send GameGuard Auth OK Packet.
			shared::network::Packet* pPacket = new shared::network::Packet();

			pPacket->Write<uint8_t>(0x0B);
			pPacket->Write<uint32_t>(pConnection->GetSessionId());

			// 35 dummy bytes.
			pPacket->WriteZeroes(35);

			pConnection->SendPacket(pPacket);

			break;
		}
		case 0x0B:
		{
			if (pConnection->GetConnectionState() != LoginConnection::ConnectionState::GameGuardOK)
				return pConnection->CloseConnection();

			sLogger.Info("Login Packet");

			if (pPacket->GetSize() < 132)
			{
				sLogger.Error("PacketProcessor::OnPacketReceived > Invalid Login Packet Size (Expected at least 132, got %llu)", pPacket->GetSize());
				return pConnection->CloseConnection();
			}

			uint8_t aDecrypted[128] = { 0 };
			if (RSA_private_decrypt(128, pPacket->GetDataPtr() + pPacket->GetPosition() + 4, aDecrypted, pConnection->GetRSAKeyPair().GetRSA(), RSA_NO_PADDING) != 128)
			{
				sLogger.Error("PacketProcessor::OnPacketReceived > RSA Decryption Failed");
				return pConnection->CloseConnection();
			}

			char* szAccountName = (char*)&aDecrypted[64];
			char* szPassword = (char*)&aDecrypted[96];

			sLogger.Info("Login: %s / %s", szAccountName, szPassword);

			// TODO Handle Authentication.

			pConnection->SetConnectionState(LoginConnection::ConnectionState::LoginOK);

			// Send Login Success Packet.
			shared::network::Packet* pPacket = new shared::network::Packet();

			pPacket->Write<uint8_t>(0x03);
			pPacket->Write<uint32_t>(0x00000322); // AccountId
			pPacket->Write<uint32_t>(0xc2458e22); // LoginOK
			pPacket->WriteZeroes(8);
			pPacket->Write<uint32_t>(1002);
			pPacket->Write<uint32_t>(126282165);
			pPacket->WriteZeroes(47);

			pConnection->SendPacket(pPacket);

			break;
		}
		case 0x05:
		{
			if (pConnection->GetConnectionState() != LoginConnection::ConnectionState::LoginOK)
				return pConnection->CloseConnection();

			// Server List Request.
			uint32_t u32AccountId = pPacket->Read<uint32_t>();
			uint32_t u32LoginTicket = pPacket->Read<uint32_t>();

			// Send Server List Packet.
			shared::network::Packet* pPacket = new shared::network::Packet();

			pPacket->Write<uint8_t>(0x04);
			pPacket->Write<uint8_t>(1); // Number of Servers
			pPacket->Write<uint8_t>(1); // Last Server ID
			// foreach GameServer
			pPacket->Write<uint8_t>(1); // Server ID
			uint8_t aServerIP[4] = { 127, 0, 0, 1 };
			pPacket->WriteBytes(aServerIP, 4);
			pPacket->Write<uint32_t>(6667);
			pPacket->Write<uint8_t>(0x00); // Age Limit
			pPacket->Write<uint8_t>(0x01); // PvP Mode
			pPacket->Write<uint16_t>(0x22); // Current Players
			pPacket->Write<uint16_t>(0x30); // Max Players
			pPacket->Write<uint8_t>(1); // IsOnline ?
			pPacket->Write<uint32_t>(1);
			pPacket->Write<uint8_t>(1);
			// endforeach

			pPacket->Write<uint16_t>(2); // Max ID + 1
			pPacket->Write<uint8_t>(0x01);
			pPacket->WriteZeroes(49);

			// for 1 to MaxId
			pPacket->Write<uint8_t>(2); // Number Characters on Server
			// endfor

			pConnection->SendPacket(pPacket);

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
