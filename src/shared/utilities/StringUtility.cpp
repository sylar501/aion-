#include <shared/utilities/StringUtility.h>

#include <sstream>
#include <iomanip>

#include <openssl/sha.h>

namespace shared
{
	namespace utilities
	{
		std::string StringUtility::BytesToHexString(uint8_t* pBytes, uint32_t u32Length)
		{
			std::stringstream oStringStream;
			
			oStringStream << std::hex << std::setfill('0');

			for (uint32_t i = 0; i < u32Length; i++)
			{
				oStringStream << std::setw(2) << (int) pBytes[i];
			}

			return oStringStream.str();
		}

		std::vector<std::string> StringUtility::Explode(const std::string& strValue, const std::string& strSeparator)
		{
			std::vector<std::string> vResult;
			std::string strSubject = strValue;

			size_t pos = 0;
			std::string token;
			while ((pos = strSubject.find(strSeparator)) != std::string::npos)
			{
				token = strSubject.substr(0, pos);
				vResult.push_back(token);
				strSubject.erase(0, pos + strSeparator.length());
			}

			vResult.push_back(strSubject);

			return vResult;
		}

		std::string StringUtility::HashStringSHA1(const std::string& strValue)
		{
			SHA_CTX oSHAContext;

			if (SHA1_Init(&oSHAContext))
			{
				if (SHA1_Update(&oSHAContext, strValue.c_str(), strValue.length()))
				{
					uint8_t aResult[SHA_DIGEST_LENGTH];
					if (SHA1_Final(aResult, &oSHAContext))
					{
						return BytesToHexString(aResult, SHA_DIGEST_LENGTH);
					}
				}
			}

			return std::string();
		}

		std::vector<uint8_t> StringUtility::IPv4StringToByteArray(const std::string& strAddress)
		{
			std::vector<std::string> vElements = Explode(strAddress, ".");

			if (vElements.size() != 4) return std::vector<uint8_t>();

			std::vector<uint8_t> vResult;

			for (int i = 0; i < 4; i++)
			{
				vResult.push_back((uint8_t)atoi(vElements.at(i).c_str()));
			}

			return vResult;
		}
	}
}