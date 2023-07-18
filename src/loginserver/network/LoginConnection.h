#ifndef AION_LOGINSERVER_NETWORK_LOGINCONNECTION_H
#define AION_LOGINSERVER_NETWORK_LOGINCONNECTION_H

#include <shared/network/TCPConnection.h>

#include "crypto/LoginEncryption.h"
#include "crypto/RSAKeyPair.h"

namespace loginserver
{
	namespace network
	{
		class LoginConnection : public shared::network::TCPConnection
		{
		public:
			enum class ConnectionState
			{
				Unknown,		// Connection object created but connection not yet handled.
				Connected,		// Client just connected.
				GameGuardOK,	// GameGuard authentication passed.
				LoginOK,		// Client logged in with username / password.
				ServerListSent,	// Server list has been sent to client.
				ServerSelected,	// Player has selected a world server to connect to.
			};

		public:
			virtual shared::network::TCPConnection* Create();

			LoginConnection();

			void					OnConnect();
			void					OnDisconnect();

			void					OnPacketReceived(shared::network::Packet* pPacket);
			void					SendPacket(shared::network::Packet* pPacket, bool bCloseAfterSend = false);

			ConnectionState			GetConnectionState();
			void					SetConnectionState(ConnectionState eConnectionState);

			uint32_t				GetSessionId();
			crypto::RSAKeyPair&		GetRSAKeyPair();

			uint32_t				GetAccountId();
			void					SetAccountId(uint32_t u32AccountId);

			uint32_t				GetLoginTicket();
			void					SetLoginTicket(uint32_t u32LoginTicket);

		private:
			ConnectionState			m_eConnectionState = ConnectionState::Unknown;

			crypto::LoginEncryption	m_oEncryption;
			crypto::RSAKeyPair		m_oRSAKeyPair;
			
			uint32_t				m_u32SessionId = 0;
			uint32_t				m_u32AccountId = 0;
			uint32_t				m_u32LoginTicket = 0;
		};
	}
}

#endif