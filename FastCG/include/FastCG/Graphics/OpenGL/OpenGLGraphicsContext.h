#ifndef FASTCG_OPENGL_GRAPHICS_CONTEXT_H
#define FASTCG_OPENGL_GRAPHICS_CONTEXT_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/BaseGraphicsContext.h>

#include <unordered_set>
#include <vector>

namespace FastCG
{
        class OpenGLGraphicsSystem;

        class OpenGLGraphicsContext : public BaseGraphicsContext<OpenGLBuffer, OpenGLShader, OpenGLTexture>
        {
        public:
                OpenGLGraphicsContext(const Args &rArgs);
                virtual ~OpenGLGraphicsContext();

                void Begin();
                void PushDebugMarker(const char *pName);
                void PopDebugMarker();
                void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height);
                void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
                void SetBlend(bool blend);
                void SetBlendFunc(BlendFunc color, BlendFunc alpha);
                void SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha);
                void SetStencilTest(bool stencilTest);
                void SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask);
                void SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass);
                void SetStencilWriteMask(Face face, uint32_t mask);
                void SetDepthTest(bool depthTest);
                void SetDepthWrite(bool depthWrite);
                void SetDepthFunc(CompareOp depthFunc);
                void SetScissorTest(bool scissorTest);
                void SetCullMode(Face face);
                void Copy(const OpenGLBuffer *pBuffer, size_t dataSize, const void *pData);
                void Copy(const OpenGLTexture *pTexture, size_t dataSize, const void *pData);
                void BindShader(const OpenGLShader *pShader);
                void BindResource(const OpenGLBuffer *pBuffer, const char *pName);
                void BindResource(const OpenGLTexture *pTexture, const char *pName);
                void Blit(const OpenGLTexture *pSrc, const OpenGLTexture *pDst);
                void SetRenderTargets(const OpenGLTexture *const *pRenderTargets, uint32_t renderTargetCount, const OpenGLTexture *pDepthStencilBuffer);
                void ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor);
                void ClearDepthStencilBuffer(float depth, int32_t stencil);
                void ClearDepthBuffer(float depth);
                void ClearStencilBuffer(int32_t stencil);
                void SetVertexBuffers(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount);
                void SetIndexBuffer(const OpenGLBuffer *pBuffer);
                void DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
                void DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
                void End();
                double GetElapsedTime() const;

        private:
                const OpenGLShader *mpBoundShader{nullptr};
                std::unordered_set<std::string> mResourceUsage;
                std::vector<const OpenGLTexture *> mRenderTargets;
                const OpenGLTexture *mpDepthStencilBuffer;
                bool mEnded{true};
#if !defined FASTCG_DISABLE_GPU_TIMING
                GLuint mTimeElapsedQueries[2]{}; // double buffered
                size_t mCurrentQuery{0};
                double mElapsedTime{0};
                bool mEndedQuery[2]{false, false};
#endif

                void SetupFbo();
                void SetupDraw();
                void BindResource(const OpenGLTexture *pTexture, const OpenGLResourceInfo &rResourceInfo);
#if !defined FASTCG_DISABLE_GPU_TIMING
                void RetrieveElapsedTime();
#endif

                friend class OpenGLGraphicsSystem;
        };

}

#endif

#endif