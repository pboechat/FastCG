#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLMaterial.h>
#include <FastCG/OpenGLExceptions.h>

constexpr GLuint gMaterialConstantsBindingIndex = 0x10;

namespace FastCG
{
    OpenGLMaterial::OpenGLMaterial(const MaterialArgs &rArgs) : BaseMaterial(rArgs)
    {
        glGenBuffers(1, &mMaterialConstantsBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, mMaterialConstantsBufferId);
#ifdef _DEBUG
        {
            auto bufferLabel = mArgs.name + " Material Constants (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mMaterialConstantsBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel.c_str());
        }
#endif
        glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialConstants), &mMaterialConstants, GL_DYNAMIC_DRAW);

        mColorMapBindingLocation = mArgs.pShader->GetBindingLocation("uColorMap");
        mBumpMapBindingLocation = mArgs.pShader->GetBindingLocation("uBumpMap");
    }

    OpenGLMaterial::~OpenGLMaterial()
    {
        if (mMaterialConstantsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mMaterialConstantsBufferId);
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

        mArgs.pShader->Bind();

        glBindBuffer(GL_UNIFORM_BUFFER, mMaterialConstantsBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialConstants), &mMaterialConstants);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, gMaterialConstantsBindingIndex, mMaterialConstantsBufferId);

        if (mpColorMap != nullptr && mColorMapBindingLocation != -1)
        {
            mArgs.pShader->BindTexture(mColorMapBindingLocation, *mpColorMap, 0);
        }
        if (mpBumpMap != nullptr && mBumpMapBindingLocation != -1)
        {
            mArgs.pShader->BindTexture(mBumpMapBindingLocation, *mpBumpMap, 1);
        }

        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLMaterial::Unbind() const
    {
        mArgs.pShader->Unbind();
    }

}

#endif