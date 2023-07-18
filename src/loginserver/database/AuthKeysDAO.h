#ifndef AION_LOGINSERVER_DATABASE_AUTHKEYSDAO_H
#define AION_LOGINSERVER_DATABASE_AUTHKEYSDAO_H

#include <stdint.h>

namespace loginserver
{
	namespace database
	{
		class AuthKeysDAO
		{
		public:
			static bool SetAuthKeys(uint32_t u32AccountId, uint32_t u32LoginTicket, uint64_t u64GameAuthKey);
		};
	}
}

#endif