#ifndef OPENGLEXCEPTIONS_H_
#define OPENGLEXCEPTIONS_H_

#include <Exception.h>

class OpenGLException: public Exception
{
public:
	OpenGLException(const std::string& rReason) :
		Exception(rReason)
	{
	}

protected:
	OpenGLException()
	{
	}

};

#endif
