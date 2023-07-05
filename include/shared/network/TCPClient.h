#ifndef AION_SHARED_NETWORK_TCPCLIENT_H
#define AION_SHARED_NETWORK_TCPCLIENT_H

#include <asio.hpp>
#include <stdint.h>
#include <memory>

#include <shared/network/TCPConnection.h>

#define RECEIVE_BUFFER_SIZE 4096

namespace shared
{
	namespace network
	{
		class TCPServer;

		class TCPClient : public std::enable_shared_from_this<TCPClient>
		{
		public:
			TCPClient(asio::ip::tcp::socket oSocket);
			~TCPClient();

			asio::ip::tcp::socket&		GetSocket();
			TCPConnection*				GetConnection();

			void						Start(TCPConnection* pConnection);

			TCPServer*					GetServer();
			void						SetServer(TCPServer* pServer);

			asio::io_context::strand&	GetSendStrand();

		private:
			void						BeginRead();

		private:
			asio::ip::tcp::socket		m_oSocket;

			uint8_t						m_aReceiveBuffer[RECEIVE_BUFFER_SIZE] = { 0 };
			asio::mutable_buffer		m_oReceiveBuffer;

			asio::io_context::strand	m_oReceiveStrand;
			asio::io_context::strand	m_oSendStrand;

			TCPConnection*				m_pConnection = nullptr;
			TCPServer*					m_pServer = nullptr;
		};
	}
}

#endif