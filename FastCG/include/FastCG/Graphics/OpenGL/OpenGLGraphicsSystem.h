#ifndef FASTCG_OPENGL_GRAPHICS_SYSTEM_H
#define FASTCG_OPENGL_GRAPHICS_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsContext.h>
#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/BaseGraphicsSystem.h>
#include <FastCG/Core/System.h>
#include <FastCG/Core/Hash.h>

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>
#include <cassert>

namespace FastCG
{
    class OpenGLGraphicsSystem : public BaseGraphicsSystem<OpenGLBuffer, OpenGLGraphicsContext, OpenGLShader, OpenGLTexture>
    {
        FASTCG_DECLARE_SYSTEM(OpenGLGraphicsSystem, GraphicsSystemArgs);

    public:
        struct DeviceProperties
        {
            GLint maxColorAttachments;
            GLint maxDrawBuffers;
            GLint maxTextureUnits;
        };

        inline uint32_t GetMaxSimultaneousFrames() const
        {
            return 1;
        }
        inline uint32_t GetCurrentFrame() const
        {
            return 0;
        }
        inline const DeviceProperties &GetDeviceProperties() const
        {
            return mDeviceProperties;
        }
        void DestroyTexture(const OpenGLTexture *pTexture);
        GLuint GetOrCreateFramebuffer(const OpenGLTexture *const *pRenderTargets, uint32_t renderTargetCount, const OpenGLTexture *pDepthStencilBuffer);
        GLuint GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount);

    protected:
        OpenGLGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~OpenGLGraphicsSystem();

    private:
#if defined FASTCG_WINDOWS
        HDC mHDC{0};
        HGLRC mHGLRC{0};
#elif defined FASTCG_LINUX
        GLXContext mpRenderContext{nullptr};
#endif
        std::unordered_map<size_t, GLuint, IdentityHasher<size_t>> mFboIds;
        std::unordered_map<GLint, std::vector<size_t>, IdentityHasher<GLint>> mTextureToFboHashes;
        std::unordered_map<size_t, GLuint, IdentityHasher<size_t>> mVaoIds;
        DeviceProperties mDeviceProperties{};

        void OnInitialize() override;
        void OnPostFinalize() override;
        void Resize() {}
        void Present();
        double GetGpuElapsedTime() const;
        void InitializeGlew();
        void CreateOpenGLContext();
        void QueryDeviceProperties();
        void DestroyOpenGLContext();
    };

}

#endif

#endif