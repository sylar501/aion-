#include <shared/network/ASIOThreadPool.h>

namespace shared
{
	ASIOThreadPool* ASIOThreadPool::ms_pInstance = nullptr;

	ASIOThreadPool* ASIOThreadPool::GetInstance()
	{
		if (!ms_pInstance)
		{
			ms_pInstance = new ASIOThreadPool();
		}

		return ms_pInstance;
	}

	void ASIOThreadPool::SetThreadCount(uint8_t u8NbThreads)
	{
		m_u8NbThreads = u8NbThreads;
	}

	asio::io_context& ASIOThreadPool::GetIOContext()
	{
		return m_oIOContext;
	}

	void ASIOThreadPool::Start()
	{
		if (!m_bStarted)
		{
			for (uint8_t i = 0; i < m_u8NbThreads; i++)
			{
				ASIOThread* pThread = new ASIOThread(i, m_oIOContext);

				m_vThreads.push_back(pThread);

				pThread->Start();
			}

			m_bStarted = true;
		}
	}

	void ASIOThreadPool::Stop()
	{
		if (m_bStarted)
		{
			m_oIOContext.stop();

			for (ASIOThread* pThread : m_vThreads)
			{
				pThread->Stop();

				delete pThread;
			}

			m_vThreads.clear();
		}
	}
}