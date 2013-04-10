#ifndef LOGUTILS_H
#define LOGUTILS_H

#include <stdarg.h>
#include <log4cpp/Category.hh>

#define BUFFER_SIZE 65536

class LogUtils
{
public:
	static void Debug(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);

		char pBuffer[BUFFER_SIZE];
		vsprintf(pBuffer, pFormat, args);

		log4cpp::Category::getRoot().debug(pBuffer);

		va_end(args);
	}

	static void Info(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);

		char pBuffer[BUFFER_SIZE];
		vsprintf(pBuffer, pFormat, args);

		log4cpp::Category::getRoot().info(pBuffer);

		va_end(args);
	}

	static void Warn(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);

		char pBuffer[BUFFER_SIZE];
		vsprintf(pBuffer, pFormat, args);

		log4cpp::Category::getRoot().warn(pBuffer);

		va_end(args);
	}

	static void Error(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);

		char pBuffer[BUFFER_SIZE];
		vsprintf(pBuffer, pFormat, args);

		log4cpp::Category::getRoot().error(pBuffer);

		va_end(args);
	}

	static void Fatal(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);

		char pBuffer[BUFFER_SIZE];
		vsprintf(pBuffer, pFormat, args);

		log4cpp::Category::getRoot().fatal(pBuffer);

		va_end(args);
	}

private:
	LogUtils();
	~LogUtils();

};

#endif
