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
        inline bool IsHeadless() const
        {
#if defined FASTCG_ANDROID
            return mHeadedContext == EGL_NO_CONTEXT;
#else
            return false;
#endif
        }
        void DestroyTexture(const OpenGLTexture *pTexture) override;
        void Synchronize();
#if defined FASTCG_ANDROID
        void OnWindowInitialized();
        void OnWindowTerminated();
#endif
    protected:
        OpenGLGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~OpenGLGraphicsSystem();

    private:
#if defined FASTCG_WINDOWS
        HDC mHDC{0};
        HGLRC mHGLRC{0};
#elif defined FASTCG_LINUX
        GLXContext mpRenderContext{nullptr};
#elif defined FASTCG_ANDROID
        EGLDisplay mDisplay{nullptr};
        EGLConfig mConfig{nullptr};
        EGLContext mHeadlessContext{EGL_NO_CONTEXT};
        EGLSurface mPbufferSurface{EGL_NO_SURFACE};
        EGLContext mHeadedContext{EGL_NO_CONTEXT};
        EGLSurface mWindowSurface{EGL_NO_SURFACE};
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
#if !defined FASTCG_ANDROID
        void InitializeGlew();
#endif
        void CreateOpenGLContext();
        void QueryDeviceProperties();
        void DestroyOpenGLContext();
        GLuint GetOrCreateFramebuffer(const OpenGLTexture *const *pRenderTargets, uint32_t renderTargetCount, const OpenGLTexture *pDepthStencilBuffer);
        GLuint GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount);

        friend class OpenGLGraphicsContext;
    };

}

#endif

#endif