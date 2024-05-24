#ifndef FASTCG_OPENGL_BUFFER_H
#define FASTCG_OPENGL_BUFFER_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/BaseBuffer.h>
#include <FastCG/Graphics/OpenGL/OpenGL.h>

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

        OpenGLBuffer(const Args &rArgs);
        OpenGLBuffer(const OpenGLBuffer &rOther) = delete;
        OpenGLBuffer(const OpenGLBuffer &&rOther) = delete;
        virtual ~OpenGLBuffer();

    private:
        GLuint mBufferId{~0u};

        OpenGLBuffer &operator=(const OpenGLBuffer &rOther) = delete;

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif