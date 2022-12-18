#ifndef FASTCG_OPENGL_MATERIAL_H
#define FASTCG_OPENGL_MATERIAL_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLBuffer.h>
#include <FastCG/BaseMaterial.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLMaterial : public BaseMaterial<OpenGLBuffer, OpenGLShader, OpenGLTexture>
    {
    private:
        OpenGLMaterial(const MaterialArgs &rArgs, const OpenGLBuffer *pConstantsBuffer) : BaseMaterial(rArgs, pConstantsBuffer)
        {
        }

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif