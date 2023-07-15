#ifndef AION_SHARED_UTILITIES_FILEUTILITY_H
#define AION_SHARED_UTILITIES_FILEUTILITY_H

#include <string>

namespace shared
{
	namespace utilities
	{
		class FileUtility
		{
		public:
			// Returns whether the file at the specified path exists.
			static bool Exists(const std::string& strFilePath);

			// Reads the file at the specified path and returns its contents as a string.
			static std::string ReadToString(const std::string strFilePath);
		};
	}
}

#endif