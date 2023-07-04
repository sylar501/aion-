#include "PacketProcessor.h"
#include "LoginConnection.h"

#include <shared/utilities/Logger.h>

#include <openssl/rsa.h>

PacketProcessor::PacketProcessor(LoginConnection& rConnection)
: m_rConnection(rConnection)
{

}

void PacketProcessor::OnPacketReceived(shared::network::Packet& rPacket)
{
	uint8_t u8Opcode = rPacket.Read<uint8_t>();

	switch (u8Opcode)
	{
		case 0x07: // GameGuard Authentication.
		{
			if (m_rConnection.GetConnectionState() != LoginConnection::ConnectionState::Connected)
				return m_rConnection.CloseConnection();

			uint32_t u32SessionId = rPacket.Read<uint32_t>();

			if (u32SessionId != m_rConnection.GetSessionId())
			{
				// Wrong SessionID received from client.
				// Either this is an encryption issue (however should not happen),
				// or there is an issue client side.
				return SendPacket_LoginFailed(2);
			}

			m_rConnection.SetConnectionState(LoginConnection::ConnectionState::GameGuardOK);

			// Send GameGuard Auth OK Packet.
			shared::network::Packet* pPacket = new shared::network::Packet();

			pPacket->Write<uint8_t>(0x0B);
			pPacket->Write<uint32_t>(m_rConnection.GetSessionId());

			// 35 dummy bytes.
			pPacket->WriteZeroes(35);

			m_rConnection.SendPacket(pPacket);

			break;
		}
		case 0x0B:
		{
			if (m_rConnection.GetConnectionState() != LoginConnection::ConnectionState::GameGuardOK)
				return m_rConnection.CloseConnection();

			sLogger.Info("Login Packet");

			if (rPacket.GetSize() < 132)
			{
				sLogger.Error("PacketProcessor::OnPacketReceived > Invalid Login Packet Size (Expected at least 132, got %llu)", rPacket.GetSize());
				return m_rConnection.CloseConnection();
			}

			uint8_t aDecrypted[128] = { 0 };
			if (RSA_private_decrypt(128, rPacket.GetDataPtr() + rPacket.GetPosition() + 4, aDecrypted, m_rConnection.GetRSAKeyPair().GetRSA(), RSA_NO_PADDING) != 128)
			{
				sLogger.Error("PacketProcessor::OnPacketReceived > RSA Decryption Failed");
				return m_rConnection.CloseConnection();
			}

			char* szAccountName = (char*)&aDecrypted[64];
			char* szPassword = (char*)&aDecrypted[96];

			sLogger.Info("Login: %s / %s", szAccountName, szPassword);

			// TODO Handle Authentication.

			m_rConnection.SetConnectionState(LoginConnection::ConnectionState::LoginOK);

			// Send Login Success Packet.
			shared::network::Packet* pPacket = new shared::network::Packet();

			pPacket->Write<uint8_t>(0x03);
			pPacket->Write<uint32_t>(0x00000322); // AccountId
			pPacket->Write<uint32_t>(0xc2458e22); // LoginOK
			pPacket->WriteZeroes(8);
			pPacket->Write<uint32_t>(1002);
			pPacket->Write<uint32_t>(126282165);
			pPacket->WriteZeroes(47);

			m_rConnection.SendPacket(pPacket);

			break;
		}
		case 0x05:
		{
			if (m_rConnection.GetConnectionState() != LoginConnection::ConnectionState::LoginOK)
				return m_rConnection.CloseConnection();

			// Server List Request.
			uint32_t u32AccountId = rPacket.Read<uint32_t>();
			uint32_t u32LoginTicket = rPacket.Read<uint32_t>();

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

			m_rConnection.SendPacket(pPacket);

			break;
		}
		default:
		{
			sLogger.Warning("PacketProcessor::OnPacketReceived > Unknown Packet Opcode %x", u8Opcode);
			break;
		}
	}
}

void PacketProcessor::SendPacket_LoginFailed(uint8_t u8Reason)
{
	shared::network::Packet* pPacket = new shared::network::Packet();

	pPacket->Write<uint8_t>(0x01);
	pPacket->Write<uint32_t>(u8Reason);

	m_rConnection.SendPacket(pPacket, true);
}
