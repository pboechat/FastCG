#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsContext.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/Core/Exception.h>

#include <vector>
#include <cassert>
#include <algorithm>

namespace FastCG
{
    OpenGLGraphicsContext::OpenGLGraphicsContext(const Args &rArgs)
        : BaseGraphicsContext<OpenGLBuffer, OpenGLShader, OpenGLTexture>(rArgs)
    {
#ifdef _DEBUG
        glGenQueries(FASTCG_ARRAYSIZE(mTimeElapsedQueries), mTimeElapsedQueries);
        FASTCG_CHECK_OPENGL_ERROR();
#endif
    }

    OpenGLGraphicsContext::~OpenGLGraphicsContext()
    {
#ifdef _DEBUG
        glDeleteQueries(FASTCG_ARRAYSIZE(mTimeElapsedQueries), mTimeElapsedQueries);
#endif
    }

    void OpenGLGraphicsContext::Begin()
    {
        assert(mEnded);
        mEnded = false;
#ifdef _DEBUG
        mElapsedTime = 0;
        glBeginQuery(GL_TIME_ELAPSED, mTimeElapsedQueries[mCurrentQuery]);
        FASTCG_CHECK_OPENGL_ERROR();
#endif
    }

    void OpenGLGraphicsContext::PushDebugMarker(const char *pName)
    {
#ifdef _DEBUG
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, pName);
#endif
    }

    void OpenGLGraphicsContext::PopDebugMarker()
    {
#ifdef _DEBUG
        glPopDebugGroup();
#endif
    }

    void OpenGLGraphicsContext::SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        glViewport((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height);
    }

    void OpenGLGraphicsContext::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        glScissor((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height);
    }

    void OpenGLGraphicsContext::SetBlend(bool blend)
    {
        if (blend)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    void OpenGLGraphicsContext::SetBlendFunc(BlendFunc color, BlendFunc alpha)
    {
        assert(color != BlendFunc::NONE);
        if (alpha == BlendFunc::NONE)
        {
            glBlendEquation(GetOpenGLBlendFunc(color));
        }
        else
        {
            glBlendEquationSeparate(GetOpenGLBlendFunc(color), GetOpenGLBlendFunc(alpha));
        }
    }

    void OpenGLGraphicsContext::SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha)
    {
        glBlendFuncSeparate(GetOpenGLBlendFactor(srcColor), GetOpenGLBlendFactor(dstColor), GetOpenGLBlendFactor(srcAlpha), GetOpenGLBlendFactor(dstAlpha));
    }

    void OpenGLGraphicsContext::SetStencilTest(bool stencilTest)
    {
        if (stencilTest)
        {
            glEnable(GL_STENCIL_TEST);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }
    }

    void OpenGLGraphicsContext::SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask)
    {
        assert(face != Face::NONE);
        auto compareOp = GetOpenGLCompareOp(stencilFunc);
        if (face == Face::BACK || face == Face::FRONT)
        {
            glStencilFuncSeparate(GetOpenGLFace(face), compareOp, (GLint)ref, (GLuint)mask);
        }
        else
        {
            glStencilFunc(compareOp, (GLint)ref, (GLuint)mask);
        }
    }

    void OpenGLGraphicsContext::SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass)
    {
        assert(face != Face::NONE);
        glStencilOpSeparate(GetOpenGLFace(face), GetOpenGLStencilFunc(stencilFail), GetOpenGLStencilFunc(depthFail), GetOpenGLStencilFunc(depthPass));
    }

    void OpenGLGraphicsContext::SetStencilWriteMask(Face face, uint32_t mask)
    {
        assert(face != Face::NONE);
        glStencilMaskSeparate(GetOpenGLFace(face), mask);
    }

    void OpenGLGraphicsContext::SetDepthTest(bool depthTest)
    {
        if (depthTest)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }

    void OpenGLGraphicsContext::SetDepthWrite(bool depthWrite)
    {
        if (depthWrite)
        {
            glDepthMask(GL_TRUE);
        }
        else
        {
            glDepthMask(GL_FALSE);
        }
    }

    void OpenGLGraphicsContext::SetDepthFunc(CompareOp depthFunc)
    {
        glDepthFunc(GetOpenGLCompareOp(depthFunc));
    }

    void OpenGLGraphicsContext::SetScissorTest(bool scissorTest)
    {
        if (scissorTest)
        {
            glEnable(GL_SCISSOR_TEST);
        }
        else
        {
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void OpenGLGraphicsContext::SetCullMode(Face face)
    {
        if (face == Face::NONE)
        {
            glDisable(GL_CULL_FACE);
        }
        else
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GetOpenGLFace(face));
        }
    }

    void OpenGLGraphicsContext::Copy(const OpenGLBuffer *pBuffer, size_t dataSize, const void *pData)
    {
        assert(pBuffer != nullptr);
        auto target = GetOpenGLTarget(pBuffer->GetUsage());
        glBindBuffer(target, *pBuffer);
        glBufferSubData(target, 0, (GLsizeiptr)dataSize, (const GLvoid *)pData);
    }

    void OpenGLGraphicsContext::Copy(const OpenGLTexture *pTexture, size_t dataSize, const void *pData)
    {
        assert(pTexture != nullptr);
        auto target = GetOpenGLTarget(pTexture->GetType());
        glBindTexture(target, *pTexture);
        glTexSubImage2D(target, 0, 0, 0, (GLsizei)pTexture->GetWidth(), (GLsizei)pTexture->GetHeight(), GetOpenGLFormat(pTexture->GetFormat()), GetOpenGLDataType(pTexture->GetFormat(), pTexture->GetBitsPerChannel()), (const GLvoid *)pData);
    }

    void OpenGLGraphicsContext::BindShader(const OpenGLShader *pShader)
    {
        assert(pShader != nullptr);
        if (mpBoundShader == pShader)
        {
            return;
        }
        glUseProgram(pShader->GetProgramId());
        mpBoundShader = pShader;
        mLastUsedTextureUnit = 0;
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLBuffer *pBuffer, const char *pName)
    {
        assert(pBuffer != nullptr);
        assert(pName != nullptr);
        assert(mpBoundShader != nullptr);
        auto resourceInfo = mpBoundShader->GetResourceInfo(pName);
        if (resourceInfo.binding == -1)
        {
            return;
        }
        glBindBufferBase(GetOpenGLTarget(pBuffer->GetUsage()), resourceInfo.binding, *pBuffer);
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLTexture *pTexture, const char *pName)
    {
        assert(mpBoundShader != nullptr);
        auto resourceInfo = mpBoundShader->GetResourceInfo(pName);
        if (resourceInfo.location == -1 || resourceInfo.binding == -1)
        {
            return;
        }
        glActiveTexture(GL_TEXTURE0 + resourceInfo.binding);
        glBindTexture(GL_TEXTURE_2D, *pTexture);
        glUniform1i(resourceInfo.location, resourceInfo.binding);
        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLGraphicsContext::Blit(const OpenGLTexture *pSrc, const OpenGLTexture *pDst)
    {
        const auto *pBackbuffer = OpenGLGraphicsSystem::GetInstance()->GetBackbuffer();
        GLint srcWidth, srcHeight;
        if (pSrc == pBackbuffer)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            srcWidth = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenWidth();
            srcHeight = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenHeight();
        }
        else
        {
            auto readFbo = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(&pSrc, 1, nullptr);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            srcWidth = (GLint)pSrc->GetWidth();
            srcHeight = (GLint)pSrc->GetHeight();
        }
        GLint dstWidth, dstHeight;
        if (pDst == pBackbuffer)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            dstWidth = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenWidth();
            dstHeight = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenHeight();
        }
        else
        {
            auto drawFbo = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(&pDst, 1, nullptr);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            dstWidth = (GLint)pDst->GetWidth();
            dstHeight = (GLint)pDst->GetHeight();
        }
        GLbitfield mask;
        if (pSrc->GetFormat() == TextureFormat::DEPTH_STENCIL)
        {
            mask = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
        }
        else
        {
            mask = GL_COLOR_BUFFER_BIT;
        }
        glBlitFramebuffer(0, 0, srcWidth, srcHeight, 0, 0, dstWidth, dstHeight, mask, GL_LINEAR);
    }

    void OpenGLGraphicsContext::ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor)
    {
        glClearBufferfv(GL_COLOR, (GLint)renderTargetIndex, (const GLfloat *)&rClearColor[0]);
    }

    void OpenGLGraphicsContext::ClearDepthStencilBuffer(float depth, int32_t stencil)
    {
        GLboolean oldDepthWrite;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthWrite);
        GLint oldStencilWriteMask;
        glGetIntegerv(GL_STENCIL_WRITEMASK, &oldStencilWriteMask);
        SetDepthWrite(true);
        SetStencilWriteMask(Face::FRONT_AND_BACK, 0xff);
        glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
        SetDepthWrite(oldDepthWrite);
        SetStencilWriteMask(Face::FRONT_AND_BACK, oldStencilWriteMask);
    }

    void OpenGLGraphicsContext::ClearDepthBuffer(float depth)
    {
        GLboolean oldDepthTest;
        glGetBooleanv(GL_DEPTH_TEST, &oldDepthTest);
        SetDepthTest(true);
        glClearBufferfv(GL_DEPTH, 0, &depth);
        SetDepthTest(oldDepthTest);
    }

    void OpenGLGraphicsContext::ClearStencilBuffer(int32_t stencil)
    {
        GLint oldStencilMask;
        glGetIntegerv(GL_STENCIL_WRITEMASK, &oldStencilMask);
        SetStencilWriteMask(Face::FRONT_AND_BACK, 0xff);
        glClearBufferiv(GL_STENCIL, 0, &stencil);
        SetStencilWriteMask(Face::FRONT_AND_BACK, oldStencilMask);
    }

    void OpenGLGraphicsContext::SetRenderTargets(const OpenGLTexture *const *pRenderTargets, uint32_t renderTargetCount, const OpenGLTexture *pDepthStencilBuffer)
    {
        if (renderTargetCount == 1 &&
            pRenderTargets[0] == OpenGLGraphicsSystem::GetInstance()->GetBackbuffer() &&
            pDepthStencilBuffer == nullptr)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            return;
        }

        auto fbId = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(pRenderTargets, renderTargetCount, pDepthStencilBuffer);
        assert(fbId != ~0u);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbId);

        std::vector<GLenum> attachments;
        attachments.reserve(renderTargetCount);
        std::for_each(pRenderTargets, pRenderTargets + renderTargetCount, [&attachments, i = 0](const auto *pTexture) mutable
                      { if (pTexture->GetFormat() != TextureFormat::DEPTH_STENCIL) attachments.emplace_back(GL_COLOR_ATTACHMENT0 + (i++)); });
        assert(attachments.size() <= (size_t)OpenGLGraphicsSystem::GetInstance()->GetDeviceProperties().maxDrawBuffers);
        if (!attachments.empty())
        {
            glDrawBuffers((GLsizei)attachments.size(), &attachments[0]);
        }
    }

    void OpenGLGraphicsContext::SetVertexBuffers(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount)
    {
        assert(pBuffers != nullptr);
        assert(mpBoundShader != nullptr);
        assert(bufferCount > 0);
        auto vaoId = OpenGLGraphicsSystem::GetInstance()->GetOrCreateVertexArray(pBuffers, bufferCount);
        assert(vaoId != ~0u);
        glBindVertexArray(vaoId);
    }

    void OpenGLGraphicsContext::SetIndexBuffer(const OpenGLBuffer *pBuffer)
    {
        assert(pBuffer != nullptr);
        assert(mpBoundShader != nullptr);
        auto target = GetOpenGLTarget(pBuffer->GetUsage());
        assert(target == GL_ELEMENT_ARRAY_BUFFER);
        glBindBuffer(target, *pBuffer);
    }

    void OpenGLGraphicsContext::DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
    {
        glDrawElementsBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT, (GLvoid *)(uintptr_t)(firstIndex * sizeof(uint32_t)), (GLint)vertexOffset);
        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLGraphicsContext::DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
    {
        assert(firstInstance == 0);
        glDrawElementsInstancedBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT, (GLvoid *)(uintptr_t)(firstIndex * sizeof(uint32_t)), (GLsizei)instanceCount, (GLint)vertexOffset);
        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLGraphicsContext::End()
    {
        assert(!mEnded);
        mpBoundShader = nullptr;
        FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
        glEndQuery(GL_TIME_ELAPSED);
        FASTCG_CHECK_OPENGL_ERROR();
        mEndedQuery[mCurrentQuery] = true;
#endif
        mEnded = true;
    }

#ifdef _DEBUG
    void OpenGLGraphicsContext::RetrieveElapsedTime()
    {
        assert(mEnded);

        mCurrentQuery = (mCurrentQuery + 1) % FASTCG_ARRAYSIZE(mTimeElapsedQueries);

        GLuint64 elapsedTime = 0;
        if (mEndedQuery[mCurrentQuery])
        {
            // retrieves the result of the previous query

            GLint done = 0;
            while (!done)
            {
                glGetQueryObjectiv(mTimeElapsedQueries[mCurrentQuery], GL_QUERY_RESULT_AVAILABLE, &done);
            }

            glGetQueryObjectui64v(mTimeElapsedQueries[mCurrentQuery], GL_QUERY_RESULT, &elapsedTime);
        }

        mElapsedTime = elapsedTime * 1e-9;
    }
#endif

    double OpenGLGraphicsContext::GetElapsedTime() const
    {
#ifdef _DEBUG
        return mElapsedTime;
#else
        return 0;
#endif
    }

}

#endif