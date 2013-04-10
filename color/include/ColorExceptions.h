#ifndef COLOREXCEPTIONS_H
#define COLOREXCEPTIONS_H

#include "Exception.h"

class InvalidColorConversionException: public Exception
{
public:
	InvalidColorConversionException(const std::string& rReason) :
			Exception(rReason)
	{
	}

protected:
	InvalidColorConversionException()
	{
	}

};

#endif
