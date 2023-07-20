#ifndef AION_LOGINSERVER_DATABASE_TICKETSDAO_H
#define AION_LOGINSERVER_DATABASE_TICKETSDAO_H

#include <stdint.h>

namespace loginserver
{
	namespace database
	{
		class TicketsDAO
		{
		public:
			static bool SetTickets(uint32_t u32AccountId, uint32_t u32LoginTicket, uint64_t u64WorldTicket, uint8_t u8WorldId);
		};
	}
}

#endif