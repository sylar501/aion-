#include <shared/network/Endian.h>

bool Endian::isPlatformBigEndian = false;
bool Endian::isPlatformBigEndianComputed = false;

bool Endian::IsPlatformBigEndian()
{
    if (!isPlatformBigEndianComputed)
    {
        union {
            uint32_t i;
            char c[4];
        } endiancheck = {0x01020304};

        isPlatformBigEndian = endiancheck.c[0] == 1;
        isPlatformBigEndianComputed = true;
    }

    return isPlatformBigEndian;
}
