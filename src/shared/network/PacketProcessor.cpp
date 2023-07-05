#include <shared/network/PacketProcessor.h>

namespace shared
{
	namespace network
	{
		PacketProcessor::PacketProcessor(uint8_t u8NbThreads)
		: m_u8NbThreads(u8NbThreads)
		{

		}

		void PacketProcessor::Start()
		{
			m_bContinue = true;

			for (uint8_t i = 0; i < m_u8NbThreads; i++)
			{
				std::thread* pThread = new std::thread([this]()
				{
					this->ThreadRoutine();
				});

				m_vThreads.push_back(pThread);
			}
		}

		void PacketProcessor::Stop()
		{
			for (std::thread* pThread : m_vThreads)
			{
				pThread->join();
				delete pThread;
			}

			m_vThreads.clear();
		}

		void PacketProcessor::EnqueuePacket(Packet* pPacket)
		{
			m_mtxPackets.lock();
			{
				m_qPackets.push_back(pPacket);
			}
			m_mtxPackets.unlock();
		}

		void PacketProcessor::ThreadRoutine()
		{
			while (m_bContinue)
			{
				m_mtxPackets.lock();
				{
					if (m_qPackets.empty())
					{
						m_mtxPackets.unlock();
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
						continue;
					}

					Packet* pPacket = m_qPackets.front();
					m_qPackets.pop_front();

					this->ProcessPacket(pPacket);

					delete pPacket;
				}
				m_mtxPackets.unlock();
			}
		}
	}
}