#include "AccountsDAO.h"
#include "LoginDatabases.h"

#include <shared/utilities/Logger.h>

namespace loginserver
{
	namespace database
	{
		uint32_t AccountsDAO::Authenticate(const std::string& strAccountName, const std::string& strPasswordHash)
		{
			uint32_t u32Result = 0;

			try
			{
				shared::database::pq_con_ptr spConnection = sLoginDatabases.GetAccountsDatabasePool().GetConnection();

				pqxx::work oTransaction(*spConnection.get());

				oTransaction.conn().prepare("", "SELECT account_id FROM accounts WHERE name = $1 AND password = $2");

				pqxx::result oResult = oTransaction.exec_prepared("", strAccountName, strPasswordHash);

				if (oResult.size() > 0)
				{
					u32Result = oResult.at(0).at(0).as<uint32_t>();
				}

				sLoginDatabases.GetAccountsDatabasePool().FreeConnection(spConnection);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("AccountsDAO::Authenticate > %s", ex.what());

				u32Result = 0;
			}

			return u32Result;
		}
	}
}