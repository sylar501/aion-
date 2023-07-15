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
				LoginOK			// Client logged in with username / password.
			};

		public:
			virtual shared::network::TCPConnection* Create();

			LoginConnection();

			void					OnConnect();
			void					OnDisconnect();

			void					OnPacketReceived(shared::network::Packet* pPacket);
			void					SendPacket(shared::network::Packet* pPacket, bool bCloseAfterSend = false);

			ConnectionState			GetConnectionState() { return m_eConnectionState; }
			void					SetConnectionState(ConnectionState eConnectionState) { m_eConnectionState = eConnectionState; }

			uint32_t				GetSessionId() { return m_u32SessionId; }
			crypto::RSAKeyPair&		GetRSAKeyPair() { return m_oRSAKeyPair; }

		private:
			crypto::LoginEncryption	m_oEncryption;
			crypto::RSAKeyPair		m_oRSAKeyPair;
			uint32_t				m_u32SessionId = 0;
			ConnectionState			m_eConnectionState = ConnectionState::Unknown;
		};
	}
}

#endif