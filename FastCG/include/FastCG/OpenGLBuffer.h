#ifndef FASTCG_OPENGL_BUFFER_H
#define FASTCG_OPENGL_BUFFER_H

#ifdef FASTCG_OPENGL

#include <FastCG/Exception.h>
#include <FastCG/BaseBuffer.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLBuffer : public BaseBuffer
    {
    public:
        inline operator GLuint() const
        {
            return mBufferId;
        }

    private:
        GLuint mBufferId{~0u};

        OpenGLBuffer(const BufferArgs &rArgs);
        OpenGLBuffer(const OpenGLBuffer &rOther) = delete;
        OpenGLBuffer(const OpenGLBuffer &&rOther) = delete;
        virtual ~OpenGLBuffer();

        OpenGLBuffer &operator=(const OpenGLBuffer &rOther) = delete;

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif