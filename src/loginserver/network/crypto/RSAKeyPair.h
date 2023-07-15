#ifndef AION_LOGINSERVER_NETWORK_RSAKEYPAIR_H
#define AION_LOGINSERVER_NETWORK_RSAKEYPAIR_H

#include <openssl/rsa.h>
#include <stdint.h>

namespace loginserver
{
	namespace network
	{
		namespace crypto
		{
			class RSAKeyPair
			{
			public:
				RSAKeyPair();
				~RSAKeyPair();

				uint8_t* GetEncryptedModulus();

				RSA* GetRSA();
			private:
				RSA* m_pRSA = nullptr;
				uint8_t		m_aEncrytedModulus[128] = { 0 };
			};
		}
	}
}

#endif