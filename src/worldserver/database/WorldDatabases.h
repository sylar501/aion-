#ifndef AION_WORLDSERVER_DATABASE_LOGINDATABASES_H
#define AION_WORLDSERVER_DATABASE_LOGINDATABASES_H

#include <shared/database/PSQLConnectionPool.h>

namespace worldserver
{
	namespace database
	{
		class WorldDatabases
		{
		public:
			bool Initialize();
			void Shutdown();

			shared::database::PSQLConnectionPool&	GetSharedDatabasePool();
		private:
			shared::database::PSQLConnectionPool	m_oSharedDatabasePool;
		};

		extern WorldDatabases sWorldDatabases;
	}
}

#endif