#include "LoginConnection.h"
#include "LoginPacketProcessor.h"

#include <shared/network/TCPClient.h>
#include <shared/utilities/Logger.h>

#include <openssl/rand.h>

namespace loginserver
{
	namespace network
	{
		shared::network::TCPConnection* LoginConnection::Create()
		{
			return new LoginConnection();
		}

		LoginConnection::LoginConnection()
		{

		}

		void LoginConnection::OnConnect()
		{
			shared::network::TCPConnection::OnConnect();

			// Generate Session ID.
			RAND_bytes((uint8_t*)&m_u32SessionId, sizeof(uint32_t));

			sLogger.Info("LoginConnection(0x%x)::OnConnect > From %s, SessionID %u", this, m_spTCPClient->GetSocket().remote_endpoint().address().to_string().c_str(), m_u32SessionId);

			m_eConnectionState = ConnectionState::Connected;

			// Make Init Packet.
			shared::network::Packet* pInitPacket = new shared::network::Packet();

			pInitPacket->Write<uint8_t>(0); // Opcode Init
			pInitPacket->Write<uint32_t>(m_u32SessionId); // sessionId
			pInitPacket->Write<uint32_t>(0x0000c621); // protocol revision
			pInitPacket->WriteBytes(m_oRSAKeyPair.GetEncryptedModulus(), 128);

			pInitPacket->WriteZeroes(16);

			pInitPacket->WriteBytes(m_oEncryption.GetBlowfishKey(), 16);
			pInitPacket->Write<uint32_t>(197635);
			pInitPacket->Write<uint32_t>(2097152);

			SendPacket(pInitPacket);
		}

		void LoginConnection::OnDisconnect()
		{
			sLogger.Info("LoginConnection(0x%x)::OnDisconnect > %u", this, m_u32SessionId);

			shared::network::TCPConnection::OnDisconnect();
		}

		void LoginConnection::OnPacketReceived(shared::network::Packet* pPacket)
		{
			// Decrypt Message.
			m_oEncryption.Decrypt(pPacket->GetDataPtr(), pPacket->GetSize());

			// Handle Packet.
			pPacket->SetPosition(0);
			sLoginPacketProcessor.EnqueuePacket(pPacket);
		}

		void LoginConnection::SendPacket(shared::network::Packet* pPacket, bool bCloseAfterSend)
		{
			pPacket->Resize(pPacket->GetPosition() + 4 + 8 - pPacket->GetPosition() % 8);

			size_t u64Size = m_oEncryption.Encrypt(pPacket->GetDataPtr() + 2, pPacket->GetPosition() - 4) + 2;

			pPacket->SetPosition(0);
			pPacket->Write<uint16_t>((uint16_t)u64Size);

			m_spTCPClient->GetSocket().async_send(asio::buffer(pPacket->GetDataPtr(), u64Size), asio::bind_executor(m_spTCPClient->GetSendStrand(), [pPacket, bCloseAfterSend, this](std::error_code ec, size_t /* bytesTransferred */)
				{
					delete pPacket;

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
	}
}