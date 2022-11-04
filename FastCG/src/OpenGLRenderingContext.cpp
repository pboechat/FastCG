#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLUtils.h>
#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLRenderingContext.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>

#include <GL/glew.h>
#include <GL/gl.h>

#include <vector>
#include <cassert>
#include <algorithm>

namespace FastCG
{
    void OpenGLRenderingContext::Begin()
    {
        FASTCG_CHECK_OPENGL_ERROR();
    }

    void OpenGLRenderingContext::PushDebugMarker(const char *name)
    {
#ifdef _DEBUG
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name);
#endif
    }

    void OpenGLRenderingContext::PopDebugMarker()
    {
#ifdef _DEBUG
        glPopDebugGroup();
#endif
    }

    void OpenGLRenderingContext::SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        glViewport((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height);
    }

    void OpenGLRenderingContext::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        glScissor((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height);
    }

    void OpenGLRenderingContext::SetBlend(bool blend)
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

    void OpenGLRenderingContext::SetBlendFunc(BlendFunc color, BlendFunc alpha)
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

    void OpenGLRenderingContext::SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha)
    {
        glBlendFuncSeparate(GetOpenGLBlendFactor(srcColor), GetOpenGLBlendFactor(dstColor), GetOpenGLBlendFactor(srcAlpha), GetOpenGLBlendFactor(dstAlpha));
    }

    void OpenGLRenderingContext::SetStencilTest(bool stencilTest)
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

    void OpenGLRenderingContext::SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask)
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

    void OpenGLRenderingContext::SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass)
    {
        assert(face != Face::NONE);
        glStencilOpSeparate(GetOpenGLFace(face), GetOpenGLStencilFunc(stencilFail), GetOpenGLStencilFunc(depthFail), GetOpenGLStencilFunc(depthPass));
    }

    void OpenGLRenderingContext::SetStencilWriteMask(uint32_t mask)
    {
        glStencilMask(mask);
    }

    void OpenGLRenderingContext::SetDepthTest(bool depthTest)
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

    void OpenGLRenderingContext::SetDepthWrite(bool depthWrite)
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

    void OpenGLRenderingContext::SetDepthFunc(CompareOp depthFunc)
    {
        glDepthFunc(GetOpenGLCompareOp(depthFunc));
    }

    void OpenGLRenderingContext::SetScissorTest(bool scissorTest)
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

    void OpenGLRenderingContext::SetCullMode(Face face)
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

    void OpenGLRenderingContext::Copy(const OpenGLBuffer *pBuffer, size_t dataSize, const void *pData)
    {
        assert(pBuffer != nullptr);
        auto target = GetOpenGLTarget(pBuffer->GetType());
        glBindBuffer(target, *pBuffer);
        glBufferSubData(target, 0, (GLsizeiptr)dataSize, (const GLvoid *)pData);
    }

    void OpenGLRenderingContext::Copy(const OpenGLTexture *pTexture, size_t dataSize, const void *pData)
    {
        assert(pTexture != nullptr);
        auto target = GetOpenGLTarget(pTexture->GetType());
        glBindTexture(target, *pTexture);
        glTexSubImage2D(target, 0, 0, 0, (GLsizei)pTexture->GetWidth(), (GLsizei)pTexture->GetHeight(), GetOpenGLFormat(pTexture->GetFormat()), GetOpenGLDataType(pTexture->GetDataType(), pTexture->GetBitsPerPixel()), (const GLvoid *)pData);
    }

    void OpenGLRenderingContext::Bind(const OpenGLShader *pShader)
    {
        assert(pShader != nullptr);
        glUseProgram(*pShader);
        mpBoundShader = pShader;
    }

    void OpenGLRenderingContext::Bind(const OpenGLBuffer *pBuffer, uint32_t binding)
    {
        assert(pBuffer != nullptr);
        assert(mpBoundShader != nullptr);
        glBindBufferBase(GetOpenGLTarget(pBuffer->GetType()), binding, *pBuffer);
    }

    void OpenGLRenderingContext::Bind(const OpenGLBuffer *pBuffer, const char *name)
    {
        assert(pBuffer != nullptr);
        assert(mpBoundShader != nullptr);
        auto binding = glGetUniformLocation(*mpBoundShader, name);
        Bind(pBuffer, binding);
    }

    void OpenGLRenderingContext::Bind(const OpenGLTexture *pTexture, uint32_t binding, uint32_t unit)
    {
        assert(pTexture != nullptr);
        assert(mpBoundShader != nullptr);
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, *pTexture);
        glUniform1i(binding, unit);
    }

    void OpenGLRenderingContext::Bind(const OpenGLTexture *pTexture, const char *name, uint32_t unit)
    {
        assert(mpBoundShader != nullptr);
        auto binding = glGetUniformLocation(*mpBoundShader, name);
        Bind(pTexture, binding, unit);
    }

    void OpenGLRenderingContext::Blit(const OpenGLTexture *pSrc, const OpenGLTexture *pDst)
    {
        auto *pRenderingSystem = OpenGLRenderingSystem::GetInstance();
        const auto *pBackbuffer = pRenderingSystem->GetBackbuffer();
        GLint srcWidth, srcHeight;
        if (pSrc == pBackbuffer)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            srcWidth = (GLint)pRenderingSystem->GetScreenWidth();
            srcHeight = (GLint)pRenderingSystem->GetScreenHeight();
        }
        else
        {
            auto readFbo = pRenderingSystem->GetOrCreateFramebuffer(&pSrc, 1);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            srcWidth = (GLint)pSrc->GetWidth();
            srcHeight = (GLint)pSrc->GetHeight();
        }
        GLint dstWidth, dstHeight;
        if (pDst == pBackbuffer)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            dstWidth = (GLint)pRenderingSystem->GetScreenWidth();
            dstHeight = (GLint)pRenderingSystem->GetScreenHeight();
        }
        else
        {
            auto drawFbo = pRenderingSystem->GetOrCreateFramebuffer(&pDst, 1);
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

    void OpenGLRenderingContext::ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor)
    {
        glClearBufferfv(GL_COLOR, (GLint)renderTargetIndex, (const GLfloat *)&rClearColor[0]);
    }

    void OpenGLRenderingContext::ClearDepthStencilTarget(uint32_t renderTargetIndex, float depth, int32_t stencil)
    {
        GLboolean depthWrite;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
        GLint stencilWriteMask;
        glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilWriteMask);
        SetDepthWrite(true);
        SetStencilWriteMask(0xff);
        glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil);
        SetDepthWrite(depthWrite);
        SetStencilWriteMask(stencilWriteMask);
    }

    void OpenGLRenderingContext::ClearDepthTarget(uint32_t renderTargetIndex, float depth)
    {
        GLboolean depthTest;
        glGetBooleanv(GL_DEPTH_TEST, &depthTest);
        SetDepthTest(true);
        glClearBufferfv(GL_DEPTH, 0, &depth);
        SetDepthTest(depthTest);
    }

    void OpenGLRenderingContext::ClearStencilTarget(uint32_t renderTargetIndex, int32_t stencil)
    {
        GLint stencilMask;
        glGetIntegerv(GL_STENCIL_WRITEMASK, &stencilMask);
        SetStencilWriteMask(0xff);
        glClearBufferiv(GL_STENCIL, 0, &stencil);
        SetStencilWriteMask(stencilMask);
    }

    void OpenGLRenderingContext::SetRenderTargets(const OpenGLTexture *const *pTextures, size_t textureCount)
    {
        assert(pTextures != nullptr);
        assert(textureCount > 0);
        if (textureCount == 1 && pTextures[0] == OpenGLRenderingSystem::GetInstance()->GetBackbuffer())
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            return;
        }
        auto fbId = OpenGLRenderingSystem::GetInstance()->GetOrCreateFramebuffer(pTextures, textureCount);
        assert(fbId != ~0u);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbId);
        std::vector<GLenum> attachments;
        attachments.reserve(textureCount);
        std::for_each(pTextures, pTextures + textureCount, [&attachments, i = 0](const auto *pTexture) mutable
                      { if (pTexture->GetFormat() != TextureFormat::DEPTH_STENCIL) attachments.emplace_back(GL_COLOR_ATTACHMENT0 + (i++)); });
        if (!attachments.empty())
        {
            glDrawBuffers((GLsizei)attachments.size(), &attachments[0]);
        }
    }

    void OpenGLRenderingContext::SetVertexBuffers(const OpenGLBuffer *const *pBuffers, size_t bufferCount)
    {
        assert(pBuffers != nullptr);
        assert(mpBoundShader != nullptr);
        assert(bufferCount > 0);
        auto vaoId = OpenGLRenderingSystem::GetInstance()->GetOrCreateVertexArray(pBuffers, bufferCount);
        assert(vaoId != ~0u);
        glBindVertexArray(vaoId);
    }

    void OpenGLRenderingContext::SetIndexBuffer(const OpenGLBuffer *pBuffer)
    {
        assert(pBuffer != nullptr);
        assert(mpBoundShader != nullptr);
        auto target = GetOpenGLTarget(pBuffer->GetType());
        assert(target == GL_ELEMENT_ARRAY_BUFFER);
        glBindBuffer(target, *pBuffer);
    }

    void OpenGLRenderingContext::DrawIndexed(PrimitiveType primitiveType, uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset)
    {
        glDrawElementsBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT, (void *)(uintptr_t)firstIndex, (GLint)vertexOffset);
    }

    void OpenGLRenderingContext::End()
    {
        mpBoundShader = nullptr;
        FASTCG_CHECK_OPENGL_ERROR();
    }

}

#endif