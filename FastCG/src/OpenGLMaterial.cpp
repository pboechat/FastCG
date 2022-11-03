#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLMaterial.h>
#include <FastCG/OpenGLExceptions.h>

namespace FastCG
{
    OpenGLMaterial::OpenGLMaterial(const MaterialArgs &rArgs) : BaseMaterial(rArgs, OpenGLRenderingSystem::GetInstance()->CreateBuffer({rArgs.name + " Material Constants",
                                                                                                                                        BufferType::UNIFORM,
                                                                                                                                        BufferUsage::DYNAMIC,
                                                                                                                                        sizeof(mMaterialConstants),
                                                                                                                                        &mMaterialConstants}))
    {
    }

    OpenGLMaterial::~OpenGLMaterial()
    {
        if (mpMaterialConstantsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpMaterialConstantsBuffer);
        }
    }

}

#endif