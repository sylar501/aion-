#include <shared/network/TCPClient.h>
#include <shared/network/TCPServer.h>
#include <shared/network/ASIOThreadPool.h>
#include <shared/utilities/Logger.h>

namespace shared
{
	namespace network
	{
		TCPClient::TCPClient(asio::ip::tcp::socket oSocket)
			: m_oSocket(std::move(oSocket))
			, m_oReceiveBuffer(m_aReceiveBuffer, RECEIVE_BUFFER_SIZE)
			, m_oReceiveStrand(ASIOThreadPool::GetInstance()->GetIOContext())
			, m_oSendStrand(ASIOThreadPool::GetInstance()->GetIOContext())
		{
			m_oSocket.set_option(asio::ip::tcp::no_delay(true));
		}

		TCPClient::~TCPClient()
		{

		}

		void TCPClient::BeginRead()
		{
			auto self(shared_from_this());

			m_oSocket.async_receive(m_oReceiveBuffer, asio::bind_executor(m_oReceiveStrand, [this, self](std::error_code ec, std::size_t u64BytesReceived)
			{
				if (ec)
				{
					// Disconnected by peer.
					m_spConnection->SetOpen(false);
					m_spConnection->OnDisconnect();
				}
				else
				{
					// Forward to Connection Object.
					m_spConnection->OnBytesReceived(m_aReceiveBuffer, u64BytesReceived);

					// Continue Read.
					BeginRead();
				}
			}));
		}

		asio::ip::tcp::socket& TCPClient::GetSocket()
		{
			return m_oSocket;
		}

		std::shared_ptr<TCPConnection> TCPClient::GetConnection()
		{
			return m_spConnection;
		}

		void TCPClient::Start(std::shared_ptr<TCPConnection> spConnection)
		{
			m_spConnection = spConnection;

			m_spConnection->SetTCPClient(shared_from_this());

			spConnection->OnConnect();

			BeginRead();
		}

		TCPServer* TCPClient::GetServer()
		{
			return m_pServer;
		}

		void TCPClient::SetServer(TCPServer* pServer)
		{
			m_pServer = pServer;
		}

		asio::io_context::strand& TCPClient::GetSendStrand()
		{
			return m_oSendStrand;
		}
	}
}