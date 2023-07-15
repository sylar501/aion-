#include <shared/utilities/FileUtility.h>

#include <fstream>
#include <sstream>

namespace shared
{
	namespace utilities
	{
		bool FileUtility::Exists(const std::string& strFilePath)
		{
			struct stat st;
			return stat(strFilePath.c_str(), &st) == 0;
		}

		std::string FileUtility::ReadToString(const std::string strFilePath)
		{
			if (Exists(strFilePath))
			{
				std::ifstream oFileStream(strFilePath);

				if (oFileStream.good())
				{
					std::stringstream oStringStream;
					oStringStream << oFileStream.rdbuf();
					return oStringStream.str();
				}
			}

			return std::string();			
		}
	}
}