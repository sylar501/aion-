#ifndef AION_SHARED_UTILITIES_LOGGER_H
#define AION_SHARED_UTILITIES_LOGGER_H

#include <stdarg.h>
#include <mutex>

namespace shared
{
	class Logger
	{
	public:
		void		Debug(const char* szFormat, ...);
		void		Info(const char* szFormat, ...);
		void		Warning(const char* szFormat, ...);
		void		Error(const char* szFormat, ...);

	private:
		void		Write(const char* szColor, const char* szLevel, const char* szFormat, va_list args);

	private:
		std::mutex	m_mtx;
	};
}

extern shared::Logger sLogger;

#endif