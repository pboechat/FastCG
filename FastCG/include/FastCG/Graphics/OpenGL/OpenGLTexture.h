#ifndef FASTCG_OPENGL_TEXTURE_H
#define FASTCG_OPENGL_TEXTURE_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/BaseTexture.h>

#include <GL/glew.h>
#include <GL/gl.h>

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

    private:
        GLuint mTextureId{~0u};

        OpenGLTexture(const TextureArgs &rArgs);
        OpenGLTexture(const OpenGLTexture &rOther) = delete;
        OpenGLTexture(const OpenGLTexture &&rOther) = delete;
        virtual ~OpenGLTexture();

        OpenGLTexture operator=(const OpenGLTexture &rOther) = delete;

        friend class OpenGLGraphicsSystem;
    };
}

#endif

#endif