#ifndef AION_SHARED_DATABASE_PSQLCONNECTIONPOOL_H
#define AION_SHARED_DATABASE_PSQLCONNECTIONPOOL_H

#include <pqxx/pqxx>
#include <memory>
#include <unordered_map>
#include <deque>
#include <mutex>

namespace shared
{
	namespace database
	{
		typedef std::shared_ptr<pqxx::connection> pq_con_ptr;

		class PSQLConnectionPool
		{
		public:
			void							Initialize(uint8_t u8NbConnections, const std::string& strConnectionString);
			void							Shutdown();
			pq_con_ptr						GetConnection();
			void							FreeConnection(pq_con_ptr spConnection);
		private:
			std::deque<pq_con_ptr>			m_qConnections;
			std::mutex						m_mtxConnections;
			std::condition_variable			m_oWaitCondition;
		};
	}
}

#endif