#ifndef FASTCG_OPENGL_BUFFER_H
#define FASTCG_OPENGL_BUFFER_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseBuffer.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLBuffer : public BaseBuffer
    {
    public:
        void SetData(size_t size, const void* pData) const;
        void SetSubData(size_t offset, size_t size, const void *pData) const;
        void Bind() const;
        void BindBase(GLuint location) const;

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