#include "LoginDatabases.h"

#include "../configuration/LoginServerConfiguration.h"
#include <shared/utilities/Logger.h>

namespace loginserver
{
	namespace database
	{
		LoginDatabases sLoginDatabases;

		bool LoginDatabases::Initialize()
		{
			try
			{
				m_oAccountsDatabasePool.Initialize(2,
					std::string("host=") + configuration::sLoginServerConfiguration.ACCOUNTS_DATABASE_HOST +
					" port=" + configuration::sLoginServerConfiguration.ACCOUNTS_DATABASE_PORT +
					" user=" + configuration::sLoginServerConfiguration.ACCOUNTS_DATABASE_USER +
					" password=" + configuration::sLoginServerConfiguration.ACCOUNTS_DATABASE_PASSWORD +
					" dbname=" + configuration::sLoginServerConfiguration.ACCOUNTS_DATABASE_NAME
				);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("Failed to connect to accounts database: %s", ex.what());
				return false;
			}

			return true;
		}

		void LoginDatabases::Shutdown()
		{
			m_oAccountsDatabasePool.Shutdown();
		}

		shared::database::pq_con_ptr LoginDatabases::GetAccountsDatabaseConnection()
		{
			return m_oAccountsDatabasePool.GetConnection();
		}
	}
}