#ifndef AION_SHARED_NETWORK_TCPCLIENT_H
#define AION_SHARED_NETWORK_TCPCLIENT_H

#include <asio.hpp>
#include <stdint.h>
#include <memory>

#include <shared/network/TCPConnection.h>

#define RECEIVE_BUFFER_SIZE 4096

namespace shared
{
	class TCPClient : public std::enable_shared_from_this<TCPClient>
	{
	public:
		TCPClient(asio::ip::tcp::socket oSocket);

		asio::ip::tcp::socket&		GetSocket();
		void						Start(TCPConnection* pConnection);

	private:
		void						BeginRead();

	private:
		asio::ip::tcp::socket		m_oSocket;
		uint8_t						m_aReceiveBuffer[RECEIVE_BUFFER_SIZE] = { 0 };
		asio::mutable_buffer		m_oReceiveBuffer;

		TCPConnection*				m_pConnection;
	};
}

#endif