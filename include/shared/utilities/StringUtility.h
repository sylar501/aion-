#ifndef AION_SHARED_UTILITIES_STRINGUTILITY_H
#define AION_SHARED_UTILITIES_STRINGUTILITY_H

#include <string>
#include <vector>

namespace shared
{
	namespace utilities
	{
		class StringUtility
		{
		public:
			static std::string BytesToHexString(uint8_t* pBytes, uint32_t u32Length);

			static std::vector<std::string> Explode(const std::string& strValue, const std::string& strSeparator);

			static std::string HashStringSHA1(const std::string& strValue);
			static std::vector<uint8_t> IPv4StringToByteArray(const std::string& strAddress);
		};
	}
}

#endif