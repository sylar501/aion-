#include "WorldDatabases.h"

#include "../configuration/WorldServerConfiguration.h"
#include <shared/utilities/Logger.h>

namespace worldserver
{
	namespace database
	{
		WorldDatabases sWorldDatabases;

		bool WorldDatabases::Initialize()
		{
			try
			{
				m_oSharedDatabasePool.Initialize(2,
					std::string("host=") + configuration::sWorldServerConfiguration.SHARED_DATABASE_HOST +
					" port=" + configuration::sWorldServerConfiguration.SHARED_DATABASE_PORT +
					" user=" + configuration::sWorldServerConfiguration.SHARED_DATABASE_USER +
					" password=" + configuration::sWorldServerConfiguration.SHARED_DATABASE_PASSWORD +
					" dbname=" + configuration::sWorldServerConfiguration.SHARED_DATABASE_NAME
				);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("Failed to connect to shared database: %s", ex.what());
				return false;
			}

			return true;
		}

		void WorldDatabases::Shutdown()
		{
			m_oSharedDatabasePool.Shutdown();
		}

		shared::database::PSQLConnectionPool& WorldDatabases::GetSharedDatabasePool()
		{
			return m_oSharedDatabasePool;
		}
	}
}