#ifndef AION_WORLDSERVER_NETWORK_WORLDCONNECTION_H
#define AION_WORLDSERVER_NETWORK_WORLDCONNECTION_H

#include <shared/network/TCPConnection.h>

namespace worldserver
{
	namespace network
	{
		class WorldConnection : public shared::network::TCPConnection
		{
		public:
			enum class ConnectionState
			{
				Unknown,		// Connection object created but connection not yet handled.
				Connected,		// Client just connected.
			};

		public:
			virtual shared::network::TCPConnection* Create();

			WorldConnection();

			void				OnConnect();
			void				OnDisconnect();

			void				OnPacketReceived(shared::network::Packet* pPacket);
			void				SendPacket(shared::network::Packet* pPacket, bool bCloseAfterSend = false);

			ConnectionState		GetConnectionState() { return m_eConnectionState; }
			void				SetConnectionState(ConnectionState eConnectionState) { m_eConnectionState = eConnectionState; }

			uint32_t			GetSessionId() { return m_u32SessionId; }

		private:
			uint32_t			m_u32SessionId = 0;
			ConnectionState		m_eConnectionState = ConnectionState::Unknown;
		};
	}
}

#endif