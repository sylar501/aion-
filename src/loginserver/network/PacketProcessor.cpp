#include "PacketProcessor.h"
#include "LoginConnection.h"

#include <shared/utilities/Logger.h>

#include <openssl/rsa.h>

PacketProcessor::PacketProcessor(LoginConnection& rConnection)
: m_rConnection(rConnection)
{

}

void PacketProcessor::OnPacketReceived(Buffer& rBuffer)
{
	uint8_t u8Opcode = rBuffer.ReadUInt8();

	switch (u8Opcode)
	{
		case 0x07: // GameGuard Authentication.
		{
			if (m_rConnection.GetConnectionState() != LoginConnection::ConnectionState::Connected)
				return m_rConnection.CloseConnection();

			uint32_t u32SessionId = rBuffer.ReadUInt32();

			if (u32SessionId != m_rConnection.GetSessionId())
			{
				// Wrong SessionID received from client.
				// Either this is an encryption issue (however should not happen),
				// or there is an issue client side.
				return SendPacket_LoginFailed(2);
			}

			m_rConnection.SetConnectionState(LoginConnection::ConnectionState::GameGuardOK);

			// Send GameGuard Auth OK Packet.
			Buffer* pPacket = new Buffer();

			pPacket->WriteUInt8(0x0B);
			pPacket->WriteUInt32(m_rConnection.GetSessionId());

			// 35 dummy bytes.
			for (int i = 0; i < 8; i++)
				pPacket->WriteUInt32(0x00);
			for (int i = 0; i < 3; i++)
				pPacket->WriteUInt8(0x00);

			m_rConnection.SendPacket(pPacket);

			break;
		}
		case 0x0B:
		{
			if (m_rConnection.GetConnectionState() != LoginConnection::ConnectionState::GameGuardOK)
				return m_rConnection.CloseConnection();

			sLogger.Info("Login Packet");

			if (rBuffer.GetSize() < 132)
			{
				sLogger.Error("PacketProcessor::OnPacketReceived > Invalid Login Packet Size (Expected at least 132, got %llu)", rBuffer.GetSize());
				return m_rConnection.CloseConnection();
			}

			uint8_t aDecrypted[128] = { 0 };
			if (RSA_private_decrypt(128, rBuffer.GetBuffer() + rBuffer.GetReadPosition() + 4, aDecrypted, m_rConnection.GetRSAKeyPair().GetRSA(), RSA_NO_PADDING) != 128)
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
			Buffer* pPacket = new Buffer();

			pPacket->WriteUInt8(0x03);
			pPacket->WriteUInt32(0x00000322); // AccountId
			pPacket->WriteUInt32(0xc2458e22); // LoginOK
			pPacket->WriteUInt32(0x00);
			pPacket->WriteUInt32(0x00);
			pPacket->WriteUInt32(1002);
			pPacket->WriteUInt32(126282165);

			// 47 dummy bytes.
			for (int i = 0; i < 11; i++)
				pPacket->WriteUInt32(0x00);
			for (int i = 0; i < 3; i++)
				pPacket->WriteUInt8(0x00);

			m_rConnection.SendPacket(pPacket);

			break;
		}
		case 0x05:
		{
			if (m_rConnection.GetConnectionState() != LoginConnection::ConnectionState::LoginOK)
				return m_rConnection.CloseConnection();

			// Server List Request.
			uint32_t u32AccountId = rBuffer.ReadUInt32();
			uint32_t u32LoginTicket = rBuffer.ReadUInt32();

			// Send Server List Packet.
			Buffer* pPacket = new Buffer();

			pPacket->WriteUInt8(0x04);
			pPacket->WriteUInt8(1); // Number of Servers
			pPacket->WriteUInt8(1); // Last Server ID
			// foreach GameServer
			pPacket->WriteUInt8(1); // Server ID
			uint8_t aServerIP[4] = { 127, 0, 0, 1 };
			pPacket->Append(aServerIP, 4);
			pPacket->WriteUInt32(6667);
			pPacket->WriteUInt8(0x00); // Age Limit
			pPacket->WriteUInt8(0x01); // PvP Mode
			pPacket->WriteUInt16(0x22); // Current Players
			pPacket->WriteUInt16(0x30); // Max Players
			pPacket->WriteUInt8(1); // IsOnline ?
			pPacket->WriteUInt32(1);
			pPacket->WriteUInt8(1);
			// endforeach

			pPacket->WriteUInt16(2); // Max ID + 1
			pPacket->WriteUInt8(0x01);
			// 49 dummy bytes.
			for (int i = 0; i < 12; i++)
				pPacket->WriteUInt32(0x00);
			pPacket->WriteUInt8(0x00);

			// for 1 to MaxId
			pPacket->WriteUInt8(2); // Number Characters on Server
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
	Buffer* pPacket = new Buffer();

	pPacket->WriteUInt8(0x01);
	pPacket->WriteUInt32(u8Reason);

	m_rConnection.SendPacket(pPacket, true);
}
