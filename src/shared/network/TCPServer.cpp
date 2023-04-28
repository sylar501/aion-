#include <shared/network/TCPServer.h>

namespace shared
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

	void TCPServer::BeginAccept()
	{
		m_oAcceptor.async_accept(m_oClientSocket, [this](std::error_code ec)
		{
			if (ec)
			{
				// LOG ERROR
			}
			else
			{
				std::shared_ptr<TCPClient> spNewClient = std::make_shared<TCPClient>(std::move(m_oClientSocket));

				spNewClient->Start(m_pConnectionPrototype->Create());

				BeginAccept();
			}
		});
	}
}