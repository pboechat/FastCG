#ifndef COLOREXCEPTIONS_H_
#define COLOREXCEPTIONS_H_

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
