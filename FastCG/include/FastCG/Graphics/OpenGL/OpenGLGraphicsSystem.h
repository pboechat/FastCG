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

#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>
#include <algorithm>

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
        };

        void DestroyTexture(const OpenGLTexture *pTexture);
        GLuint GetOrCreateFramebuffer(const OpenGLTexture *const *pTextures, size_t textureCount);
        GLuint GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, size_t bufferCount);
        inline const DeviceProperties &GetDeviceProperties() const
        {
            return mDeviceProperties;
        }

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
        std::unordered_map<uint32_t, GLuint> mFboIds;
        std::unordered_map<GLint, std::vector<GLuint>> mTextureToFboHashes;
        std::unordered_map<uint32_t, GLuint> mVaoIds;
#ifdef _DEBUG
        GLuint mPresentTimestampQuery{~0u};
        double mPresentElapsedTime{0};
#endif
        DeviceProperties mDeviceProperties{};

        void OnInitialize() override;
        void OnFinalize() override;
        void Resize() {}
        void Present();
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;
        void InitializeGlew();
        void CreateOpenGLContext();
        void InitializeDeviceProperties();
        void DestroyOpenGLContext();
    };

}

#endif

#endif