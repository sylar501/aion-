#include "TicketsDAO.h"
#include "LoginDatabases.h"

#include <shared/utilities/Logger.h>

namespace loginserver
{
	namespace database
	{
		bool TicketsDAO::SetTickets(uint32_t u32AccountId, uint32_t u32LoginTicket, uint64_t u64WorldTicket, uint8_t u8WorldId)
		{
			bool bResult = false;

			try
			{
				shared::database::pq_con_ptr spConnection = sLoginDatabases.GetSharedDatabasePool().GetConnection();

				pqxx::work oTransaction(*spConnection.get());

				oTransaction.conn().prepare("", "INSERT INTO tickets (account_id,login_ticket,world_ticket,world_id) VALUES ($1,$2,$3,$4) ON CONFLICT (account_id) DO UPDATE SET login_ticket = excluded.login_ticket, world_ticket = excluded.world_ticket, world_id = excluded.world_id");

				pqxx::result oResult = oTransaction.exec_prepared0("", u32AccountId, u32LoginTicket, u64WorldTicket, (uint16_t)u8WorldId);

				bResult = oResult.affected_rows() == 1;

				if (bResult)
				{
					oTransaction.commit();
				}

				sLoginDatabases.GetSharedDatabasePool().FreeConnection(spConnection);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("TicketsDAO::SetTickets > %s", ex.what());

				bResult = false;
			}

			return bResult;
		}

		bool TicketsDAO::ValidateLoginTicket(uint32_t u32AccountId, uint32_t u32LoginTicket)
		{
			bool bResult = false;

			try
			{
				shared::database::pq_con_ptr spConnection = sLoginDatabases.GetSharedDatabasePool().GetConnection();

				pqxx::work oTransaction(*spConnection.get());

				oTransaction.conn().prepare("", "SELECT COUNT(*) FROM tickets WHERE account_id = $1 AND login_ticket = $2");

				pqxx::row oRow = oTransaction.exec_prepared1("", u32AccountId, u32LoginTicket);

				bResult = oRow.at(0).as<uint32_t>() == 1;

				sLoginDatabases.GetSharedDatabasePool().FreeConnection(spConnection);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("TicketsDAO::ValidateLoginTicket > %s", ex.what());

				bResult = false;
			}

			return bResult;
		}
	}
}