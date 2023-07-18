#include "AuthKeysDAO.h"
#include "LoginDatabases.h"

#include <shared/utilities/Logger.h>

namespace loginserver
{
	namespace database
	{
		bool AuthKeysDAO::SetAuthKeys(uint32_t u32AccountId, uint32_t u32LoginTicket, uint64_t u64GameAuthKey)
		{
			bool bResult = false;

			try
			{
				shared::database::pq_con_ptr spConnection = sLoginDatabases.GetSharedDatabasePool().GetConnection();

				pqxx::work oTransaction(*spConnection.get());

				oTransaction.conn().prepare("", "INSERT INTO auth_keys (account_id,login_ticket,game_auth_key) VALUES ($1,$2,$3) ON CONFLICT (account_id) DO UPDATE SET login_ticket = excluded.login_ticket, game_auth_key = excluded.game_auth_key");

				pqxx::result oResult = oTransaction.exec_prepared0("", u32AccountId, u32LoginTicket, u64GameAuthKey);

				bResult = oResult.affected_rows() == 1;

				if (bResult)
				{
					oTransaction.commit();
				}

				sLoginDatabases.GetSharedDatabasePool().FreeConnection(spConnection);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("AuthKeysDAO::SetAuthKeys > %s", ex.what());

				bResult = false;
			}

			return bResult;
		}
	}
}