#include <shared/network/TCPConnection.h>

namespace shared
{
	void TCPConnection::SetTCPClient(std::shared_ptr<TCPClient> spTCPClient)
	{
		m_spTCPClient = spTCPClient;
	}
}