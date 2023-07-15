#include <shared/database/PSQLConnectionPool.h>

namespace shared
{
	namespace database
	{
		void PSQLConnectionPool::Initialize(uint8_t u8NbConnections, const std::string& strConnectionString)
		{
			std::scoped_lock<std::mutex> oLock(m_mtxConnections);

			for (uint8_t i = 0; i < u8NbConnections; i++)
			{
				m_qConnections.push_back(std::make_shared<pqxx::connection>(pqxx::zview(strConnectionString.c_str())));
			}
		}

		void PSQLConnectionPool::Shutdown()
		{
			std::scoped_lock<std::mutex> oLock(m_mtxConnections);

			m_qConnections.clear(); // Connection objects will be deleted once already obtained shared pointers are destroyed.
		}

		pq_con_ptr PSQLConnectionPool::GetConnection()
		{
			std::unique_lock<std::mutex> oLock(m_mtxConnections);

			while (m_qConnections.empty())
			{
				m_oWaitCondition.wait(oLock);
			}

			pq_con_ptr spConnection = m_qConnections.front();
			m_qConnections.pop_front();

			return spConnection;
		}

		void PSQLConnectionPool::FreeConnection(pq_con_ptr spConnection)
		{
			std::unique_lock<std::mutex> oLock(m_mtxConnections);

			m_qConnections.push_back(spConnection);

			oLock.unlock();

			m_oWaitCondition.notify_one();
		}
	}
}