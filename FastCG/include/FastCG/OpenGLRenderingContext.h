#ifndef FASTCG_OPENGL_RENDERING_CONTEXT_H
#define FASTCG_OPENGL_RENDERING_CONTEXT_H

#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLTexture.h>
#include <FastCG/OpenGLShader.h>
#include <FastCG/OpenGLBuffer.h>
#include <FastCG/BaseRenderingContext.h>

#include <GL/glew.h>
#include <GL/gl.h>

namespace FastCG
{
    class OpenGLRenderingSystem;

    class OpenGLRenderingContext : public BaseRenderingContext<OpenGLBuffer, OpenGLShader, OpenGLTexture>
    {
    public:
        void Begin();
        void PushDebugMarker(const char *name);
        void PopDebugMarker();
        void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void SetBlend(bool blend);
        void SetBlendFunc(BlendFunc color, BlendFunc alpha);
        void SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha);
        void SetStencilTest(bool stencilTest);
        void SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask);
        void SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass);
        void SetStencilWriteMask(uint32_t mask);
        void SetDepthTest(bool depthTest);
        void SetDepthWrite(bool depthWrite);
        void SetDepthFunc(CompareOp depthFunc);
        void SetScissorTest(bool scissorTest);
        void SetCullMode(Face face);
        void Copy(const OpenGLBuffer *pBuffer, size_t dataSize, const void *pData);
        void Copy(const OpenGLTexture *pTexture, size_t dataSize, const void *pData);
        void Bind(const OpenGLShader *pShader);
        void Bind(const OpenGLBuffer *pBuffer, uint32_t binding);
        void Bind(const OpenGLBuffer *pBuffer, const char *name);
        void Bind(const OpenGLTexture *pTexture, uint32_t binding, uint32_t unit);
        void Bind(const OpenGLTexture *pTexture, const char *name, uint32_t unit);
        void Blit(const OpenGLTexture *pSrc, const OpenGLTexture *pDst);
        void SetRenderTargets(const OpenGLTexture *const *pTextures, size_t textureCount);
        void ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor);
        void ClearDepthStencilTarget(uint32_t renderTargetIndex, float depth, int32_t stencil);
        void ClearDepthTarget(uint32_t renderTargetIndex, float depth);
        void ClearStencilTarget(uint32_t renderTargetIndex, int32_t stencil);
        void SetVertexBuffers(const OpenGLBuffer *const *pBuffers, size_t bufferCount);
        void SetIndexBuffer(const OpenGLBuffer *pBuffer);
        void DrawIndexed(PrimitiveType primitiveType, uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset);
        void End();
        double GetElapsedTime();

    private:
        const OpenGLShader *mpBoundShader{nullptr};
        bool mEnded{true};
#ifdef _DEBUG
        GLuint mTimeElapsedQueries[2]{}; // double buffered
        size_t mCurrentQuery{0};
        double mElapsedTime{0};
#endif

        OpenGLRenderingContext();
        virtual ~OpenGLRenderingContext();

#ifdef _DEBUG
        void RetrieveElapsedTime();
#endif

        friend class OpenGLRenderingSystem;
    };

}

#endif

#endif