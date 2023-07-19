#ifndef AION_WORLDSERVER_CONFIGURATION_LOGINSERVERCONFIGURATION_H
#define AION_WORLDSERVER_CONFIGURATION_LOGINSERVERCONFIGURATION_H

#include <string>

namespace worldserver
{
	namespace configuration
	{
		class WorldServerConfiguration
		{
		public:
			bool			Load();
		public:
			uint16_t		WORLDSERVER_ID = 0;

			// Shared Database.
			std::string		SHARED_DATABASE_HOST;
			std::string		SHARED_DATABASE_PORT;
			std::string		SHARED_DATABASE_USER;
			std::string		SHARED_DATABASE_PASSWORD;
			std::string		SHARED_DATABASE_NAME;

			// Characters
			uint16_t		CHARACTERS_MAX_COUNT = 0;
		};

		extern WorldServerConfiguration sWorldServerConfiguration;
	}
}

#endif