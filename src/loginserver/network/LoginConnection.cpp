#include "LoginConnection.h"

shared::TCPConnection* LoginConnection::Create()
{
	return new LoginConnection();
}

void LoginConnection::OnConnect()
{
	printf("New Login Connection\n");
}

void LoginConnection::OnDisconnect()
{
	printf("Disconnected\n");
}

void LoginConnection::OnBytesReceived(uint8_t* aBuffer, size_t u64BytesReceived)
{
	printf("Received %llu bytes\n", u64BytesReceived);
}
