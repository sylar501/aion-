#ifndef AION_SHARED_NETWORK_ASIOTHREADPOOL_H
#define AION_SHARED_NETWORK_ASIOTHREADPOOL_H

#include <asio.hpp>
#include <stdint.h>
#include <vector>

#include <shared/network/ASIOThread.h>

namespace shared
{
	namespace network
	{
		/*
			This class handles a pool of threads, each processing pending
			handles for the same ASIO IO Context.

			It allows for multi-threading use of the ASIO Library.
		*/
		class ASIOThreadPool
		{
		public:
			static ASIOThreadPool*		GetInstance();

		public:
			void						SetThreadCount(uint8_t u8NbThreads);
			asio::io_context&			GetIOContext();

			void						Start();
			void						Stop();

		private:
			static ASIOThreadPool*		ms_pInstance;

		private:
			uint8_t						m_u8NbThreads;
			std::vector<ASIOThread*>	m_vThreads;
			asio::io_context			m_oIOContext;
			bool						m_bStarted = false;
		};
	}
}

#endif