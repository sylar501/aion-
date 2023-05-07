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

			break;
		}
		default:
		{
			sLogger.Warning("PacketProcessor::OnPacketReceived > Unknown Packet Opcode %x", this, u8Opcode);
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
