#ifndef FASTCG_EXCEPTION_H_
#define FASTCG_EXCEPTION_H_

#include <string>
#include <sstream>
#include <exception>
#include <cstdio>

namespace FastCG
{
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& rReason) :
			mReason(rReason)
		{
		}

		virtual const char* what() const
		{
			return mReason.c_str();
		}

		inline void SetFunction(const std::string& rFunction)
		{
			mFunction = rFunction;
		}

		inline void SetFile(const std::string& rFile)
		{
			mFile = rFile;
		}

		inline void SetLine(size_t line)
		{
			mLine = line;
		}

		inline const std::string& GetFunction() const
		{
			return mFunction;
		}

		inline const std::string& GetFile() const
		{
			return mFile;
		}

		inline size_t GetLine() const
		{
			return mLine;
		}

		inline const std::string& GetReason() const
		{
			return mReason;
		}

		inline std::string GetFullDescription() const
		{
			std::stringstream stringStream;
			stringStream << "Reason: " << mReason << "\n\n";
			stringStream << "Where: " << mFunction << "(..) @(" << mFile << ":" << mLine << ")\n\n";
			return stringStream.str();
		}

	protected:
		std::string mReason;
		std::string mFunction;
		std::string mFile;
		size_t mLine{ 0 };

		Exception() :
			mReason(""), mFunction(""), mFile(""), mLine(0)
		{
		}

	};

#ifdef _DEBUG

#ifdef _WIN32
#define BREAK_TO_DEBUGGER() __debugbreak()
#else
#error "BREAK_TO_DEBUGGER() is not implemented on the current platform"
#endif

#define THROW_EXCEPTION(exceptionType, format, ...) \
{ \
	char __buffer[4096]; \
	sprintf_s(__buffer, sizeof(__buffer) / sizeof(char), format, ##__VA_ARGS__); \
	exceptionType __exception(__buffer); \
	__exception.SetFunction(__FUNCTION__); \
	__exception.SetFile(__FILE__); \
	__exception.SetLine(__LINE__); \
	BREAK_TO_DEBUGGER(); \
	throw __exception; \
}
#else
#define THROW_EXCEPTION(exceptionType, format, ...)
#endif
}

#endif
