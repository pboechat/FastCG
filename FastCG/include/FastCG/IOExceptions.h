#ifndef FASTCG_IO_EXCEPTIONS_H_
#define FASTCG_IO_EXCEPTIONS_H_

#include <FastCG/Exception.h>

namespace FastCG
{
	class IOException : public Exception
	{
	public:
		IOException(const std::string& rReason) :
			Exception(rReason)
		{
		}

	protected:
		IOException() : Exception()
		{
		}

	};

	class FileNotFoundException : public IOException
	{
	public:
		FileNotFoundException(const std::string& rReason) : IOException(rReason)
		{
		}

	};

}

#endif
