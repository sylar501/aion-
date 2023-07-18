#ifndef AION_LOGINSERVER_MODELS_WORLDSERVER_H
#define AION_LOGINSERVER_MODELS_WORLDSERVER_H

#include <string>

namespace loginserver
{
	namespace models
	{
		struct WorldServer
		{
			uint8_t		u8ID = 0;
			std::string strAddress;
			uint16_t	u16Port = 0;
			uint32_t	u32MaxPlayers = 0;
			uint32_t	u32CurrentPlayers = 0;
			bool		bOnline = false;
		};
	}
}

#endif