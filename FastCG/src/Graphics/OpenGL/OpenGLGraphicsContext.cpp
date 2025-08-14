#ifdef FASTCG_OPENGL

#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/OpenGL/OpenGLErrorHandling.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsContext.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Platform/Application.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>

namespace
{
    struct TemporaryDepthWriteStateChanger
    {
        TemporaryDepthWriteStateChanger(GLboolean newDepthWrite, FastCG::OpenGLGraphicsContext &rContext)
            : mrContext(rContext)
        {
            FASTCG_CHECK_OPENGL_CALL(glGetBooleanv(GL_DEPTH_WRITEMASK, &mOldDepthWrite));
            if (mOldDepthWrite != newDepthWrite)
            {
                mrContext.SetDepthWrite(newDepthWrite);
                mChangedDepthWrite = true;
            }
        }
        ~TemporaryDepthWriteStateChanger()
        {
            if (mChangedDepthWrite)
            {
                mrContext.SetDepthWrite(mOldDepthWrite);
            }
        }

    private:
        GLboolean mOldDepthWrite;
        FastCG::OpenGLGraphicsContext &mrContext;
        bool mChangedDepthWrite{false};
    };

    struct TemporaryFragmentTestStateChanger
    {
        static constexpr uint8_t DEPTH_TEST_MASK = 1 << 0;
        static constexpr uint8_t STENCIL_TEST_MASK = 1 << 1;
        static constexpr uint8_t SCISSOR_TEST_MASK = 1 << 2;

        TemporaryFragmentTestStateChanger(GLboolean newDepthTest, GLboolean newStencilTest, GLboolean newScissorTest,
                                          FastCG::OpenGLGraphicsContext &rContext)
            : mrContext(rContext)
        {
            FASTCG_CHECK_OPENGL_CALL(glGetBooleanv(GL_DEPTH_TEST, &mOldDepthTest));
            if (newDepthTest != mOldDepthTest)
            {
                mrContext.SetDepthTest(newDepthTest);
                mChangeMask |= DEPTH_TEST_MASK;
            }
            FASTCG_CHECK_OPENGL_CALL(glGetBooleanv(GL_STENCIL_TEST, &mOldStencilTest));
            if (newStencilTest != mOldStencilTest)
            {
                mrContext.SetStencilTest(newStencilTest);
                mChangeMask |= STENCIL_TEST_MASK;
            }
            FASTCG_CHECK_OPENGL_CALL(glGetBooleanv(GL_SCISSOR_TEST, &mOldScissorTest));
            if (newScissorTest != mOldScissorTest)
            {
                mrContext.SetScissorTest(newScissorTest);
                mChangeMask |= SCISSOR_TEST_MASK;
            }
        }
        ~TemporaryFragmentTestStateChanger()
        {
            if ((mChangeMask & SCISSOR_TEST_MASK) != 0)
            {
                mrContext.SetScissorTest(mOldScissorTest);
            }
            if ((mChangeMask & STENCIL_TEST_MASK) != 0)
            {
                mrContext.SetStencilTest(mOldStencilTest);
            }
            if ((mChangeMask & DEPTH_TEST_MASK) != 0)
            {
                mrContext.SetDepthTest(mOldDepthTest);
            }
        }

    private:
        GLboolean mOldDepthTest;
        GLboolean mOldStencilTest;
        GLboolean mOldScissorTest;
        FastCG::OpenGLGraphicsContext &mrContext;
        uint8_t mChangeMask{0};
    };
}

namespace FastCG
{
    OpenGLGraphicsContext::OpenGLGraphicsContext(const Args &rArgs)
        : BaseGraphicsContext<OpenGLBuffer, OpenGLShader, OpenGLTexture>(rArgs)
    {
    }

    void OpenGLGraphicsContext::OnPostContextCreate()
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        // create query objects
        FASTCG_CHECK_OPENGL_CALL(glGenQueries(FASTCG_ARRAYSIZE(mTimeElapsedQueries), mTimeElapsedQueries));
#endif
    }

    void OpenGLGraphicsContext::OnPreContextCreate()
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        // clear query objects
        FASTCG_CHECK_OPENGL_CALL(glDeleteQueries(FASTCG_ARRAYSIZE(mTimeElapsedQueries), mTimeElapsedQueries));
        std::memset(mTimeElapsedQueries, 0, sizeof(mTimeElapsedQueries));
#endif
    }

    void OpenGLGraphicsContext::Begin()
    {
        assert(mEnded);
        mEnded = false;
#if !defined FASTCG_DISABLE_GPU_TIMING
        mElapsedTimes[OpenGLGraphicsSystem::GetInstance()->GetCurrentFrame()] = 0;
        FASTCG_CHECK_OPENGL_CALL(
            glBeginQuery(GL_TIME_ELAPSED, mTimeElapsedQueries[OpenGLGraphicsSystem::GetInstance()->GetCurrentFrame()]));
#endif
    }

    void OpenGLGraphicsContext::PushDebugMarker(const char *pName)
    {
        FASTCG_UNUSED(pName);
#if _DEBUG
        FASTCG_CHECK_OPENGL_CALL(glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, pName));
#endif
    }

    void OpenGLGraphicsContext::PopDebugMarker()
    {
#if _DEBUG
        FASTCG_CHECK_OPENGL_CALL(glPopDebugGroup());
#endif
    }

    void OpenGLGraphicsContext::SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        FASTCG_CHECK_OPENGL_CALL(glViewport((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height));
    }

    void OpenGLGraphicsContext::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        FASTCG_CHECK_OPENGL_CALL(glScissor((GLint)x, (GLint)y, (GLsizei)width, (GLsizei)height));
    }

    void OpenGLGraphicsContext::SetBlend(bool blend)
    {
        if (blend)
        {
            FASTCG_CHECK_OPENGL_CALL(glEnable(GL_BLEND));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glDisable(GL_BLEND));
        }
    }

    void OpenGLGraphicsContext::SetBlendFunc(BlendFunc color, BlendFunc alpha)
    {
        assert(color != BlendFunc::NONE);
        if (alpha == BlendFunc::NONE)
        {
            FASTCG_CHECK_OPENGL_CALL(glBlendEquation(GetOpenGLBlendFunc(color)));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glBlendEquationSeparate(GetOpenGLBlendFunc(color), GetOpenGLBlendFunc(alpha)));
        }
    }

    void OpenGLGraphicsContext::SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha,
                                                BlendFactor dstAlpha)
    {
        FASTCG_CHECK_OPENGL_CALL(glBlendFuncSeparate(GetOpenGLBlendFactor(srcColor), GetOpenGLBlendFactor(dstColor),
                                                     GetOpenGLBlendFactor(srcAlpha), GetOpenGLBlendFactor(dstAlpha)));
    }

    void OpenGLGraphicsContext::SetStencilTest(bool stencilTest)
    {
        if (stencilTest)
        {
            FASTCG_CHECK_OPENGL_CALL(glEnable(GL_STENCIL_TEST));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glDisable(GL_STENCIL_TEST));
        }
    }

    void OpenGLGraphicsContext::SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask)
    {
        assert(face != Face::NONE);
        auto compareOp = GetOpenGLCompareOp(stencilFunc);
        if (face == Face::BACK || face == Face::FRONT)
        {
            FASTCG_CHECK_OPENGL_CALL(glStencilFuncSeparate(GetOpenGLFace(face), compareOp, (GLint)ref, (GLuint)mask));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glStencilFunc(compareOp, (GLint)ref, (GLuint)mask));
        }
    }

    void OpenGLGraphicsContext::SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass)
    {
        assert(face != Face::NONE);
        FASTCG_CHECK_OPENGL_CALL(glStencilOpSeparate(GetOpenGLFace(face), GetOpenGLStencilFunc(stencilFail),
                                                     GetOpenGLStencilFunc(depthFail), GetOpenGLStencilFunc(depthPass)));
    }

    void OpenGLGraphicsContext::SetStencilWriteMask(Face face, uint32_t mask)
    {
        assert(face != Face::NONE);
        FASTCG_CHECK_OPENGL_CALL(glStencilMaskSeparate(GetOpenGLFace(face), mask));
    }

    void OpenGLGraphicsContext::SetDepthTest(bool depthTest)
    {
        if (depthTest)
        {
            FASTCG_CHECK_OPENGL_CALL(glEnable(GL_DEPTH_TEST));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glDisable(GL_DEPTH_TEST));
        }
    }

    void OpenGLGraphicsContext::SetDepthWrite(bool depthWrite)
    {
        if (depthWrite)
        {
            FASTCG_CHECK_OPENGL_CALL(glDepthMask(GL_TRUE));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glDepthMask(GL_FALSE));
        }
    }

    void OpenGLGraphicsContext::SetDepthFunc(CompareOp depthFunc)
    {
        FASTCG_CHECK_OPENGL_CALL(glDepthFunc(GetOpenGLCompareOp(depthFunc)));
    }

    void OpenGLGraphicsContext::SetScissorTest(bool scissorTest)
    {
        if (scissorTest)
        {
            FASTCG_CHECK_OPENGL_CALL(glEnable(GL_SCISSOR_TEST));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glDisable(GL_SCISSOR_TEST));
        }
    }

    void OpenGLGraphicsContext::SetCullMode(Face face)
    {
        if (face == Face::NONE)
        {
            FASTCG_CHECK_OPENGL_CALL(glDisable(GL_CULL_FACE));
        }
        else
        {
            FASTCG_CHECK_OPENGL_CALL(glEnable(GL_CULL_FACE));
            FASTCG_CHECK_OPENGL_CALL(glCullFace(GetOpenGLFace(face)));
        }
    }

    void OpenGLGraphicsContext::Copy(const OpenGLBuffer *pDst, const void *pSrc, size_t size)
    {
        assert(pDst != nullptr);
        auto target = GetOpenGLTarget(pDst->GetUsage());
        FASTCG_CHECK_OPENGL_CALL(glBindBuffer(target, *pDst));
        FASTCG_CHECK_OPENGL_CALL(glBufferSubData(target, 0, (GLsizeiptr)size, (const GLvoid *)pSrc));
    }

    void OpenGLGraphicsContext::Copy(const OpenGLTexture *pDst, const void *pSrc, size_t size)
    {
        FASTCG_UNUSED(size);
        assert(pDst != nullptr);
        auto target = GetOpenGLTarget(pDst->GetType());
        FASTCG_CHECK_OPENGL_CALL(glBindTexture(target, *pDst));
        FASTCG_CHECK_OPENGL_CALL(glTexSubImage2D(target, 0, 0, 0, (GLsizei)pDst->GetWidth(), (GLsizei)pDst->GetHeight(),
                                                 GetOpenGLFormat(pDst->GetFormat()),
                                                 GetOpenGLDataType(pDst->GetFormat()), (const GLvoid *)pSrc));
    }

    void OpenGLGraphicsContext::Copy(void *pDst, const OpenGLBuffer *pSrc, size_t offset, size_t size)
    {
        assert(pDst != nullptr);
        assert(pSrc != nullptr);
        assert(size > 0);

        auto target = GetOpenGLTarget(pSrc->GetUsage());
        FASTCG_CHECK_OPENGL_CALL(glBindBuffer(target, *pSrc));
        void *pMapped = glMapBufferRange(target, (GLintptr)offset, (GLsizeiptr)size, GL_MAP_READ_BIT);
        FASTCG_CHECK_OPENGL_ERROR("TODO");
        if (pMapped)
        {
            std::memcpy(pDst, pMapped, size);
            FASTCG_CHECK_OPENGL_CALL(glUnmapBuffer(target));
        }
    }

    void OpenGLGraphicsContext::AddMemoryBarrier()
    {
        // TODO: too broad?
        FASTCG_CHECK_OPENGL_CALL(glMemoryBarrier(GL_ALL_BARRIER_BITS));
    }

    void OpenGLGraphicsContext::BindShader(const OpenGLShader *pShader)
    {
        assert(pShader != nullptr);
        if (mpBoundShader == pShader)
        {
            return;
        }
        FASTCG_CHECK_OPENGL_CALL(glUseProgram(pShader->GetProgramId()));
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
        FASTCG_CHECK_OPENGL_CALL(
            glBindBufferBase(GetOpenGLTarget(pBuffer->GetUsage()), rResourceInfo.binding, *pBuffer));
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
        FASTCG_CHECK_OPENGL_CALL(glActiveTexture(GL_TEXTURE0 + rResourceInfo.binding));
        if (pTexture == nullptr)
        {
            // TODO: check the texture type from shader reflection
            pTexture = OpenGLGraphicsSystem::GetInstance()->GetMissingTexture(TextureType::TEXTURE_2D);
        }
        FASTCG_CHECK_OPENGL_CALL(glBindTexture(GetOpenGLTarget(pTexture->GetType()), *pTexture));
        FASTCG_CHECK_OPENGL_CALL(glUniform1i(rResourceInfo.location, rResourceInfo.binding));
        FASTCG_CHECK_OPENGL_ERROR("Couldn't bind texture to resource (texture: %s, location: %d, binding: %d)",
                                  pTexture->GetName().c_str(), rResourceInfo.location, rResourceInfo.binding);
    }

    void OpenGLGraphicsContext::Blit(const OpenGLTexture *pSrc, const OpenGLTexture *pDst)
    {
        TemporaryFragmentTestStateChanger testStateChanger(false, false, false, *this);
        const auto *pBackbuffer = OpenGLGraphicsSystem::GetInstance()->GetBackbuffer();
        GLint srcWidth, srcHeight;
        if (pSrc == pBackbuffer)
        {
            FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
            srcWidth = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenWidth();
            srcHeight = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenHeight();
        }
        else
        {
            auto readFbo = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(&pSrc, 1, nullptr);
            FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo));
            FASTCG_CHECK_OPENGL_CALL(glReadBuffer(GL_COLOR_ATTACHMENT0));
            srcWidth = (GLint)pSrc->GetWidth();
            srcHeight = (GLint)pSrc->GetHeight();
        }
        GLint dstWidth, dstHeight;
        if (pDst == pBackbuffer)
        {
            FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
            dstWidth = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenWidth();
            dstHeight = (GLint)OpenGLGraphicsSystem::GetInstance()->GetScreenHeight();
        }
        else
        {
            auto drawFbo = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(&pDst, 1, nullptr);
            FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo));
            GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
            FASTCG_CHECK_OPENGL_CALL(glDrawBuffers(1, drawBuffers));
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
        FASTCG_CHECK_OPENGL_CALL(
            glBlitFramebuffer(0, 0, srcWidth, srcHeight, 0, 0, dstWidth, dstHeight, mask, GL_LINEAR));
    }

    void OpenGLGraphicsContext::ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor)
    {
        TemporaryFragmentTestStateChanger tempFragTestState(false, false, false, *this);
        FASTCG_CHECK_OPENGL_CALL(glClearBufferfv(GL_COLOR, (GLint)renderTargetIndex, (const GLfloat *)&rClearColor[0]));
    }

    void OpenGLGraphicsContext::ClearDepthStencilBuffer(float depth, int32_t stencil)
    {
        TemporaryDepthWriteStateChanger tempDepthWriteState(true, *this);
        TemporaryFragmentTestStateChanger tempFragTestState(false, false, false, *this);
        FASTCG_CHECK_OPENGL_CALL(glClearBufferfi(GL_DEPTH_STENCIL, 0, depth, stencil));
    }

    void OpenGLGraphicsContext::ClearDepthBuffer(float depth)
    {
        TemporaryDepthWriteStateChanger tempDepthWriteState(true, *this);
        TemporaryFragmentTestStateChanger tempFragTestState(false, false, false, *this);
        FASTCG_CHECK_OPENGL_CALL(glClearBufferfv(GL_DEPTH, 0, &depth));
    }

    void OpenGLGraphicsContext::ClearStencilBuffer(int32_t stencil)
    {
        TemporaryDepthWriteStateChanger tempDepthWriteState(true, *this);
        TemporaryFragmentTestStateChanger tempFragTestState(false, false, false, *this);
        FASTCG_CHECK_OPENGL_CALL(glClearBufferiv(GL_STENCIL, 0, &stencil));
    }

    void OpenGLGraphicsContext::SetRenderTargets(const OpenGLTexture *const *ppRenderTargets,
                                                 uint32_t renderTargetCount, const OpenGLTexture *pDepthStencilBuffer)
    {
        if (renderTargetCount == 1 && ppRenderTargets[0] == OpenGLGraphicsSystem::GetInstance()->GetBackbuffer() &&
            pDepthStencilBuffer == nullptr)
        {
            FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
            return;
        }

        auto fbId = OpenGLGraphicsSystem::GetInstance()->GetOrCreateFramebuffer(ppRenderTargets, renderTargetCount,
                                                                                pDepthStencilBuffer);
        assert(fbId != ~0u);
        FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbId));

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
            FASTCG_CHECK_OPENGL_CALL(glDrawBuffers((GLsizei)attachments.size(), &attachments[0]));
        }
    }

    void OpenGLGraphicsContext::SetVertexBuffers(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount)
    {
        assert(pBuffers != nullptr);
        assert(mpBoundShader != nullptr);
        assert(bufferCount > 0);
        auto vaoId = OpenGLGraphicsSystem::GetInstance()->GetOrCreateVertexArray(pBuffers, bufferCount);
        assert(vaoId != ~0u);
        FASTCG_CHECK_OPENGL_CALL(glBindVertexArray(vaoId));
    }

    void OpenGLGraphicsContext::SetIndexBuffer(const OpenGLBuffer *pBuffer)
    {
        assert(pBuffer != nullptr);
        assert(mpBoundShader != nullptr);
        auto target = GetOpenGLTarget(pBuffer->GetUsage());
        assert(target == GL_ELEMENT_ARRAY_BUFFER);
        FASTCG_CHECK_OPENGL_CALL(glBindBuffer(target, *pBuffer));
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
        assert(indexCount > 0);
        SetupDraw();
        FASTCG_CHECK_OPENGL_CALL(
            glDrawElementsBaseVertex(GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT,
                                     (GLvoid *)(uintptr_t)(firstIndex * sizeof(uint32_t)), (GLint)vertexOffset));
    }

    void OpenGLGraphicsContext::DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance,
                                                     uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount,
                                                     int32_t vertexOffset)
    {
        FASTCG_UNUSED(firstInstance);
        assert(firstInstance == 0);
        assert(indexCount > 0);
        assert(instanceCount > 0);
        SetupDraw();
        FASTCG_CHECK_OPENGL_CALL(glDrawElementsInstancedBaseVertex(
            GetOpenGLPrimitiveType(primitiveType), (GLsizei)indexCount, GL_UNSIGNED_INT,
            (GLvoid *)(uintptr_t)(firstIndex * sizeof(uint32_t)), (GLsizei)instanceCount, (GLint)vertexOffset));
    }

    void OpenGLGraphicsContext::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        assert(groupCountX > 0);
        assert(groupCountY > 0);
        assert(groupCountZ > 0);
        FASTCG_CHECK_OPENGL_CALL(glDispatchCompute(groupCountX, groupCountY, groupCountZ));
    }

    void OpenGLGraphicsContext::End()
    {
        assert(!mEnded);
        mpBoundShader = nullptr;

#if !defined FASTCG_DISABLE_GPU_TIMING
        FASTCG_CHECK_OPENGL_CALL(glEndQuery(GL_TIME_ELAPSED));
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
                FASTCG_CHECK_OPENGL_CALL(
                    glGetQueryObjectiv(mTimeElapsedQueries[frame], GL_QUERY_RESULT_AVAILABLE, &done));
            } while (!done);

            FASTCG_CHECK_OPENGL_CALL(glGetQueryObjectui64v(mTimeElapsedQueries[frame], GL_QUERY_RESULT, &elapsedTime));
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