#ifndef AION_SHARED_NETWORK_TCPCONNECTION_H
#define AION_SHARED_NETWORK_TCPCONNECTION_H

#include <stdint.h>
#include <memory>

#include <shared/network/Packet.h>

namespace shared
{
	namespace network
	{
		class TCPClient;

		class TCPConnection
		{
		public:
			virtual TCPConnection* Create() = 0;

			void						SetTCPClient(std::shared_ptr<TCPClient> spTCPClient);

			virtual void				OnConnect();
			virtual void				OnDisconnect();

			void						CloseConnection();

			bool						IsOpen();
			void						SetOpen(bool bOpen);

			void						OnBytesReceived(uint8_t* aBuffer, size_t u64BytesReceived);

			virtual void				OnPacketReceived(Packet* pPacket) = 0;

		protected:
			std::shared_ptr<TCPClient>	m_spTCPClient;
			bool						m_bIsOpen = false;
			std::vector<uint8_t>		m_vReceiveBuffer;
			uint32_t					m_u32ReceiveBufferPosition = 0;
		};
	}
}

#endif