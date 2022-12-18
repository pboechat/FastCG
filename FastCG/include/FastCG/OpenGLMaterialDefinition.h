#ifndef FASTCG_OPENGL_MATERIAL_DEFINITION_H
#define FASTCG_OPENGL_MATERIAL_DEFINITION_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/BaseMaterialDefinition.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLMaterialDefinition : public BaseMaterialDefinition<OpenGLShader, OpenGLTexture>
    {
    private:
        OpenGLMaterialDefinition(const MaterialDefinitionArgs &rArgs) : BaseMaterialDefinition(rArgs)
        {
        }
        virtual ~OpenGLMaterialDefinition() = default;

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif