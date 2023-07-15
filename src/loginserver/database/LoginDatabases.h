#ifndef AION_LOGINSERVER_DATABASE_LOGINDATABASES_H
#define AION_LOGINSERVER_DATABASE_LOGINDATABASES_H

#include <shared/database/PSQLConnectionPool.h>

namespace loginserver
{
	namespace database
	{
		class LoginDatabases
		{
		public:
			bool Initialize();
			void Shutdown();

			shared::database::pq_con_ptr			GetAccountsDatabaseConnection();
		private:
			shared::database::PSQLConnectionPool	m_oAccountsDatabasePool;
		};

		extern LoginDatabases sLoginDatabases;
	}
}

#endif