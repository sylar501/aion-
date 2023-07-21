#ifndef AION_WORLDSERVER_NETWORK_WORLDCONNECTION_H
#define AION_WORLDSERVER_NETWORK_WORLDCONNECTION_H

#include <shared/network/TCPConnection.h>
#include "crypto/WorldEncryption.h"
#include "WorldPacket.h"

namespace worldserver
{
	namespace network
	{
		class WorldConnection : public shared::network::TCPConnection
		{
			friend class WorldPacketProcessor;

		public:
			enum class ConnectionState
			{
				Unknown,		// Connection object created but connection not yet handled.
				Connected,		// Client just connected.
				Authenticated,	// Client is authenticated to the World Server.
				Playing			// Client has entered world and is playing.
			};

		public:
			virtual shared::network::TCPConnection* Create();

			WorldConnection();

			void					OnConnect();
			void					OnDisconnect();

			void					OnPacketReceived(shared::network::Packet* pPacket);
			void					SendPacket(WorldPacket* pPacket, bool bCloseAfterSend = false);

			ConnectionState			GetConnectionState() { return m_eConnectionState; }
			void					SetConnectionState(ConnectionState eConnectionState) { m_eConnectionState = eConnectionState; }

			uint32_t				GetSessionId() { return m_u32SessionId; }

			uint32_t				GetAccountId();
			void					SetAccountId(uint32_t u32AccountId);

			void					SendPacket_Quit(bool bEditMode = false, bool bCloseAfterSend = false);

		private:
			void					writePlayerInfomation(WorldPacket* pPacket);

		private:
			uint32_t				m_u32SessionId = 0;
			ConnectionState			m_eConnectionState = ConnectionState::Unknown;
			crypto::WorldEncryption	m_oEncryption;
			uint32_t				m_u32AccountId = 0;
		};
	}
}

#endif