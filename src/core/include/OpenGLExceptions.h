#ifndef OPENGLEXCEPTIONS_H
#define OPENGLEXCEPTIONS_H

#include "Exception.h"

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glext.h>

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