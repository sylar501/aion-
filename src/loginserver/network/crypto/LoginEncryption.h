#ifndef AION_LOGINSERVER_NETWORK_CRYPTO_LOGINENCRYPTION_H
#define AION_LOGINSERVER_NETWORK_CRYPTO_LOGINENCRYPTION_H

#include <stdint.h>

#include "Blowfish.h"

namespace loginserver
{
	namespace network
	{
		namespace crypto
		{
			class LoginEncryption
			{
			public:
				LoginEncryption();

				bool		Decrypt(uint8_t* aBuffer, size_t u64Length);
				size_t		Encrypt(uint8_t* aBuffer, size_t u64Length);

				uint8_t*	GetBlowfishKey();
			private:
				bool		m_bFirstPacket = true;
				Blowfish	m_oBlowfish;
				uint8_t		m_aBlowfishKey[16] = { 0 };
			};

			extern uint8_t sLoginEncryptionInitialKey[16];
		}
	}
}

#endif