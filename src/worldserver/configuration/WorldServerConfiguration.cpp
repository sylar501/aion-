#include "WorldServerConfiguration.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <shared/utilities/Logger.h>

namespace worldserver
{
	namespace configuration
	{
		WorldServerConfiguration sWorldServerConfiguration;

		bool WorldServerConfiguration::Load()
		{
			std::string strFilePath = "c:\\worldserver.cfg";

			std::ifstream oFileStream(strFilePath);

			if (oFileStream.good())
			{
				nlohmann::json oJSON = nlohmann::json::parse(oFileStream);

				try
				{
					WORLDSERVER_ID = oJSON["id"];

					SHARED_DATABASE_HOST = oJSON["database"]["shared"]["host"];
					SHARED_DATABASE_PORT = oJSON["database"]["shared"]["port"];
					SHARED_DATABASE_USER = oJSON["database"]["shared"]["user"];
					SHARED_DATABASE_PASSWORD = oJSON["database"]["shared"]["password"];
					SHARED_DATABASE_NAME = oJSON["database"]["shared"]["name"];

					CHARACTERS_MAX_COUNT = oJSON["characters"]["maxCount"];
				}
				catch (std::exception& ex)
				{
					sLogger.Error("Failed to load configuration: %s", ex.what());
					return false;
				}
			}

			return true;
		}
	}
}