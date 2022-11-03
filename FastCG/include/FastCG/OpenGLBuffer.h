#ifndef FASTCG_OPENGL_BUFFER_H
#define FASTCG_OPENGL_BUFFER_H

#ifdef FASTCG_OPENGL

#include <FastCG/Exception.h>
#include <FastCG/BaseBuffer.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

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
        virtual ~OpenGLBuffer();

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif