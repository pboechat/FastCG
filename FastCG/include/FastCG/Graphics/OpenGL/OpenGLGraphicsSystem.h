#ifndef FASTCG_OPENGL_GRAPHICS_SYSTEM_H
#define FASTCG_OPENGL_GRAPHICS_SYSTEM_H

#ifdef FASTCG_OPENGL

#include <FastCG/Core/Hash.h>
#include <FastCG/Core/System.h>
#include <FastCG/Graphics/BaseGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGL.h>
#include <FastCG/Graphics/OpenGL/OpenGLBuffer.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsContext.h>
#include <FastCG/Graphics/OpenGL/OpenGLShader.h>
#include <FastCG/Graphics/OpenGL/OpenGLTexture.h>

#include <cassert>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

namespace FastCG
{
    class OpenGLGraphicsSystem
        : public BaseGraphicsSystem<OpenGLBuffer, OpenGLGraphicsContext, OpenGLShader, OpenGLTexture>
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
            return 2; // double-buffering
        }
        inline uint32_t GetPreviousFrame() const
        {
            return mCurrentFrame ^ 1;
        }
        inline uint32_t GetCurrentFrame() const
        {
            return mCurrentFrame;
        }
        inline const DeviceProperties &GetDeviceProperties() const
        {
            return mDeviceProperties;
        }
        OpenGLGraphicsContext *CreateGraphicsContext(const typename OpenGLGraphicsContext::Args &rArgs) override;
        void DestroyTexture(const OpenGLTexture *pTexture) override;
        void Submit();
        void WaitPreviousFrame();
        void OnPostWindowInitialize(void *pWindow);
        void OnPreWindowTerminate(void *pWindow);

    protected:
        OpenGLGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~OpenGLGraphicsSystem();

    private:
#if defined FASTCG_WINDOWS
        HDC mDeviceContext{nullptr};
        HGLRC mContext{nullptr};
        HPBUFFERARB mPbuffer{nullptr};
#elif defined FASTCG_LINUX
        GLXContext mContext{nullptr};
        GLXDrawable mDrawable{0};
#elif defined FASTCG_ANDROID
        EGLDisplay mDisplay{nullptr};
        EGLConfig mConfig{nullptr};
        EGLContext mContext{EGL_NO_CONTEXT};
        EGLSurface mSurface{EGL_NO_SURFACE};
#endif
        std::unordered_map<size_t, GLuint, IdentityHasher<size_t>> mFboIds;
        std::unordered_map<GLint, std::vector<size_t>, IdentityHasher<GLint>> mTextureToFboHashes;
        std::unordered_map<size_t, GLuint, IdentityHasher<size_t>> mVaoIds;
        DeviceProperties mDeviceProperties{};
        GLsync mFrameFences[2]{nullptr, nullptr}; // double-buffered
        uint32_t mCurrentFrame{0};                // 0 or 1 (double-buffering)

        void OnInitialize() override;
        void OnPostFinalize() override;
        void Resize()
        {
        }
        void SwapBuffers() const;
        void SwapFrame();
        double GetGpuElapsedTime(uint32_t frame) const;
        void CreateOpenGLHeadlessContext(void *pWindow = nullptr);
        void CreateOpenGLHeadedContext(void *pWindow);
        void DestroyOpenGLContext(void *pWindow);
        void QueryDeviceProperties();
        GLuint GetOrCreateFramebuffer(const OpenGLTexture *const *pRenderTargets, uint32_t renderTargetCount,
                                      const OpenGLTexture *pDepthStencilBuffer);
        GLuint GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount);
        void NotifyPostContextCreate();
        void NotifyPreContextDestroy();

        friend class OpenGLGraphicsContext;
    };

}

#endif

#endif