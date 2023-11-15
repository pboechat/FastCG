#ifndef FASTCG_OPENGL_EXCEPTIONS_H
#define FASTCG_OPENGL_EXCEPTIONS_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Core/Exception.h>

#include <stdio.h>

namespace FastCG
{
	class OpenGLException : public Exception
	{
	public:
		OpenGLException(const std::string &rReason) : Exception(rReason) {}

	protected:
		OpenGLException() = default;
	};

}

inline const char *glGetErrorString(GLenum error)
{
#if defined FASTCG_ANDROID

#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str) \
	case str:                   \
		return #str

	switch (error)
	{
		CASE_RETURN_STRING(GL_NO_ERROR);
		CASE_RETURN_STRING(GL_INVALID_ENUM);
		CASE_RETURN_STRING(GL_INVALID_VALUE);
		CASE_RETURN_STRING(GL_INVALID_OPERATION);
		CASE_RETURN_STRING(GL_OUT_OF_MEMORY);
		CASE_RETURN_STRING(GL_INVALID_FRAMEBUFFER_OPERATION);
	default:
		FASTCG_THROW_EXCEPTION(FastCG::Exception, "OpenGL: Unhandled error %d", (int)error);
		return nullptr;
	}

#undef CAST_RETURN_STRING

#else
	return (const char *)gluErrorString(error);
#endif
}

#ifdef _DEBUG
#define FASTCG_CHECK_OPENGL_ERROR(fmt, ...)                                                                                    \
	{                                                                                                                          \
		GLenum __error;                                                                                                        \
		if ((__error = glGetError()) != GL_NO_ERROR)                                                                           \
		{                                                                                                                      \
			char __openGLErrorBuffer[4096];                                                                                    \
			FASTCG_COMPILER_WARN_PUSH                                                                                                   \
			FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION                                                                               \
			snprintf(__openGLErrorBuffer, FASTCG_ARRAYSIZE(__openGLErrorBuffer), fmt, ##__VA_ARGS__);                          \
			FASTCG_COMPILER_WARN_POP                                                                                                    \
			FASTCG_THROW_EXCEPTION(FastCG::OpenGLException, "%s (error: %s)", __openGLErrorBuffer, glGetErrorString(__error)); \
		}                                                                                                                      \
	}
#else
#define FASTCG_CHECK_OPENGL_ERROR(...)
#endif

#endif

#endif
