#include <shared/network/ASIOThread.h>

namespace shared
{
	ASIOThread::ASIOThread(uint8_t u8ThreadId, asio::io_context& rIOContext)
		: m_u8ThreadId(u8ThreadId)
		, m_rIOContext(rIOContext)
	{

	}

	void ASIOThread::Start()
	{
		if (!m_pThread)
		{
			m_pThread = new std::thread([this]()
			{
				ThreadRoutine();
			});
		}
	}

	void ASIOThread::Stop()
	{
		if (m_pThread)
		{
			m_bContinue = false;

			m_pThread->join();

			delete m_pThread;
			m_pThread = nullptr;
		}
	}

	void ASIOThread::ThreadRoutine()
	{
		m_bContinue = true;

		while (m_bContinue)
		{
			m_rIOContext.run();
		}
	}
}