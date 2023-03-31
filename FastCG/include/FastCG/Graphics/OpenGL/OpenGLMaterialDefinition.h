#ifndef FASTCG_OPENGL_MATERIAL_DEFINITION_H
#define FASTCG_OPENGL_MATERIAL_DEFINITION_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/BaseMaterialDefinition.h>

namespace FastCG
{
    class OpenGLGraphicsSystem;

    class OpenGLMaterialDefinition : public BaseMaterialDefinition<OpenGLShader, OpenGLTexture>
    {
    private:
        OpenGLMaterialDefinition(const MaterialDefinitionArgs &rArgs) : BaseMaterialDefinition(rArgs)
        {
        }
        virtual ~OpenGLMaterialDefinition() = default;

        friend class OpenGLGraphicsSystem;
    };

}

#endif

#endif