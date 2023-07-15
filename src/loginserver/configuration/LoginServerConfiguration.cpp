#include "LoginServerConfiguration.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <shared/utilities/Logger.h>

namespace loginserver
{
	namespace configuration
	{
		LoginServerConfiguration sLoginServerConfiguration;

		bool LoginServerConfiguration::Load()
		{
			std::string strFilePath = "c:\\loginserver.cfg";

			std::ifstream oFileStream(strFilePath);

			if (oFileStream.good())
			{
				nlohmann::json oJSON = nlohmann::json::parse(oFileStream);

				try
				{
					ACCOUNTS_DATABASE_HOST = oJSON["database"]["accounts"]["host"];
					ACCOUNTS_DATABASE_PORT = oJSON["database"]["accounts"]["port"];
					ACCOUNTS_DATABASE_USER = oJSON["database"]["accounts"]["user"];
					ACCOUNTS_DATABASE_PASSWORD = oJSON["database"]["accounts"]["password"];
					ACCOUNTS_DATABASE_NAME = oJSON["database"]["accounts"]["name"];
				}
				catch (std::exception& ex)
				{
					sLogger.Error("Failed to load configuration: %s", ex.what());
					return false;
				}
			}
		}
	}
}