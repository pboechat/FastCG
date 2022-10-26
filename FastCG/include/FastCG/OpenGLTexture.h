#ifndef FASTCG_OPENGL_TEXTURE_H
#define FASTCG_OPENGL_TEXTURE_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseTexture.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLTexture : public BaseTexture
    {
    public:
        inline operator GLuint() const
        {
            return mTextureId;
        }

    private:
        GLuint mTextureId{~0u};

        OpenGLTexture(const TextureArgs &rArgs);
        virtual ~OpenGLTexture();

        friend class OpenGLRenderingSystem;
    };
}

#endif

#endif