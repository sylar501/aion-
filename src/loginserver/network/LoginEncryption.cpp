#include "LoginEncryption.h"

#include <stdlib.h>
#include <openssl/rand.h>
#include <stdio.h>

uint8_t sLoginEncryptionInitialKey[16] =
{
	0x6b, 0x60, 0xcb, 0x5b, 0x82, 0xce, 0x90, 0xb1, 0xcc, 0x2b, 0x6c, 0x55, 0x6c, 0x6c, 0x6c, 0x6c
};

LoginEncryption::LoginEncryption()
{
	m_oBlowfish.SetKey(sLoginEncryptionInitialKey);

	RAND_bytes(m_aBlowfishKey, 16);
}

bool LoginEncryption::Decrypt(uint8_t* aBuffer, size_t u64Length)
{
	return true;
}

size_t LoginEncryption::Encrypt(uint8_t* aBuffer, size_t u64Length)
{
	printf("Before Encrypt: %llu\n", u64Length);

	u64Length += 4;

	if (m_bFirstPacket)
	{
		u64Length += 4;
		u64Length += 8 - u64Length % 8;

		int32_t u32XORKey = rand();

		int32_t edx;
		int32_t ecx = u32XORKey;
		int32_t pos = 4;
		int32_t stop = (int32_t)u64Length - 8;

		while (pos < stop)
		{
			edx = aBuffer[pos] & 0xff;
			edx |= (aBuffer[pos + 1] & 0xff) << 8;
			edx |= (aBuffer[pos + 2] & 0xff) << 16;
			edx |= (aBuffer[pos + 3] & 0xff) << 24;
			ecx += edx;
			edx ^= ecx;
			aBuffer[pos++] = edx & 0xff;
			aBuffer[pos++] = edx >> 8 & 0xff;
			aBuffer[pos++] = edx >> 16 & 0xff;
			aBuffer[pos++] = edx >> 24 & 0xff;
		}

		aBuffer[pos++] = ecx & 0xff;
		aBuffer[pos++] = ecx >> 8 & 0xff;
		aBuffer[pos++] = ecx >> 16 & 0xff;
		aBuffer[pos++] = ecx >> 24 & 0xff;

		m_oBlowfish.Cipher(aBuffer, u64Length);

		m_oBlowfish.SetKey(m_aBlowfishKey);

		printf("After Encrypt: %llu\n", u64Length);

		m_bFirstPacket = false;
	}
	else
	{
		u64Length += 8 - u64Length % 8;

		int64_t chksum = 0;
		int32_t count = (int32_t)u64Length - 4;
		int64_t ecx;
		int i;

		for (i = 0; i < count; i += 4)
		{
			ecx = aBuffer[i] & 0xff;
			ecx |= aBuffer[i + 1] << 8 & 0xff00;
			ecx |= aBuffer[i + 2] << 0x10 & 0xff0000;
			ecx |= aBuffer[i + 3] << 0x18 & 0xff000000;
			chksum ^= ecx;
		}

		ecx = aBuffer[i] & 0xff;
		ecx |= aBuffer[i + 1] << 8 & 0xff00;
		ecx |= aBuffer[i + 2] << 0x10 & 0xff0000;
		ecx |= aBuffer[i + 3] << 0x18 & 0xff000000;

		aBuffer[i] = chksum & 0xff;
		aBuffer[i + 1] = chksum >> 0x08 & 0xff;
		aBuffer[i + 2] = chksum >> 0x10 & 0xff;
		aBuffer[i + 3] = chksum >> 0x18 & 0xff;

		m_oBlowfish.Cipher(aBuffer, u64Length);
	}

	return u64Length;
}

uint8_t* LoginEncryption::GetBlowfishKey()
{
	return m_aBlowfishKey;
}
