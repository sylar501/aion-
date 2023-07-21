#ifndef AION_WORLDSERVER_DATABASE_TICKETSDAO_H
#define AION_WORLDSERVER_DATABASE_TICKETSDAO_H

#include <stdint.h>

namespace worldserver
{
	namespace database
	{
		class TicketsDAO
		{
		public:
			static bool ValidateTickets(uint32_t u32AccountId, uint32_t u32LoginTicket, uint64_t u64WorldTicket, uint8_t u8WorldId);
			static bool SetLoginTicket(uint32_t u32AccountId, uint32_t u32LoginTicket);
		};
	}
}

#endif