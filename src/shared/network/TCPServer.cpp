#include <shared/network/TCPServer.h>
#include <shared/utilities/Logger.h>

namespace shared
{
	namespace network
	{
		TCPServer::TCPServer(const std::string& strListenAddress, uint16_t u16ListenPort)
			: m_strListenAddress(strListenAddress)
			, m_u16ListenPort(u16ListenPort)
			, m_oAcceptor(ASIOThreadPool::GetInstance()->GetIOContext(), asio::ip::tcp::endpoint(asio::ip::address_v4::from_string(strListenAddress), u16ListenPort))
			, m_oClientSocket(ASIOThreadPool::GetInstance()->GetIOContext())
		{

		}

		void TCPServer::Start()
		{
			if (!m_bStarted)
			{
				m_bStarted = true;

				BeginAccept();
			}
		}

		void TCPServer::Stop()
		{
			if (m_bStarted)
			{
				m_oAcceptor.close();

				m_bStarted = false;
			}
		}

		void TCPServer::SetConnectionPrototype(TCPConnection* pConnectionPrototype)
		{
			m_pConnectionPrototype = pConnectionPrototype;
		}

		asio::ip::tcp::socket& TCPServer::GetSocket()
		{
			return m_oClientSocket;
		}

		void TCPServer::RemoveClientConnection(TCPConnection* pConnection)
		{
			m_mtxClientConnections.lock();
			{
				auto it = std::find(m_vClientConnections.begin(), m_vClientConnections.end(), pConnection);

				if (it != m_vClientConnections.end())
				{
					m_vClientConnections.erase(it);
				}
			}
			m_mtxClientConnections.unlock();
		}

		void TCPServer::BeginAccept()
		{
			m_oAcceptor.async_accept(m_oClientSocket, [this](std::error_code ec)
			{
				if (ec)
				{
					sLogger.Error("TCPServer(0x%x)::async_accept > %s", this, ec.message().c_str());
				}
				else
				{
					// Create TCPClient.
					std::shared_ptr<TCPClient> spNewClient = std::make_shared<TCPClient>(std::move(m_oClientSocket));

					// Create TCPConnection.
					TCPConnection* pNewConnection = m_pConnectionPrototype->Create();

					// Store Connection.
					m_mtxClientConnections.lock();
					{
						m_vClientConnections.push_back(pNewConnection);
					}
					m_mtxClientConnections.unlock();

					// Start TCPClient work.
					spNewClient->SetServer(this);
					spNewClient->Start(pNewConnection);

					BeginAccept();
				}
			});
		}
	}
}