#include <shared/network/TCPConnection.h>
#include <shared/network/TCPClient.h>

namespace shared
{
	void TCPConnection::SetTCPClient(std::shared_ptr<TCPClient> spTCPClient)
	{
		m_spTCPClient = spTCPClient;
	}

	void TCPConnection::OnConnect()
	{
		m_oReceiveBuffer.SetWritePosition(0);
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
		m_oReceiveBuffer.Append(aBuffer, (uint32_t) u64BytesReceived);

		while (true)
		{
			uint32_t u32BufferSize = m_oReceiveBuffer.GetSize();

			if (u32BufferSize < 2) break; // Partial size received.

			m_oReceiveBuffer.SetReadPosition(0);

			uint16_t u16Size = m_oReceiveBuffer.ReadUInt16();
			int32_t u32Delta = m_oReceiveBuffer.GetSize() - u16Size;

			if (u32Delta < 0) break; // Fragmented packet, the rest will be received later.

			Buffer oPacket;

			oPacket.SetWritePosition(0);
			oPacket.Append(m_oReceiveBuffer.GetBuffer() + 2, u16Size - 2);

			OnPacketReceived(oPacket);

			if(u32Delta > 0)
			{
				m_oReceiveBuffer.MoveData(u16Size + 2, 0, u32Delta);
				m_oReceiveBuffer.Resize(u32Delta);
				m_oReceiveBuffer.SetWritePosition(u32Delta);
			}
			else
			{
				// End of data.
				m_oReceiveBuffer.Clear();
				break;
			}
		}
	}
}