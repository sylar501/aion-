#include "WorldEncryption.h"

#include <openssl/rand.h>
#include <string.h>

#include <shared/utilities/Logger.h>
#include <shared/network/Packet.h>

#define WORLD_PACKET_CLIENT_STATIC_CODE		0x57
#define WORLD_PACKET_STATIC_CRYPT_KEY		"nKO/WctQ0AVLbpzfBkS6NevDYT8ourG5CRlmdjyJ72aswx4EPq1UgZhFMXH?3iI9"

namespace worldserver
{
	namespace network
	{
		namespace crypto
		{
			WorldEncryption::WorldEncryption()
			{
				RAND_bytes((uint8_t*)&m_u32BaseKey, 4);

				memcpy(m_aServerKey, &m_u32BaseKey, 4);
				m_aServerKey[4] = 0xa1;
				m_aServerKey[5] = 0x6c;
				m_aServerKey[6] = 0x54;
				m_aServerKey[7] = 0x87;

				memcpy(m_aClientKey, m_aServerKey, 8);
			}

			uint32_t WorldEncryption::GetBaseKey()
			{
				return m_u32BaseKey;
			}

			bool WorldEncryption::Decrypt(uint8_t* pBuffer, uint32_t u32Length)
			{
				if (!m_bEnabled) return true;

				uint32_t u32Index = 0;
				uint8_t u8Previous = pBuffer[u32Index];
				
				pBuffer[u32Index++] ^= m_aClientKey[0];

				for (uint32_t i = 1; i < u32Length; i++, u32Index++)
				{
					uint8_t u8Current = pBuffer[u32Index];
					pBuffer[u32Index] ^= WORLD_PACKET_STATIC_CRYPT_KEY[i & 63] ^ m_aClientKey[i & 7] ^ u8Previous;
					u8Previous = u8Current;
				}

				// Validate that the client packet is correct.
				if ((uint16_t)(*((uint16_t*)&pBuffer[0])) != (uint16_t)(~(*((uint16_t*)&pBuffer[3]))))
					return false;
					
				if (pBuffer[2] != WORLD_PACKET_CLIENT_STATIC_CODE)
					return false;

				uint64_t u64CurrentKey = *((uint64_t*)m_aClientKey);

				u64CurrentKey += u32Length;

				memcpy(m_aClientKey, &u64CurrentKey, 8);

				return true;
			}

			void WorldEncryption::Encrypt(uint8_t* pBuffer, uint32_t u32Length)
			{
				if (!m_bEnabled)
				{
					m_bEnabled = true;
					return;
				}

				uint32_t u32Index = 0;

				pBuffer[u32Index] ^= m_aServerKey[0];

				uint8_t u8Previous = pBuffer[u32Index++];

				for (uint32_t i = 1; i < u32Length; i++, u32Index++)
				{
					pBuffer[u32Index] ^= WORLD_PACKET_STATIC_CRYPT_KEY[i & 63] ^ m_aServerKey[i & 7] ^ u8Previous;
					u8Previous = pBuffer[u32Index];
				}

				uint64_t u64CurrentKey = *((uint64_t*)m_aServerKey);

				u64CurrentKey += u32Length;

				memcpy(m_aServerKey, &u64CurrentKey, 8);
			}
		}
	}
}