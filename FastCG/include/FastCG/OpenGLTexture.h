#ifndef FASTCG_OPENGL_TEXTURE_H
#define FASTCG_OPENGL_TEXTURE_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseTexture.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;
    class OpenGLMaterial;
    class OpenGLForwardRenderingPathStrategy;
    class OpenGLDeferredRenderingPathStrategy;

    class OpenGLTexture : public BaseTexture
    {
    private:
        GLuint mTextureId{~0u};

        OpenGLTexture(const TextureArgs &rArgs);
        virtual ~OpenGLTexture();

        inline operator GLuint() const
        {
            return mTextureId;
        }

        friend class OpenGLRenderingSystem;
        friend class OpenGLMaterial;
        friend class OpenGLForwardRenderingPathStrategy;
        friend class OpenGLDeferredRenderingPathStrategy;
    };
}

#endif

#endif