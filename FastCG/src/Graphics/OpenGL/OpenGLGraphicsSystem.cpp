#ifdef FASTCG_OPENGL

#if defined FASTCG_WINDOWS
#include <FastCG/Platform/Windows/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/Platform/Linux/X11Application.h>
#endif
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Exception.h>
#include <FastCG/Assets/AssetSystem.h>

#if defined FASTCG_LINUX
#include <X11/extensions/Xrender.h>
#endif

#include <iostream>
#include <cassert>
#include <algorithm>

namespace
{
#ifdef FASTCG_LINUX
    int GLXContextErrorHandler(Display *dpy, XErrorEvent *ev)
    {
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Error creating GLX context (error_code: %d)", ev->error_code);
        return 0;
    }
#endif

    void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid *userParam)
    {
        std::cout << "[OPENGL]"
                  << " - " << FastCG::GetOpenGLDebugOutputMessageSeverity(severity)
                  << " - " << FastCG::GetOpenGLDebugOutputMessageSourceString(source)
                  << " - " << FastCG::GetOpenGLDebugOutputMessageTypeString(type)
                  << " - " << id
                  << " - " << message
                  << std::endl;
    }

}

namespace FastCG
{
    OpenGLGraphicsSystem::OpenGLGraphicsSystem(const GraphicsSystemArgs &rArgs) : BaseGraphicsSystem(rArgs)
    {
    }

    OpenGLGraphicsSystem::~OpenGLGraphicsSystem() = default;

    void OpenGLGraphicsSystem::OnInitialize()
    {
        BaseGraphicsSystem::OnInitialize();

        InitializeGlew();

        CreateOpenGLContext();

        QueryDeviceProperties();

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallbackARB(OpenGLDebugCallback, nullptr);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
#endif

#ifdef _DEBUG
        glGenQueries(1, &mPresentTimestampQuery);
#endif
    }

    void OpenGLGraphicsSystem::OnPostFinalize()
    {
        for (const auto &rKvp : mFboIds)
        {
            glDeleteFramebuffers(1, &rKvp.second);
        }
        mFboIds.clear();

        for (const auto &rKvp : mVaoIds)
        {
            glDeleteVertexArrays(1, &rKvp.second);
        }
        mVaoIds.clear();

#ifdef _DEBUG
        glDeleteQueries(1, &mPresentTimestampQuery);
#endif

        DestroyOpenGLContext();

        BaseGraphicsSystem::OnPostFinalize();
    }

#define DECLARE_DESTROY_METHOD(className, containerMember)                                                              \
    void OpenGLGraphicsSystem::Destroy##className(const OpenGL##className *p##className)                                \
    {                                                                                                                   \
        assert(p##className != nullptr);                                                                                \
        auto it = std::find(containerMember.cbegin(), containerMember.cend(), p##className);                            \
        if (it != containerMember.cend())                                                                               \
        {                                                                                                               \
            containerMember.erase(it);                                                                                  \
            delete p##className;                                                                                        \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy " #className " '%s'", p##className->GetName().c_str()); \
        }                                                                                                               \
    }

    void OpenGLGraphicsSystem::DestroyTexture(const OpenGLTexture *pTexture)
    {
        assert(pTexture != nullptr);
        // Delete fbos that reference the texture to be deleted
        {
            auto it = mTextureToFboHashes.find(*pTexture);
            if (it != mTextureToFboHashes.end())
            {
                for (auto fboHash : it->second)
                {
                    if (mFboIds.find(fboHash) != mFboIds.end())
                    {
                        glDeleteFramebuffers(1, &mFboIds[fboHash]);
                        mFboIds.erase(fboHash);
                    }
                }
            }
        }
        BaseGraphicsSystem::DestroyTexture(pTexture);
    }

    GLuint OpenGLGraphicsSystem::GetOrCreateFramebuffer(const OpenGLTexture *const *pRenderTargets, uint32_t renderTargetCount, const OpenGLTexture *pDepthStencilBuffer)
    {
        size_t fboHash;
        {
            auto size = (renderTargetCount + 1) * sizeof(OpenGLTexture *);
            auto *pData = (const OpenGLTexture **)alloca(size);
            for (uint32_t i = 0; i < renderTargetCount; ++i)
            {
                pData[i] = pRenderTargets[i];
            }
            pData[renderTargetCount] = pDepthStencilBuffer;
            fboHash = (size_t)FNV1a((const uint8_t *)pData, size);
        }
        auto it = mFboIds.find(fboHash);
        if (it != mFboIds.end())
        {
            return it->second;
        }

        GLint oldFboId;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldFboId);

        GLuint fboId;
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboId);
#ifdef _DEBUG
        {
            auto framebufferLabel = std::string("FBO ") + std::to_string(mFboIds.size()) + " (GL_FRAMEBUFFER)";
            glObjectLabel(GL_FRAMEBUFFER, fboId, (GLsizei)framebufferLabel.size(), framebufferLabel.c_str());
        }
#endif
        std::for_each(pRenderTargets, pRenderTargets + renderTargetCount, [i = 0](const auto *pRenderTarget) mutable
                      { glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (i++), GetOpenGLTarget(pRenderTarget->GetType()), *pRenderTarget, 0); });

        if (pDepthStencilBuffer != nullptr)
        {
            GLenum attachment;
            if (pDepthStencilBuffer->GetFormat() == TextureFormat::DEPTH)
            {
                attachment = GL_DEPTH_ATTACHMENT;
            }
            else
            {
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GetOpenGLTarget(pDepthStencilBuffer->GetType()), *pDepthStencilBuffer, 0);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldFboId);

        auto status = glCheckNamedFramebufferStatusEXT(fboId, GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            FASTCG_THROW_EXCEPTION(OpenGLException, "Error creating FBO: 0x%x\n", status);
        }

        mFboIds.emplace(fboHash, fboId);

        std::for_each(pRenderTargets, pRenderTargets + renderTargetCount, [&](const auto *pRenderTarget)
                      { mTextureToFboHashes[*pRenderTarget].emplace_back(fboHash); });

        if (pDepthStencilBuffer != nullptr)
        {
            mTextureToFboHashes[*pDepthStencilBuffer].emplace_back(fboHash);
        }

        return fboId;
    }

    GLuint OpenGLGraphicsSystem::GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount)
    {
        assert(bufferCount > 0);
        assert(std::all_of(pBuffers, pBuffers + bufferCount, [](const auto *pBuffer)
                           { 
                            assert(pBuffer != nullptr); 
                            const auto& rVbDescs = pBuffer->GetVertexBindingDescriptors(); 
                            return (pBuffer->GetUsage() & BufferUsageFlagBit::VERTEX_BUFFER) != 0 && 
                                std::all_of(rVbDescs.cbegin(), rVbDescs.cend(), [](const auto& rVbDesc) { return rVbDesc.IsValid(); }); }));

        auto vaoHash = (size_t)FNV1a(reinterpret_cast<const uint8_t *>(pBuffers), bufferCount * sizeof(OpenGLBuffer *));

        auto it = mVaoIds.find(vaoHash);
        if (it != mVaoIds.end())
        {
            return it->second;
        }

        GLuint vaoId;
        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);
#ifdef _DEBUG
        {
            auto vertexArrayLabel = std::string("VAO ") + std::to_string(mVaoIds.size()) + " (GL_VERTEX_ARRAY)";
            glObjectLabel(GL_VERTEX_ARRAY, vaoId, (GLsizei)vertexArrayLabel.size(), vertexArrayLabel.c_str());
        }
#endif

        std::for_each(pBuffers, pBuffers + bufferCount, [](const auto *pBuffer)
                      {
                        glBindBuffer(GetOpenGLTarget(pBuffer->GetUsage()), *pBuffer);
            for (const auto &rVbDesc : pBuffer->GetVertexBindingDescriptors())
            {
                glEnableVertexAttribArray(rVbDesc.binding);
                glVertexAttribPointer(rVbDesc.binding, rVbDesc.size, GetOpenGLType(rVbDesc.type), (GLboolean)rVbDesc.normalized, rVbDesc.stride, (const GLvoid*)(uintptr_t)rVbDesc.offset);
            } });

        mVaoIds.emplace(vaoHash, vaoId);

        return vaoId;
    }

    void OpenGLGraphicsSystem::InitializeGlew()
    {
#if defined FASTCG_WINDOWS
        auto hWnd = WindowsApplication::GetInstance()->GetWindow();
        assert(hWnd != 0);

        mHDC = GetDC(hWnd);
        assert(mHDC != 0);

        PIXELFORMATDESCRIPTOR pixelFormatDescr =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA,
                32,                     // color bits
                0, 0, 0, 0, 0, 0, 0, 0, // per-channel color bits and shifts (RGBA)
                0,                      // accum bits
                0, 0, 0, 0,             // per-channel accum bits
                0,                      // depth bits
                0,                      // stencil bits
                0,                      // auxiliary buffers
                PFD_MAIN_PLANE,         // layer type
                0,                      // reserved
                0, 0, 0,                // layer mask, visible mask, damage mask
            };
        auto pixelFormat = ChoosePixelFormat(mHDC, &pixelFormatDescr);
        if (!SetPixelFormat(mHDC, pixelFormat, &pixelFormatDescr))
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Error setting current pixel format");
        }

        mHGLRC = wglCreateContext(mHDC);
        if (mHGLRC == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error creating a temporary WGL context");
        }

        if (!wglMakeCurrent(mHDC, mHGLRC))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error making a temporary WGL context current");
        }
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);

        auto &rWindow = X11Application::GetInstance()->CreateSimpleWindow();

        XWindowAttributes windowAttribs;
        XGetWindowAttributes(pDisplay, rWindow, &windowAttribs);

        XVisualInfo visualInfoTemplate;
        visualInfoTemplate.visualid = XVisualIDFromVisual(windowAttribs.visual);

        int numItems;
        auto *pVisualInfo = XGetVisualInfo(pDisplay, VisualIDMask, &visualInfoTemplate, &numItems);

        auto *pOldErrorHandler = XSetErrorHandler(&GLXContextErrorHandler);
        mpRenderContext = glXCreateContext(pDisplay, pVisualInfo, 0, true);
        XSetErrorHandler(pOldErrorHandler);

        glXMakeCurrent(pDisplay, rWindow, mpRenderContext);
#endif

        GLenum glewInitRes;
        if ((glewInitRes = glewInit()) != GLEW_OK)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error intializing Glew: %s", glewGetErrorString(glewInitRes));
        }
    }

    void OpenGLGraphicsSystem::CreateOpenGLContext()
    {
#if defined FASTCG_WINDOWS
        const int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef _DEBUG
                                       | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
            ,
            0};

        auto oldHGLRC = mHGLRC;

        mHGLRC = wglCreateContextAttribsARB(mHDC, mHGLRC, attribs);
        if (mHGLRC == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error creating the WGL context");
        }

        if (!wglMakeCurrent(mHDC, mHGLRC))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error making the WGL context current");
        }

        wglDeleteContext(oldHGLRC);

        wglSwapIntervalEXT(mArgs.vsync ? 1 : 0);
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);

        const int attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
            GLX_CONTEXT_MINOR_VERSION_ARB, 3,
            GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef _DEBUG
                                       | GLX_CONTEXT_DEBUG_BIT_ARB
#endif
            ,
            0};

        int dummy;
        if (!glXQueryExtension(pDisplay, &dummy, &dummy))
        {
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL not supported by X server");
        }

        const int fbAttribs[] = {
            GLX_RENDER_TYPE, GLX_RGBA_BIT,
            GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
            GLX_DOUBLEBUFFER, True,
            GLX_RED_SIZE, 8,
            GLX_GREEN_SIZE, 8,
            GLX_BLUE_SIZE, 8,
            GLX_ALPHA_SIZE, 8,
            GLX_DEPTH_SIZE, 0,
            None};

        auto defaultScreen = DefaultScreen(pDisplay);

        int numFbConfigs;
        auto fbConfigs = glXChooseFBConfig(pDisplay, defaultScreen, fbAttribs, &numFbConfigs);

        GLXFBConfig fbConfig;
        XVisualInfo *pVisualInfo;
        for (int i = 0; i < numFbConfigs; i++)
        {
            auto *pCurrVisualInfo = (XVisualInfo *)glXGetVisualFromFBConfig(pDisplay, fbConfigs[i]);
            if (pCurrVisualInfo == nullptr)
            {
                continue;
            }

            auto *pVisualFormat = XRenderFindVisualFormat(pDisplay, pCurrVisualInfo->visual);
            if (pVisualFormat == nullptr)
            {
                continue;
            }

            if (pVisualFormat->direct.alphaMask == 0)
            {
                fbConfig = fbConfigs[i];
                pVisualInfo = pCurrVisualInfo;
                break;
            }

            XFree(pCurrVisualInfo);
        }

        if (fbConfig == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "No matching framebuffer config");
        }

        auto &rWindow = X11Application::GetInstance()->CreateWindow(pVisualInfo);

        XFree(pVisualInfo);

        glXDestroyContext(pDisplay, mpRenderContext);

        auto *pOldErrorHandler = XSetErrorHandler(&GLXContextErrorHandler);
        mpRenderContext = glXCreateContextAttribsARB(pDisplay, fbConfig, 0, True, attribs);
        XSetErrorHandler(pOldErrorHandler);

        if (mpRenderContext == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error creating the GLX context");
        }

        XSync(pDisplay, False);

        if (!glXMakeContextCurrent(pDisplay, rWindow, rWindow, mpRenderContext))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error making the GLX context current");
        }

        glXSwapIntervalEXT(pDisplay, rWindow, mArgs.vsync ? 1 : 0);
#else
#error FastCG::OpenGLRenderingSystem::CreateOpenGLContext() is not implemented on the current platform
#endif
    }

    void OpenGLGraphicsSystem::QueryDeviceProperties()
    {
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mDeviceProperties.maxColorAttachments);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mDeviceProperties.maxDrawBuffers);
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mDeviceProperties.maxTextureUnits);
    }

    void OpenGLGraphicsSystem::DestroyOpenGLContext()
    {
#if defined FASTCG_WINDOWS
        if (mHGLRC != 0)
        {
            wglMakeCurrent(mHDC, nullptr);
            wglDeleteContext(mHGLRC);
            mHGLRC = 0;
        }

        if (mHDC != 0)
        {
            auto hWnd = WindowsApplication::GetInstance()->GetWindow();
            assert(hWnd != 0);

            ReleaseDC(hWnd, mHDC);
            mHDC = 0;
        }
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);

        if (mpRenderContext != nullptr)
        {
            glXMakeContextCurrent(pDisplay, None, None, nullptr);
            glXDestroyContext(pDisplay, mpRenderContext);

            mpRenderContext = nullptr;
        }
#else
#error "FastCG::OpenGLRenderingSystem::DestroyOpenGLContext() is not implemented on the current platform"
#endif
    }

    void OpenGLGraphicsSystem::Present()
    {
#ifdef _DEBUG
        GLint64 presentStart;
        glQueryCounter(mPresentTimestampQuery, GL_TIMESTAMP);
        glGetInteger64v(GL_TIMESTAMP, &presentStart);
#endif
#if defined FASTCG_WINDOWS
        SwapBuffers(mHDC);
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        auto &rWindow = X11Application::GetInstance()->GetWindow();
        glXSwapBuffers(pDisplay, rWindow);
#else
#error "OpenGLRenderingSystem::Present() not implemented on the current platform"
#endif
#ifdef _DEBUG
        GLint done = 0;
        while (!done)
        {
            glGetQueryObjectiv(mPresentTimestampQuery, GL_QUERY_RESULT_AVAILABLE, &done);
        }

        GLuint64 presentEnd;
        glGetQueryObjectui64v(mPresentTimestampQuery, GL_QUERY_RESULT, &presentEnd);

        mPresentElapsedTime = (presentEnd - (GLuint64)presentStart) * 1e-9;

        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime();
        }
#endif
    }

    double OpenGLGraphicsSystem::GetPresentElapsedTime() const
    {
#ifdef _DEBUG
        return mPresentElapsedTime;
#else
        return 0;
#endif
    }

    double OpenGLGraphicsSystem::GetGpuElapsedTime() const
    {
#ifdef _DEBUG
        double elapsedTime = 0;
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            elapsedTime += pGraphicsContext->GetElapsedTime();
        }
        return elapsedTime;
#else
        return 0;
#endif
    }
}

#endif
