#ifndef AION_SHARED_NETWORK_TCPCONNECTION_H
#define AION_SHARED_NETWORK_TCPCONNECTION_H

#include <stdint.h>
#include <memory>

namespace shared
{
	class TCPClient;

	class TCPConnection
	{
	public:
		virtual TCPConnection*		Create() = 0;

		void						SetTCPClient(std::shared_ptr<TCPClient> spTCPClient);

		virtual void				OnConnect() {}
		virtual void				OnDisconnect() {}

		virtual void				OnBytesReceived(uint8_t* /* aBuffer */, size_t /* u64BytesReceived */) {}

	protected:
		std::shared_ptr<TCPClient>	m_spTCPClient;
	};
}

#endif