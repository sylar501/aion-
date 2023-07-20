#include "TicketsDAO.h"
#include "WorldDatabases.h"

#include <shared/utilities/Logger.h>

namespace worldserver
{
	namespace database
	{
		bool TicketsDAO::ValidateTickets(uint32_t u32AccountId, uint32_t u32LoginTicket, uint64_t u64WorldTicket, uint8_t u8WorldId)
		{
			bool bResult = false;

			try
			{
				shared::database::pq_con_ptr spConnection = sWorldDatabases.GetSharedDatabasePool().GetConnection();

				pqxx::work oTransaction(*spConnection.get());

				oTransaction.conn().prepare("", "SELECT COUNT(*) FROM tickets WHERE account_id = $1 AND login_ticket = $2 AND world_ticket = $3 AND world_id = $4");

				pqxx::row oRow = oTransaction.exec_prepared1("", u32AccountId, u32LoginTicket, u64WorldTicket, (uint16_t)u8WorldId);

				bResult = oRow.at(0).as<uint32_t>() == 1;

				sWorldDatabases.GetSharedDatabasePool().FreeConnection(spConnection);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("WorldServersDAO::IsWorldServerAvailable > %s", ex.what());

				bResult = false;
			}

			return bResult;
		}
	}
}