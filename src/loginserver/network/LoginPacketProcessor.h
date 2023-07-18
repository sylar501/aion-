#ifndef AION_LOGINSERVER_NETWORK_LOGINPACKETPROCESSOR_H
#define AION_LOGINSERVER_NETWORK_LOGINPACKETPROCESSOR_H

#include <shared/network/PacketProcessor.h>
#include "LoginConnection.h"

namespace loginserver
{
	namespace network
	{
		class LoginPacketProcessor : public shared::network::PacketProcessor
		{
		public:
			LoginPacketProcessor();

			virtual void	ProcessPacket(shared::network::Packet* pPacket) override;
		private:
			void			SendPacket_LoginFailed(LoginConnection* pConnection, uint8_t u8Reason);
			void			SendPacket_PlayFailed(LoginConnection* pConnection, uint8_t u8Reason);
		};

		extern LoginPacketProcessor sLoginPacketProcessor;
	}
}

#endif