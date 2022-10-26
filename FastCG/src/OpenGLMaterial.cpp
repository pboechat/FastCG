#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLMaterial.h>
#include <FastCG/OpenGLExceptions.h>

constexpr GLuint MATERIAL_CONSTANTS_BINDING_INDEX = 0x10;

namespace FastCG
{
    OpenGLMaterial::OpenGLMaterial(const MaterialArgs &rArgs) : BaseMaterial(rArgs)
    {
        mpMaterialConstantsBuffer = OpenGLRenderingSystem::GetInstance()->CreateBuffer({mName + " Material Constants",
                                                                                        BufferType::UNIFORM,
                                                                                        BufferUsage::DYNAMIC,
                                                                                        sizeof(mMaterialConstants),
                                                                                        &mMaterialConstants});

        mColorMapBindingLocation = mpShader->GetBindingLocation("uColorMap");
        mBumpMapBindingLocation = mpShader->GetBindingLocation("uBumpMap");
    }

    OpenGLMaterial::~OpenGLMaterial()
    {
        if (mpMaterialConstantsBuffer != nullptr)
        {
            OpenGLRenderingSystem::GetInstance()->DestroyBuffer(mpMaterialConstantsBuffer);
        }
    }

    void OpenGLMaterial::Bind() const
    {
        if (mHasDepth)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }
        glDisable(GL_STENCIL_TEST);
        glStencilMask(0);

        if (mTwoSided)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }

        glDisable(GL_BLEND);

        mpMaterialConstantsBuffer->SetSubData(0, sizeof(MaterialConstants), &mMaterialConstants);

        mpShader->Bind();

        mpMaterialConstantsBuffer->BindBase(MATERIAL_CONSTANTS_BINDING_INDEX);

        if (mpColorMap != nullptr && mColorMapBindingLocation != -1)
        {
            mpShader->BindTexture(mColorMapBindingLocation, *mpColorMap, 0);
        }
        if (mpBumpMap != nullptr && mBumpMapBindingLocation != -1)
        {
            mpShader->BindTexture(mBumpMapBindingLocation, *mpBumpMap, 1);
        }

        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLMaterial::Unbind() const
    {
        mpShader->Unbind();
    }

}

#endif