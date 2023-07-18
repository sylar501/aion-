#ifndef AION_LOGINSERVER_WORLDSERVERS_DAO_H
#define AION_LOGINSERVER_WORLDSERVERS_DAO_H

#include "../models/WorldServer.h"

#include <vector>

namespace loginserver
{
	namespace database
	{
		class WorldServersDAO
		{
		public:
			static std::vector<models::WorldServer> GetWorldServers();
			static bool								IsWorldServerAvailable(uint8_t u8ServerId);
		};
	}
}

#endif