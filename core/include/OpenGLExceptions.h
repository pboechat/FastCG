#ifndef OPENGLEXCEPTIONS_H_
#define OPENGLEXCEPTIONS_H_

#include <Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

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

// ====================================================================================================

#define EXPAND(x) x
#define CHECK_FOR_OPENGL_ERRORS() \
{ \
	unsigned int __errorCode; \
	if ((__errorCode = glGetError()) != GL_NO_ERROR) \
{ \
	const unsigned char* __pErrorMessage = gluErrorString(__errorCode); \
	std::stringstream __stringStream; \
	if (__pErrorMessage) \
{ \
	__stringStream << __pErrorMessage; \
} \
	__stringStream << "\n" << "(error code: %d)"; \
	EXPAND(THROW_EXCEPTION(OpenGLException, __stringStream.str().c_str(), __errorCode)); \
} \
} \

#endif
