#ifndef OPENGLEXCEPTIONS_H
#define OPENGLEXCEPTIONS_H

#include "Exception.h"

class OpenGLException : public Exception
{
public:
	OpenGLException(const std::string& rReason) 
		: 
		Exception(rReason)
	{
	}

protected:
	OpenGLException()
	{
	}

};

#endif