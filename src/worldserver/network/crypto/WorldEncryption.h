#ifndef AION_WORLDSERVER_NETWORK_CRYPTO_WORLDENCRYPTION_H
#define AION_WORLDSERVER_NETWORK_CRYPTO_WORLDENCRYPTION_H

#include <stdint.h>

namespace worldserver
{
	namespace network
	{
		namespace crypto
		{
			class WorldEncryption
			{
			public:
				WorldEncryption();

				uint32_t	GetBaseKey();

				bool		Decrypt(uint8_t* pBuffer, uint32_t u32Length);
				void		Encrypt(uint8_t* pBuffer, uint32_t u32Length);
			private:
				bool		m_bEnabled = false;
				uint32_t	m_u32BaseKey = 0;
				uint8_t		m_aClientKey[8] = { 0 };
				uint8_t		m_aServerKey[8] = { 0 };
			};
		}
	}
}

#endif