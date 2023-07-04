#ifndef AION_LOGINSERVER_NETWORK_PACKETPROCESSOR_H
#define AION_LOGINSERVER_NETWORK_PACKETPROCESSOR_H

#include <shared/network/Packet.h>

class LoginConnection;

class PacketProcessor
{
public:
	PacketProcessor(LoginConnection& rConnection);

	void				OnPacketReceived(shared::network::Packet& rPacket);

	void				SendPacket_LoginFailed(uint8_t u8Reason);

private:
	LoginConnection&	m_rConnection;
};

#endif