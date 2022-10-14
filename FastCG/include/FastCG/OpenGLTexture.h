#ifndef FASTCG_OPENGL_TEXTURE_H
#define FASTCG_OPENGL_TEXTURE_H

#ifdef FASTCG_OPENGL

#include <FastCG/BaseTexture.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLTexture : public BaseTexture
    {
    public:
        virtual ~OpenGLTexture();

        void OnInitialize(bool generateMipmaps, void *pData) override;
        void Bind() const override;
        void Unbind() const override;

    private:
        GLuint mTextureId{~0u};

        void DeallocateResources();
    };
}

#endif

#endif