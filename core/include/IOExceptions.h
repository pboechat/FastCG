#ifndef IOEXCEPTIONS_H
#define IOEXCEPTIONS_H

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
	FileNotFoundException(const std::string& rFileName) :
			IOException("File could not be found: " + rFileName)
	{
	}
};

#endif
