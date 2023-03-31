#ifndef FASTCG_OPENGL_MATERIAL_H
#define FASTCG_OPENGL_MATERIAL_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/BaseMaterial.h>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLMaterial : public BaseMaterial<OpenGLBuffer, OpenGLShader, OpenGLTexture>
    {
    private:
        OpenGLMaterial(const MaterialArgs &rArgs, const OpenGLBuffer *pConstantsBuffer) : BaseMaterial(rArgs, pConstantsBuffer)
        {
        }

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif