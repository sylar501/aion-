#include "LoginConnection.h"

#include <shared/network/Buffer.h>
#include <shared/network/TCPClient.h>
#include <shared/utilities/Logger.h>

#include <openssl/rand.h>

shared::TCPConnection* LoginConnection::Create()
{
	return new LoginConnection();
}

LoginConnection::LoginConnection()
: m_oPacketProcessor(*this)
{

}

void LoginConnection::OnConnect()
{
	shared::TCPConnection::OnConnect();

	// Generate Session ID.
	RAND_bytes((uint8_t*)&m_u32SessionId, sizeof(uint32_t));

	sLogger.Info("LoginConnection(0x%x)::OnConnect > From %s, SessionID %u", this, m_spTCPClient->GetSocket().remote_endpoint().address().to_string().c_str(), m_u32SessionId);

	m_eConnectionState = ConnectionState::Connected;

	// Make Init Packet.
	Buffer* pInitPacket = new Buffer();

	pInitPacket->WriteInt8(0); // Opcode Init
	pInitPacket->WriteUInt32(m_u32SessionId); // sessionId
	pInitPacket->WriteUInt32(0x0000c621); // protocol revision
	pInitPacket->Append(m_oRSAKeyPair.GetEncryptedModulus(), 128);
	
	for (int8_t i = 0; i < 4; i++)
		pInitPacket->WriteUInt32(0);

	pInitPacket->Append(m_oEncryption.GetBlowfishKey(), 16);
	pInitPacket->WriteUInt32(197635);
	pInitPacket->WriteUInt32(2097152);
	
	SendPacket(pInitPacket);
}

void LoginConnection::OnDisconnect()
{
	sLogger.Info("LoginConnection(0x%x)::OnDisconnect > %u", this, m_u32SessionId);

	shared::TCPConnection::OnDisconnect();
}

void LoginConnection::OnPacketReceived(Buffer& rBuffer)
{
	// Decrypt Message.
	m_oEncryption.Decrypt(rBuffer.GetBuffer(), rBuffer.GetSize());

	// Handle Packet.
	m_oPacketProcessor.OnPacketReceived(rBuffer);
}

void LoginConnection::SendPacket(Buffer* pBuffer, bool bCloseAfterSend)
{
	pBuffer->Resize(pBuffer->GetWritePosition() + 4 + 8 - pBuffer->GetWritePosition() % 8);

	size_t u64Size = m_oEncryption.Encrypt(pBuffer->GetBuffer() + 2, pBuffer->GetWritePosition() - 4) + 2;

	pBuffer->SetWritePosition(0);
	pBuffer->WriteUInt16((uint16_t)u64Size);

	m_spTCPClient->GetSocket().async_send(asio::buffer(pBuffer->GetBuffer(), u64Size), asio::bind_executor(m_spTCPClient->GetSendStrand(), [pBuffer, bCloseAfterSend, this](std::error_code ec, size_t /* bytesTransferred */)
	{
		delete pBuffer;

		if (ec)
		{
			sLogger.Error("LoginConnection(0x%x)::Send > %s", this, ec.message().c_str());
		}

		if (bCloseAfterSend)
		{
			CloseConnection();
		}
	}));
}