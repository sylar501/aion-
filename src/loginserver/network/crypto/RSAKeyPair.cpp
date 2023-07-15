#include "RSAKeyPair.h"

namespace loginserver
{
	namespace network
	{
		namespace crypto
		{
			RSAKeyPair::RSAKeyPair()
			{
				m_pRSA = RSA_generate_key(1024, 65537, NULL, NULL);

				while (BN_num_bytes(RSA_get0_n(m_pRSA)) != 128)
				{
					printf("Generating another RSA Key Pair due to invalid modulus size\n");
					m_pRSA = RSA_generate_key(1024, 65537, NULL, NULL);
				}

				// Generate Encrypted Modulus.
				BN_bn2bin(RSA_get0_n(m_pRSA), m_aEncrytedModulus);

				for (uint8_t i = 0; i < 4; i++)
				{
					uint8_t old = m_aEncrytedModulus[i];
					m_aEncrytedModulus[i] = m_aEncrytedModulus[0x4d + i];
					m_aEncrytedModulus[0x4d + i] = old;
				}

				for (uint8_t i = 0; i < 0x40; i++)
				{
					m_aEncrytedModulus[i] = m_aEncrytedModulus[i] ^ m_aEncrytedModulus[0x40 + i];
				}

				for (uint8_t i = 0; i < 4; i++)
				{
					m_aEncrytedModulus[0x0d + i] = m_aEncrytedModulus[0x0d + i] ^ m_aEncrytedModulus[0x34 + i];
				}

				for (uint8_t i = 0; i < 0x40; i++)
				{
					m_aEncrytedModulus[0x40 + i] = m_aEncrytedModulus[0x40 + i] ^ m_aEncrytedModulus[i];
				}
			}

			RSAKeyPair::~RSAKeyPair()
			{
				RSA_free(m_pRSA);
			}

			uint8_t* RSAKeyPair::GetEncryptedModulus()
			{
				return m_aEncrytedModulus;
			}

			RSA* RSAKeyPair::GetRSA()
			{
				return m_pRSA;
			}
		}
	}
}