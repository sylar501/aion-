#ifndef AION_LOGINSERVER_DATABASE_ACCOUNTS_DAO_H
#define AION_LOGINSERVER_DATABASE_ACCOUNTS_DAO_H

#include <string>

namespace loginserver
{
	namespace database
	{
		class AccountsDAO
		{
		public:
			static uint32_t Authenticate(const std::string& strAccountName, const std::string& strPasswordHash);
		};
	}
}

#endif