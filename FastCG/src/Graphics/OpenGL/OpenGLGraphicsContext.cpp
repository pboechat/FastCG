#ifdef FASTCG_OPENGL

#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsContext.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Platform/Application.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>

namespace FastCG
{
    OpenGLGraphicsContext::OpenGLGraphicsContext(const Args &rArgs)
        : BaseGraphicsContext<OpenGLBuffer, OpenGLShader, OpenGLTexture>(rArgs)
    {
    }

    void OpenGLGraphicsContext::OnPostContextCreate()
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        glGenQueries(FASTCG_ARRAYSIZE(mTimeElapsedQueries), mTimeElapsedQueries);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't generate time queries");
#endif
    }

    void OpenGLGraphicsContext::OnPreContextDestroy()
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        glDeleteQueries(FASTCG_ARRAYSIZE(mTimeElapsedQueries), mTimeElapsedQueries);
        std::memset(mTimeElapsedQueries, 0, sizeof(mTimeElapsedQueries));
#endif
    }

    void OpenGLGraphicsContext::Begin()
    {
        assert(mEnded);
        mEnded = false;
#if !defined FASTCG_DISABLE_GPU_TIMING
        mElapsedTimes[OpenGLGraphicsSystem::GetInstance()->GetCurrentFrame()] = 0;
        glBeginQuery(GL_TIME_ELAPSED, mTimeElapsedQueries[OpenGLGraphicsSystem::GetInstance()->GetCurrentFrame()]);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't begin time queries");
#endif
    }

    void OpenGLGraphicsContext::PushDebugMarker(const char *pName)
    {
        FASTCG_UNUSED(pName);
#if _DEBUG
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, pName);
#endif
    }

    void OpenGLGraphicsContext::PopDebugMarker()
    {
#if _DEBUG
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

    void OpenGLGraphicsContext::SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha,
                                                BlendFactor dstAlpha)
    {
        glBlendFuncSeparate(GetOpenGLBlendFactor(srcColor), GetOpenGLBlendFactor(dstColor),
                            GetOpenGLBlendFactor(srcAlpha), GetOpenGLBlendFactor(dstAlpha));
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
        glStencilOpSeparate(GetOpenGLFace(face), GetOpenGLStencilFunc(stencilFail), GetOpenGLStencilFunc(depthFail),
                            GetOpenGLStencilFunc(depthPass));
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

    void OpenGLGraphicsContext::Copy(const OpenGLBuffer *pDst, const void *pSrc, size_t size)
    {
        assert(pDst != nullptr);
        auto target = GetOpenGLTarget(pDst->GetUsage());
        glBindBuffer(target, *pDst);
        glBufferSubData(target, 0, (GLsizeiptr)size, (const GLvoid *)pSrc);
    }

    void OpenGLGraphicsContext::Copy(const OpenGLTexture *pDst, const void *pSrc, size_t size)
    {
        FASTCG_UNUSED(size);
        assert(pDst != nullptr);
        auto target = GetOpenGLTarget(pDst->GetType());
        glBindTexture(target, *pDst);
        glTexSubImage2D(target, 0, 0, 0, (GLsizei)pDst->GetWidth(), (GLsizei)pDst->GetHeight(),
                        GetOpenGLFormat(pDst->GetFormat()), GetOpenGLDataType(pDst->GetFormat()), (const GLvoid *)pSrc);
    }

    void OpenGLGraphicsContext::Copy(void *pDst, const OpenGLBuffer *pSrc, size_t offset, size_t size)
    {
        assert(pDst != nullptr);
        assert(pSrc != nullptr);
        assert(size > 0);

        auto target = GetOpenGLTarget(pSrc->GetUsage());
        glBindBuffer(target, *pSrc);
        void *pMapped = glMapBufferRange(target, (GLintptr)offset, (GLsizeiptr)size, GL_MAP_READ_BIT);
        if (pMapped)
        {
            std::memcpy(pDst, pMapped, size);
            glUnmapBuffer(target);
        }
    }

    void OpenGLGraphicsContext::AddMemoryBarrier()
    {
        // TODO: too broad?
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
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
        mResourceUsage.clear();
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLBuffer *pBuffer, const char *pName)
    {
        assert(pBuffer != nullptr);
        assert(pName != nullptr);
        assert(mpBoundShader != nullptr);
        const auto &rResourceInfo = mpBoundShader->GetResourceInfo(pName);
        if (rResourceInfo.binding == -1)
        {
            return;
        }
        glBindBufferBase(GetOpenGLTarget(pBuffer->GetUsage()), rResourceInfo.binding, *pBuffer);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't bind buffer to resource (buffer: %s, resource: %s, binding: %d)",
                                  pBuffer->GetName().c_str(), pName, rResourceInfo.binding);
        mResourceUsage.emplace(pName);
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLTexture *pTexture, const char *pName)
    {
        assert(mpBoundShader != nullptr);
        const auto &rResourceInfo = mpBoundShader->GetResourceInfo(pName);
        BindResource(pTexture, rResourceInfo);
        mResourceUsage.emplace(pName);
    }

    void OpenGLGraphicsContext::BindResource(const OpenGLTexture *pTexture, const OpenGLResourceInfo &rResourceInfo)
    {
        assert(mpBoundShader != nullptr);
        if (rResourceInfo.location == -1 || rResourceInfo.binding == -1)
        {
            return;
        }
        glActiveTexture(GL_TEXTURE0 + rResourceInfo.binding);
        if (pTexture == nullptr)
        {
            // TODO: check the texture type from shader reflection
            pTexture = OpenGLGraphicsSystem::GetInstance()->GetMissingTexture(TextureType::TEXTURE_2D);
        }
        glBindTexture(GetOpenGLTarget(pTexture->GetType()), *pTexture);
        glUniform1i(rResourceInfo.location, rResourceInfo.binding);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't bind texture to resource (texture: %s, location: %d, binding: %d)",
                                  pTexture->GetName().c_str(), rResourceInfo.location, rResourceInfo.binding);
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
            GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            glDrawBuffers(1, drawBuffers);
            dstWidth = (GLint)pDst->GetWidth();
            dstHeight = (GLint)pDst->GetHeight();
        }
        GLbitfield mask;
        if (IsDepthFormat(pSrc->GetFormat()))
        {
            if (HasStencil(pSrc->GetFormat()))
            {
                mask = GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT;
            }
            else
            {
                mask = GL_DEPTH_BUFFER_BIT;
            }
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

    void OpenGLGraphicsContext::SetRenderTargets(const OpenGLTexture *const *ppRenderTargets,
                                                 uint32_t renderTargetCount, const OpenGLTexture *pDepthStencilBuffer)
    {
        if (renderTargetCount == 1 && ppRenderTargets[0] == OpenGLGraphicsSystem::GetInstance()->GetBackbuffer() &&
            pDepthStencilBuffer == nullptr)
        {
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            return;
        }

        auto fbId = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(ppRenderTargets, renderTargetCount,
                                                                                pDepthStencilBuffer);
        assert(fbId != ~0u);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbId);

        std::vector<GLenum> attachments;
        attachments.reserve(renderTargetCount);
        std::for_each(ppRenderTargets, ppRenderTargets + renderTargetCount,
                      [&attachments, i = 0](const auto *pTexture) mutable {
                          if (IsColorFormat(pTexture->GetFormat()))
                              attachments.emplace_back(GL_COLOR_ATTACHMENT0 + (i++));
                      });
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

    void OpenGLGraphicsContext::SetupDraw()
    {
        assert(mpBoundShader != nullptr);
        for (const auto &rEntry : mpBoundShader->GetResourceInfoMap())
        {
            const auto &rResourceName = rEntry.first;
            if (mResourceUsage.find(rResourceName) != mResourceUsage.end())
            {
                continue;
            }

            const auto &rResourceInfo = rEntry.second;
            // TODO: implement other default missing resources
            if (rResourceInfo.iface == GL_UNIFORM)
            {
                if (rResourceInfo.type == GL_SAMPLER_2D || rResourceInfo.type == GL_IMAGE_2D)
                {
                    BindResource(OpenGLGraphicsSystem::GetInstance()->GetMissingTexture(TextureType::TEXTURE_2D),
                                 rResourceInfo);
                }
            }
        }
    }

    void OpenGLGraphicsContext::DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount,
                                            int32_t vertexOffset)
    {
        SetupDraw();
        glDrawElementsBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT,
                                 (GLvoid *)(uintptr_t)(firstIndex * sizeof(uint32_t)), (GLint)vertexOffset);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't draw");
    }

    void OpenGLGraphicsContext::DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance,
                                                     uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount,
                                                     int32_t vertexOffset)
    {
        FASTCG_UNUSED(firstInstance);
        assert(firstInstance == 0);
        SetupDraw();
        glDrawElementsInstancedBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT,
                                          (GLvoid *)(uintptr_t)(firstIndex * sizeof(uint32_t)), (GLsizei)instanceCount,
                                          (GLint)vertexOffset);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't draw instanced");
    }

    void OpenGLGraphicsContext::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        assert(groupCountX > 0);
        assert(groupCountY > 0);
        assert(groupCountZ > 0);
        glDispatchCompute(groupCountX, groupCountY, groupCountZ);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't dispatch");
    }

    void OpenGLGraphicsContext::End()
    {
        assert(!mEnded);
        mpBoundShader = nullptr;
        FASTCG_CHECK_OPENGL_ERROR("Couldn't end graphics context");

#if !defined FASTCG_DISABLE_GPU_TIMING
        glEndQuery(GL_TIME_ELAPSED);
        FASTCG_CHECK_OPENGL_ERROR("Couldn't end time queries");
        mEndedQuery[OpenGLGraphicsSystem::GetInstance()->GetCurrentFrame()] = true;
#endif
        mEnded = true;
    }

#if !defined FASTCG_DISABLE_GPU_TIMING
    void OpenGLGraphicsContext::RetrieveElapsedTime(uint32_t frame)
    {
        GLuint64 elapsedTime = 0;
        if (mEndedQuery[frame])
        {
            // retrieves the result of the previous query

            GLint done;
            do
            {
                glGetQueryObjectiv(mTimeElapsedQueries[frame], GL_QUERY_RESULT_AVAILABLE, &done);
            } while (!done);

            glGetQueryObjectui64v(mTimeElapsedQueries[frame], GL_QUERY_RESULT, &elapsedTime);
        }
        else
        {
            mElapsedTimes[frame] = 0;
        }

        mElapsedTimes[frame] = elapsedTime * 1e-9;
    }
#endif

    double OpenGLGraphicsContext::GetElapsedTime(uint32_t frame) const
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        return mElapsedTimes[frame];
#else
        return 0;
#endif
    }

}

#endif