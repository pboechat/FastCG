#ifndef FASTCG_OPENGL_MATERIAL_H
#define FASTCG_OPENGL_MATERIAL_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLBuffer.h>
#include <FastCG/BaseMaterial.h>

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