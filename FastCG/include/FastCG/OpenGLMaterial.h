#ifndef FASTCG_OPENGL_MATERIAL_H
#define FASTCG_OPENGL_MATERIAL_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/BaseMaterial.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;
    class OpenGLForwardRenderingPathStrategy;
    class OpenGLDeferredRenderingPathStrategy;

    class OpenGLMaterial : public BaseMaterial<OpenGLShader, OpenGLTexture>
    {
    private:
        GLuint mMaterialConstantsBufferId{~0u};
        GLint mColorMapBindingLocation{-1};
        GLint mBumpMapBindingLocation{-1};

        OpenGLMaterial(const MaterialArgs &rArgs);
        virtual ~OpenGLMaterial();

        void Bind() const;
        void Unbind() const;

        friend class OpenGLRenderingSystem;
        friend class OpenGLForwardRenderingPathStrategy;
        friend class OpenGLDeferredRenderingPathStrategy;
    };

}

#endif

#endif