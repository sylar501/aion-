#include <shared/network/TCPClient.h>

namespace shared
{
	TCPClient::TCPClient(asio::ip::tcp::socket oSocket)
		: m_oSocket(std::move(oSocket))
		, m_oReceiveBuffer(m_aReceiveBuffer, RECEIVE_BUFFER_SIZE)
	{
		m_oSocket.set_option(asio::ip::tcp::no_delay(true));
	}

	void TCPClient::BeginRead()
	{
		auto self(shared_from_this());

		m_oSocket.async_receive(m_oReceiveBuffer, [this, self](std::error_code ec, std::size_t u64BytesReceived)
		{
			if (ec)
			{
				// LOG ERROR
				printf("Socket Error\n");
			}
			else
			{
				// Forward to Connection Object.
				printf("Received %llu bytes\n", u64BytesReceived);

				m_pConnection->OnBytesReceived(m_aReceiveBuffer, u64BytesReceived);

				BeginRead();
			}
		});
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
}