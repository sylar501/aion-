#ifndef AION_WORLDSERVER_NETWORK_WORLDPACKETPROCESSOR_H
#define AION_WORLDSERVER_NETWORK_WORLDPACKETPROCESSOR_H

#include <shared/network/PacketProcessor.h>
#include "WorldConnection.h"

namespace worldserver
{
	namespace network
	{
		class WorldPacketProcessor : public shared::network::PacketProcessor
		{
		public:
			WorldPacketProcessor();

			virtual void	ProcessPacket(shared::network::Packet* pPacket) override;
		private:
			void			SendPacket_AuthenticationResponse(std::shared_ptr<WorldConnection> pConnection, bool bResponse, const std::string& strAccountName);
		};

		extern WorldPacketProcessor sWorldPacketProcessor;
	}
}

#endif