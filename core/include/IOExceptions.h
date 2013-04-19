#ifndef IOEXCEPTIONS_H_
#define IOEXCEPTIONS_H_

#include <Exception.h>

class IOException: public Exception
{
public:
	IOException(const std::string& rReason) :
		Exception(rReason)
	{
	}

protected:
	IOException() :
		Exception()
	{
	}

};

class FileNotFoundException: public IOException
{
public:
	FileNotFoundException(const std::string& rReason) :
		IOException(rReason)
	{
	}
};

#endif
