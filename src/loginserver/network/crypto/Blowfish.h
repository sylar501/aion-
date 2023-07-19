#ifndef AION_LOGINSERVER_NETWORK_CRYPTO_BLOWFISH_H
#define AION_LOGINSERVER_NETWORK_CRYPTO_BLOWFISH_H

#include <stdint.h>

namespace loginserver
{
	namespace network
	{
		namespace crypto
		{
			class Blowfish
			{
			public:
				void		SetKey(uint8_t* aKey);
				void		Cipher(uint8_t* aBuffer, size_t u64Length);
				void		Decipher(uint8_t* aBuffer, size_t u64Length);

			public:
				void		initArrays();
				void		initSBox(uint8_t* aBase, uint32_t* aSBox);

				uint32_t	byteArrayToInteger(uint8_t* aData);
				void		integerToByteArray(uint32_t u32Value, uint8_t* aData);

				uint32_t	feistel(uint32_t x);

			private:
				uint8_t		m_aKey[16] = { 0 };
				uint32_t	m_pArray[18] = { 0 };
				uint32_t	m_sBoxes[4][256] = { 0 };
			};
		}
	}
}

#endif