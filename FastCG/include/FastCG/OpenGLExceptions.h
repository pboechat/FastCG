#ifndef FASTCG_OPENGL_EXCEPTIONS_H_
#define FASTCG_OPENGL_EXCEPTIONS_H_

#include <FastCG/Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <sstream>

namespace FastCG
{
	class OpenGLException : public Exception
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

#ifdef _DEBUG
#define CHECK_FOR_OPENGL_ERRORS() \
{ \
	GLenum __errorCode; \
	if ((__errorCode = glGetError()) != GL_NO_ERROR) \
	{ \
		const auto* __pErrorMessage = gluErrorString(__errorCode); \
		std::stringstream __stringStream; \
		if (__pErrorMessage) \
		{ \
			__stringStream << __pErrorMessage; \
		} \
		__stringStream << "\n" << "(error code: %d)"; \
		THROW_EXCEPTION(OpenGLException, __stringStream.str().c_str(), __errorCode); \
	} \
}
#else
#define CHECK_FOR_OPENGL_ERRORS()
#endif

}

#endif