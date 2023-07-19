#include "WorldConnection.h"
#include "WorldPacketProcessor.h"

#include <shared/network/TCPClient.h>
#include <shared/utilities/Logger.h>

#include <openssl/rand.h>

namespace worldserver
{
	namespace network
	{
		shared::network::TCPConnection* WorldConnection::Create()
		{
			return new WorldConnection();
		}

		WorldConnection::WorldConnection()
		{

		}

		void WorldConnection::OnConnect()
		{
			shared::network::TCPConnection::OnConnect();

			// Generate Session ID.
			RAND_bytes((uint8_t*)&m_u32SessionId, sizeof(uint32_t));

			sLogger.Info("WorldConnection(0x%x)::OnConnect > From %s, SessionID %u", this, m_spTCPClient->GetSocket().remote_endpoint().address().to_string().c_str(), m_u32SessionId);

			m_eConnectionState = ConnectionState::Connected;

			// Make Init Packet.			
			WorldPacket* pInitPacket = new WorldPacket(0x48);

			pInitPacket->Write<uint32_t>((m_oEncryption.GetBaseKey() ^ 0xCD92E451) + 0x3FF2CC87);

			SendPacket(pInitPacket);
		}

		void WorldConnection::OnDisconnect()
		{
			sLogger.Info("WorldConnection(0x%x)::OnDisconnect > %u", this, m_u32SessionId);

			shared::network::TCPConnection::OnDisconnect();
		}

		void WorldConnection::OnPacketReceived(shared::network::Packet* pPacket)
		{
			// Decrypt Message.
			if (m_oEncryption.Decrypt(pPacket->GetDataPtr(), pPacket->GetSize()))
			{
				// Handle Packet.
				pPacket->SetPosition(0);
				sWorldPacketProcessor.EnqueuePacket(pPacket);
			}
			else
			{
				sLogger.Error("WorldConnection::OnPacketReceived > Failed to validate client packet!");
			}
		}

		void WorldConnection::SendPacket(WorldPacket* pPacket, bool bCloseAfterSend)
		{
			m_oEncryption.Encrypt(pPacket->GetDataPtr() + 2, pPacket->GetPosition() - 4);

			uint64_t u64PacketSize = pPacket->GetPosition();

			pPacket->SetPosition(0);
			pPacket->Write<uint16_t>((uint16_t)u64PacketSize);

			if (IsOpen())
			{
				m_spTCPClient->GetSocket().async_send(asio::buffer(pPacket->GetDataPtr(), u64PacketSize), asio::bind_executor(m_spTCPClient->GetSendStrand(), [pPacket, bCloseAfterSend, this](std::error_code ec, size_t /* bytesTransferred */)
					{
						delete pPacket;

						if (ec)
						{
							sLogger.Error("WorldConnection(0x%x)::Send > %s", this, ec.message().c_str());
						}

						if (bCloseAfterSend)
						{
							ShutdownConnection();
						}
					}));
			}
		}
	}
}