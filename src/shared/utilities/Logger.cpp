#include <shared/utilities/Logger.h>

#include <stdio.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

shared::Logger sLogger;

namespace shared
{
	void Logger::Debug(const char* szFormat, ...)
	{
		va_list args;
		va_start(args, szFormat);
		Write(NULL, "DBG", szFormat, args);
		va_end(args);
	}

	void Logger::Info(const char* szFormat, ...)
	{
		va_list args;
		va_start(args, szFormat);
		Write(ANSI_COLOR_GREEN, "INF", szFormat, args);
		va_end(args);
	}

	void Logger::Warning(const char* szFormat, ...)
	{
		va_list args;
		va_start(args, szFormat);
		Write(ANSI_COLOR_YELLOW, "WRN", szFormat, args);
		va_end(args);
	}

	void Logger::Error(const char* szFormat, ...)
	{
		va_list args;
		va_start(args, szFormat);
		Write(ANSI_COLOR_RED, "ERR", szFormat, args);
		va_end(args);
	}

	void Logger::Write(const char* szColor, const char* szLevel, const char* szFormat, va_list args)
	{
		char szBuffer[1024] = { 0 };
		char szTime[128] = { 0 };

		int64_t i64Now = time(NULL);
		strftime(szTime, sizeof(szTime), "%H:%M:%S", localtime(&i64Now));

		if (szColor)
		{
			snprintf(szBuffer, sizeof(szBuffer), "%s%s [%s] %s" ANSI_COLOR_RESET "\n", szColor, szTime, szLevel, szFormat);

			m_mtx.lock();

			vprintf(szBuffer, args);

			m_mtx.unlock();
		}
		else
		{
			snprintf(szBuffer, sizeof(szBuffer), "%s [%s] %s\n", szTime, szLevel, szFormat);

			m_mtx.lock();

			vprintf(szBuffer, args);

			m_mtx.unlock();
		}
	}
}