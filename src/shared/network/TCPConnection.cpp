#include <shared/network/TCPConnection.h>
#include <shared/network/TCPClient.h>

namespace shared
{
	namespace network
	{
		void TCPConnection::SetTCPClient(std::shared_ptr<TCPClient> spTCPClient)
		{
			m_spTCPClient = spTCPClient;
		}

		void TCPConnection::OnConnect()
		{
			m_vReceiveBuffer.resize(8192);
			m_u32ReceiveBufferPosition = 0;
		}

		void TCPConnection::OnDisconnect()
		{
			m_spTCPClient.reset();
		}

		void TCPConnection::CloseConnection()
		{
			m_spTCPClient->GetSocket().close();
		}

		void TCPConnection::OnBytesReceived(uint8_t* aBuffer, size_t u64BytesReceived)
		{
			if (m_u32ReceiveBufferPosition + u64BytesReceived > m_vReceiveBuffer.size())
			{
				// The 8KB buffer is not sufficient to store the data.
				// Reallocate with twice its size.
				m_vReceiveBuffer.resize(m_vReceiveBuffer.size() * 2);
			}

			memcpy(&m_vReceiveBuffer[m_u32ReceiveBufferPosition], aBuffer, u64BytesReceived);
			m_u32ReceiveBufferPosition += u64BytesReceived;

			while (true)
			{
				if (m_u32ReceiveBufferPosition < 2) break; // Partial size received.

				uint16_t u16PacketSize = *((uint16_t*)&m_vReceiveBuffer[0]);
				int32_t u32Delta = m_u32ReceiveBufferPosition - u16PacketSize;

				if (u32Delta < 0) break; // Fragmented packet, the rest will be received later.

				// Create Packet.
				Packet oPacket;

				oPacket.SetPosition(0);
				oPacket.WriteBytes(&m_vReceiveBuffer[2], u16PacketSize - 2);

				OnPacketReceived(oPacket);

				if (u32Delta > 0)
				{
					// TODO rework this to reduce calls to memmove when multiple packets are in the buffer.
					memmove(&m_vReceiveBuffer[u16PacketSize + 2], &m_vReceiveBuffer[0], u32Delta);
					m_u32ReceiveBufferPosition = u32Delta;
				}
				else
				{
					// End of data.
					m_u32ReceiveBufferPosition = 0;
					break;
				}
			}
		}
	}
}