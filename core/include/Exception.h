#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <cstdio>
#include <exception>
#include <string>
#include <sstream>

class Exception: public std::exception
{
public:
	Exception(const std::string& rReason) :
			mReason(rReason), mFunction(""), mFile(""), mLine(0)
	{
	}

	virtual ~Exception() throw ()
	{
	}

	virtual const char* what() const throw ()
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

	inline void SetLine(unsigned int line)
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

	inline unsigned int GetLine() const
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
	unsigned int mLine;

	Exception() :
			mReason(""), mFunction(""), mFile(""), mLine(0)
	{
	}

};

#define THROW_EXCEPTION(exceptionType, format, ...) \
{ \
	char __buffer[4096]; \
	sprintf(__buffer, format, __VA_ARGS__); \
	exceptionType __exception(__buffer); \
	__exception.SetFunction(__FUNCTION__); \
	__exception.SetFile(__FILE__); \
	__exception.SetLine(__LINE__); \
	throw __exception; \
} \

#endif
