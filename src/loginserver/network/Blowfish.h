#ifndef AION_LOGINSERVER_NETWORK_BLOWFISH_H
#define AION_LOGINSERVER_NETWORK_BLOWFISH_H

#include <stdint.h>

class Blowfish
{
public:
	void SetKey(uint8_t* aKey);

	void Cipher(uint8_t* aBuffer, size_t u64Length);

public:
	void initArrays();
	void initSBox(uint8_t* aBase, int* aSBox);

	int byteArrayToInteger(uint8_t* aData);
	void integerToByteArray(int i32Value, uint8_t* aData);

	int feistel(int x);

private:
	uint8_t m_aKey[16] = { 0 };
	int m_pArray[18] = { 0 };
	int m_sBoxes[4][256] = { 0 };
};

#endif