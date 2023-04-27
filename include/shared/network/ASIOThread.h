#ifndef AION_SHARED_NETWORK_ASIOTHREAD_H
#define AION_SHARED_NETWORK_ASIOTHREAD_H

#include <asio.hpp>
#include <stdint.h>
#include <thread>

namespace shared
{
	class ASIOThread
	{
	public:
		ASIOThread(uint8_t u8ThreadId, asio::io_context& rIOContext);

		void				Start();
		void				Stop();

	private:
		void				ThreadRoutine();

	private:
		uint8_t				m_u8ThreadId = 0;
		asio::io_context&	m_rIOContext;
		bool				m_bContinue = false;
		std::thread*		m_pThread = nullptr;
	};
}

#endif