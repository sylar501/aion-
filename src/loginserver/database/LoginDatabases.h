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

			shared::database::PSQLConnectionPool&	GetAccountsDatabasePool();
			shared::database::PSQLConnectionPool&	GetSharedDatabasePool();
		private:
			shared::database::PSQLConnectionPool	m_oAccountsDatabasePool;
			shared::database::PSQLConnectionPool	m_oSharedDatabasePool;
		};

		extern LoginDatabases sLoginDatabases;
	}
}

#endif