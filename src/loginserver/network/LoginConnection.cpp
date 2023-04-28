#include "LoginConnection.h"

#include <shared/network/Buffer.h>
#include <shared/network/TCPClient.h>

shared::TCPConnection* LoginConnection::Create()
{
	return new LoginConnection();
}

void LoginConnection::OnConnect()
{
	printf("New Login Connection\n");

	// Make Init Packet.
	Buffer initPacket;
	initPacket.WriteInt16(0); // Size
	initPacket.WriteInt8(0); // Opcode Init
	initPacket.WriteUInt32(0x32c4e822); // sessionId
	initPacket.WriteUInt32(0x0000c621); // protocol revision
	initPacket.Append(m_oRSAKeyPair.GetEncryptedModulus(), 128);
	
	for (int8_t i = 0; i < 4; i++)
		initPacket.WriteUInt32(0);

	initPacket.Append(m_oEncryption.GetBlowfishKey(), 16);
	initPacket.WriteUInt32(197635);
	initPacket.WriteUInt32(2097152);

	size_t u64Len = initPacket.GetWritePosition();

	for (int8_t i = 0; i < 4; i++)
		initPacket.WriteUInt32(0);

	printf("Packet Size before encryption: %d\n", initPacket.GetWritePosition());

	int size = m_oEncryption.Encrypt(initPacket.GetBuffer() + 2, u64Len - 4) + 2;

	initPacket.SetWritePosition(0);
	initPacket.WriteUInt16((uint16_t)size);

	printf("Packet Size after encryption: %d\n", size);

	initPacket.HexDump();

	m_spTCPClient->GetSocket().async_send(asio::buffer(initPacket.GetBuffer(), size), [](std::error_code ec, size_t bytesTransferred) {
		printf("sent %llu bytes\n", bytesTransferred);
		});
}

void LoginConnection::OnDisconnect()
{
	printf("Disconnected\n");
}

void LoginConnection::OnBytesReceived(uint8_t* aBuffer, size_t u64BytesReceived)
{
	printf("Received %llu bytes\n", u64BytesReceived);
}
