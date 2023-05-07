#include <shared/network/TCPClient.h>
#include <shared/network/ASIOThreadPool.h>

namespace shared
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
		if (m_pConnection)
		{
			delete m_pConnection;
		}
	}

	void TCPClient::BeginRead()
	{
		auto self(shared_from_this());

		m_oSocket.async_receive(m_oReceiveBuffer, asio::bind_executor(m_oReceiveStrand, [this, self](std::error_code ec, std::size_t u64BytesReceived)
		{
			if (ec)
			{
				// Disconnected by peer.
				m_pConnection->OnDisconnect();
			}
			else
			{
				// Forward to Connection Object.
				m_pConnection->OnBytesReceived(m_aReceiveBuffer, u64BytesReceived);

				// Continue Read.
				BeginRead();
			}
		}));
	}

	asio::ip::tcp::socket& TCPClient::GetSocket()
	{
		return m_oSocket;
	}

	void TCPClient::Start(TCPConnection* pConnection)
	{
		m_pConnection = pConnection;

		m_pConnection->SetTCPClient(shared_from_this());

		pConnection->OnConnect();

		BeginRead();
	}

	asio::io_context::strand& TCPClient::GetSendStrand()
	{
		return m_oSendStrand;
	}
}