#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsContext.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/Exception.h>

#include <vector>
#include <cassert>
#include <algorithm>

namespace FastCG
{
    OpenGLGraphicsContext::OpenGLGraphicsContext(const GraphicsContextArgs& rArgs)
        : BaseGraphicsContext<OpenGLBuffer, OpenGLShader, OpenGLTexture>(rArgs)
    {
#ifdef _DEBUG
        glGenQueries(FASTCG_ARRAYSIZE(mTimeElapsedQueries), mTimeElapsedQueries);
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
#endif
    }

    void OpenGLGraphicsContext::PushDebugMarker(const char *name)
    {
#ifdef _DEBUG
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name);
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
        auto target = GetOpenGLTarget(pBuffer->GetType());
        glBindBuffer(target, *pBuffer);
        glBufferSubData(target, 0, (GLsizeiptr)dataSize, (const GLvoid *)pData);
    }

    void OpenGLGraphicsContext::Copy(const OpenGLTexture *pTexture, size_t dataSize, const void *pData)
    {
        assert(pTexture != nullptr);
        auto target = GetOpenGLTarget(pTexture->GetType());
        glBindTexture(target, *pTexture);
        glTexSubImage2D(target, 0, 0, 0, (GLsizei)pTexture->GetWidth(), (GLsizei)pTexture->GetHeight(), GetOpenGLFormat(pTexture->GetFormat()), GetOpenGLDataType(pTexture->GetDataType(), pTexture->GetBitsPerPixel()), (const GLvoid *)pData);
    }

    void OpenGLGraphicsContext::BindShader(const OpenGLShader *pShader)
    {
        assert(pShader != nullptr);
        glUseProgram(*pShader);
        mpBoundShader = pShader;
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLBuffer *pBuffer, uint32_t index)
    {
        assert(pBuffer != nullptr);
        assert(mpBoundShader != nullptr);
        glBindBufferBase(GetOpenGLTarget(pBuffer->GetType()), index, *pBuffer);
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLBuffer *pBuffer, const char *name)
    {
        assert(pBuffer != nullptr);
        assert(mpBoundShader != nullptr);
        auto index = glGetUniformLocation(*mpBoundShader, name);
        BindResource(pBuffer, index);
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLTexture *pTexture, uint32_t index, uint32_t unit)
    {
        assert(pTexture != nullptr);
        assert(mpBoundShader != nullptr);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, *pTexture);
        glUniform1i(index, unit);
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLTexture *pTexture, const char *name, uint32_t unit)
    {
        assert(mpBoundShader != nullptr);
        auto index = glGetUniformLocation(*mpBoundShader, name);
        BindResource(pTexture, index, unit);
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
            auto readFbo = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(&pSrc, 1);
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
            auto drawFbo = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(&pDst, 1);
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

    void OpenGLGraphicsContext::ClearDepthStencilTarget(uint32_t renderTargetIndex, float depth, int32_t stencil)
    {
        GLboolean depthWrite;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
        GLint stencilWriteMask;
        glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilWriteMask);
        SetDepthWrite(true);
        SetStencilWriteMask(Face::FRONT_AND_BACK, 0xff);
        glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
        SetDepthWrite(depthWrite);
        SetStencilWriteMask(Face::FRONT_AND_BACK, stencilWriteMask);
    }

    void OpenGLGraphicsContext::ClearDepthTarget(uint32_t renderTargetIndex, float depth)
    {
        GLboolean depthTest;
        glGetBooleanv(GL_DEPTH_TEST, &depthTest);
        SetDepthTest(true);
        glClearBufferfv(GL_DEPTH, 0, &depth);
        SetDepthTest(depthTest);
    }

    void OpenGLGraphicsContext::ClearStencilTarget(uint32_t renderTargetIndex, int32_t stencil)
    {
        GLint stencilMask;
        glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilMask);
        SetStencilWriteMask(Face::FRONT_AND_BACK, 0xff);
        glClearBufferiv(GL_STENCIL, 0, &stencil);
        SetStencilWriteMask(Face::FRONT_AND_BACK, stencilMask);
    }

    void OpenGLGraphicsContext::SetRenderTargets(const OpenGLTexture *const *pTextures, size_t textureCount)
    {
        assert(pTextures != nullptr);
        assert(textureCount > 0);
        if (textureCount == 1 && pTextures[0] == OpenGLGraphicsSystem::GetInstance()->GetBackbuffer())
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            return;
        }
        auto fbId = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(pTextures, textureCount);
        assert(fbId != ~0u);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbId);
        std::vector<GLenum> attachments;
        attachments.reserve(textureCount);
        std::for_each(pTextures, pTextures + textureCount, [&attachments, i = 0](const auto *pTexture) mutable
                      { if (pTexture->GetFormat() != TextureFormat::DEPTH_STENCIL) attachments.emplace_back(GL_COLOR_ATTACHMENT0 + (i++)); });
        assert(attachments.size() <= (size_t)OpenGLGraphicsSystem::GetInstance()->GetDeviceProperties().maxDrawBuffers);
        if (!attachments.empty())
        {
            glDrawBuffers((GLsizei)attachments.size(), &attachments[0]);
        }
    }

    void OpenGLGraphicsContext::SetVertexBuffers(const OpenGLBuffer *const *pBuffers, size_t bufferCount)
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
        auto target = GetOpenGLTarget(pBuffer->GetType());
        assert(target == GL_ELEMENT_ARRAY_BUFFER);
        glBindBuffer(target, *pBuffer);
    }

    void OpenGLGraphicsContext::DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
    {
        glDrawElementsBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT, (void *)(uintptr_t)firstIndex, (GLint)vertexOffset);
    }

    void OpenGLGraphicsContext::DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
    {
        assert(firstInstance == 0);
        glDrawElementsInstancedBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT, (void *)(uintptr_t)firstIndex, (GLsizei)instanceCount, (GLint)vertexOffset);
    }

    void OpenGLGraphicsContext::End()
    {
        assert(!mEnded);
        mpBoundShader = nullptr;
        FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
        glEndQuery(GL_TIME_ELAPSED);
#endif
        mEnded = true;
    }

#ifdef _DEBUG
    void OpenGLGraphicsContext::RetrieveElapsedTime()
    {
        assert(mEnded);

        // retrieves the result of the previous query

        mCurrentQuery = (mCurrentQuery + 1) % FASTCG_ARRAYSIZE(mTimeElapsedQueries);

        GLint done = 0;
        while (!done)
        {
            glGetQueryObjectiv(mTimeElapsedQueries[mCurrentQuery], GL_QUERY_RESULT_AVAILABLE, &done);
        }

        GLuint64 elapsedTime;
        glGetQueryObjectui64v(mTimeElapsedQueries[mCurrentQuery], GL_QUERY_RESULT, &elapsedTime);

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