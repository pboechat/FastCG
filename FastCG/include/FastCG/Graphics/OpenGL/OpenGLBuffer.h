#ifndef FASTCG_OPENGL_BUFFER_H
#define FASTCG_OPENGL_BUFFER_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/BaseBuffer.h>

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

        OpenGLBuffer(const BufferArgs &rArgs);
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