#ifndef AION_SHARED_NETWORK_PACKETPROCESSOR_H
#define AION_SHARED_NETWORK_PACKETPROCESSOR_H

#include <shared/network/Packet.h>
#include <deque>
#include <thread>
#include <mutex>

namespace shared
{
	namespace network
	{
		class PacketProcessor
		{
		public:
			PacketProcessor(uint8_t u8NbThreads);

			void						Start();
			void						Stop();

			void						EnqueuePacket(Packet* pPacket);

			virtual void				ProcessPacket(Packet* pPacket) = 0;
		private:
			void						ThreadRoutine();
		private:
			std::deque<Packet*>			m_qPackets; // Packets queued for processing.
			std::mutex					m_mtxPackets;

			bool						m_bContinue = false;

			std::vector<std::thread*>	m_vThreads;
			uint8_t						m_u8NbThreads = 1;
		};
	}
}

#endif