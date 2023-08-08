#ifndef FASTCG_OPENGL_TEXTURE_H
#define FASTCG_OPENGL_TEXTURE_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/BaseTexture.h>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLTexture : public BaseTexture
    {
    public:
        inline operator GLuint() const
        {
            return mTextureId;
        }

        OpenGLTexture(const Args &rArgs);
        OpenGLTexture(const OpenGLTexture &rOther) = delete;
        OpenGLTexture(const OpenGLTexture &&rOther) = delete;
        virtual ~OpenGLTexture();

    private:
        GLuint mTextureId{~0u};

        OpenGLTexture operator=(const OpenGLTexture &rOther) = delete;

        friend class OpenGLGraphicsSystem;
    };
}

#endif

#endif