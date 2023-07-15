#ifndef AION_LOGINSERVER_CONFIGURATION_LOGINSERVERCONFIGURATION_H
#define AION_LOGINSERVER_CONFIGURATION_LOGINSERVERCONFIGURATION_H

#include <string>

namespace loginserver
{
	namespace configuration
	{
		class LoginServerConfiguration
		{
		public:
			bool			Load();
		public:
			// Accounts Database.
			std::string		ACCOUNTS_DATABASE_HOST;
			std::string		ACCOUNTS_DATABASE_PORT;
			std::string		ACCOUNTS_DATABASE_USER;
			std::string		ACCOUNTS_DATABASE_PASSWORD;
			std::string		ACCOUNTS_DATABASE_NAME;
		};

		extern LoginServerConfiguration sLoginServerConfiguration;
	}
}

#endif