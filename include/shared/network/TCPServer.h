#ifndef AION_SHARED_NETWORK_TCPSERVER_H
#define AION_SHARED_NETWORK_TCPSERVER_H

#include <shared/network/TCPClient.h>
#include <shared/network/TCPConnection.h>
#include <shared/network/ASIOThreadPool.h>

namespace shared
{
	namespace network
	{
		typedef std::vector<std::shared_ptr<TCPConnection>> ClientList;

		class TCPServer
		{
		public:
			TCPServer(const std::string& strListenAddress, uint16_t u16ListenPort);

			void						Start();
			void						Stop();

			void						SetConnectionPrototype(TCPConnection* pConnectionPrototype);
			asio::ip::tcp::socket&		GetSocket();

			void						RemoveClientConnection(std::shared_ptr<TCPConnection> spConnection);

		private:
			void						BeginAccept();

		private:
			std::string					m_strListenAddress;
			uint16_t					m_u16ListenPort = 0;

			asio::ip::tcp::acceptor		m_oAcceptor;
			asio::ip::tcp::socket		m_oClientSocket;

			bool						m_bStarted = false;

			TCPConnection*				m_pConnectionPrototype = nullptr;

			ClientList					m_vClientConnections;
			std::mutex					m_mtxClientConnections;
		};
	}
}

#endif