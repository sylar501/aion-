#ifndef AION_LOGINSERVER_NETWORK_LOGINCONNECTION_H
#define AION_LOGINSERVER_NETWORK_LOGINCONNECTION_H

#include <shared/network/TCPConnection.h>

#include "LoginEncryption.h"

class LoginConnection : public shared::TCPConnection
{
public:
	virtual shared::TCPConnection* Create();

	void				OnConnect();
	void				OnDisconnect();

	void				OnBytesReceived(uint8_t* aBuffer, size_t u64BytesReceived);
private:
	LoginEncryption		m_encryption;
};

#endif