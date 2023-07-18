#include "WorldServersDAO.h"
#include "LoginDatabases.h"

#include <shared/utilities/Logger.h>

namespace loginserver
{
	namespace database
	{
		std::vector<models::WorldServer> WorldServersDAO::GetWorldServers()
		{
			std::vector<models::WorldServer> vResult;

			try
			{
				shared::database::pq_con_ptr spConnection = sLoginDatabases.GetSharedDatabasePool().GetConnection();

				pqxx::work oTransaction(*spConnection.get());

				for (auto [id,address,port,maxPlayers,currentPlayers,online] : oTransaction.query<uint16_t,std::string,uint16_t,uint32_t,uint32_t,bool>
					("SELECT id,address,port,max_players,current_players,online FROM worldservers"))
				{
					models::WorldServer oWorldServer;

					oWorldServer.u8ID = (uint8_t)id;
					oWorldServer.strAddress = address;
					oWorldServer.u16Port = port;
					oWorldServer.u32MaxPlayers = maxPlayers;
					oWorldServer.u32CurrentPlayers = currentPlayers;
					oWorldServer.bOnline = online;

					vResult.push_back(oWorldServer);
				}

				sLoginDatabases.GetSharedDatabasePool().FreeConnection(spConnection);
			}
			catch (std::exception& ex)
			{
				sLogger.Error("WorldServersDAO::GetWorldServers > %s", ex.what());

				vResult.clear();
			}

			return vResult;
		}

		bool WorldServersDAO::IsWorldServerAvailable(uint8_t u8ServerID)
		{
			bool bResult = false;

			try
			{
				shared::database::pq_con_ptr spConnection = sLoginDatabases.GetSharedDatabasePool().GetConnection();

				pqxx::work oTransaction(*spConnection.get());

				oTransaction.conn().prepare("", "SELECT COUNT(*) FROM worldservers WHERE id = $1 AND current_players < max_players");

				pqxx::row oRow = oTransaction.exec_prepared1("", (uint16_t)u8ServerID);

				bResult = oRow.at(0).as<uint32_t>() == 1;

				sLoginDatabases.GetSharedDatabasePool().FreeConnection(spConnection);
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