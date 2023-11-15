#ifdef FASTCG_OPENGL

#include <FastCG/Platform/Application.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGLExceptions.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Exception.h>
#include <FastCG/Assets/AssetSystem.h>

#if defined FASTCG_LINUX
#include <X11/extensions/Xrender.h>
#endif

#include <stdio.h>
#include <iostream>
#include <algorithm>

namespace
{
#if defined FASTCG_LINUX
    int GLXContextErrorHandler(Display *dpy, XErrorEvent *ev)
    {
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Couldn't create the GLX context (error: %d)", ev->error_code);
        return 0;
    }
#elif defined FASTCG_ANDROID
    inline const char *eglGetErrorString(EGLint error)
    {
#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str) \
    case str:                   \
        return #str

        switch (error)
        {
            CASE_RETURN_STRING(EGL_SUCCESS);
            CASE_RETURN_STRING(EGL_NOT_INITIALIZED);
            CASE_RETURN_STRING(EGL_BAD_ACCESS);
            CASE_RETURN_STRING(EGL_BAD_ALLOC);
            CASE_RETURN_STRING(EGL_BAD_ATTRIBUTE);
            CASE_RETURN_STRING(EGL_BAD_CONTEXT);
            CASE_RETURN_STRING(EGL_BAD_CONFIG);
            CASE_RETURN_STRING(EGL_BAD_CURRENT_SURFACE);
            CASE_RETURN_STRING(EGL_BAD_DISPLAY);
            CASE_RETURN_STRING(EGL_BAD_SURFACE);
            CASE_RETURN_STRING(EGL_BAD_MATCH);
            CASE_RETURN_STRING(EGL_BAD_PARAMETER);
            CASE_RETURN_STRING(EGL_BAD_NATIVE_PIXMAP);
            CASE_RETURN_STRING(EGL_BAD_NATIVE_WINDOW);
            CASE_RETURN_STRING(EGL_CONTEXT_LOST);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "EGL: Unhandled error %d", (int)error);
            return nullptr;
        }

#undef CASE_RETURN_STRING
    }

#ifdef _DEBUG
#define FASTCG_CHECK_EGL_ERROR(fmt, ...)                                                                               \
    {                                                                                                                  \
        EGLint __error;                                                                                                \
        if ((__error = eglGetError()) != EGL_SUCCESS)                                                                  \
        {                                                                                                              \
            char __eglErrorBuffer[4096];                                                                               \
            FASTCG_WARN_PUSH                                                                                           \
            FASTCG_WARN_IGNORE_FORMAT_TRUNCATION                                                                       \
            sprintf(__eglErrorBuffer, fmt, ##__VA_ARGS__);                                                             \
            FASTCG_WARN_POP                                                                                            \
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "%s (error: %s)", __eglErrorBuffer, eglGetErrorString(__error)); \
        }                                                                                                              \
    }
#else
#define FASTCG_CHECK_EGL_ERROR(...)
#endif

    const EGLint EGL_CONTEXT_ATTRIBS[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 2,
        EGL_NONE};

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

#if !defined FASTCG_ANDROID
        InitializeGlew();
#endif

        CreateOpenGLContext();

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLDebugCallback, nullptr);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
#endif

        QueryDeviceProperties();
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

        auto status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            FASTCG_THROW_EXCEPTION(OpenGLException, "Couldn't create FBO: 0x%x\n", status);
        }

        mFboIds.emplace(fboHash, fboId);

        std::for_each(pRenderTargets, pRenderTargets + renderTargetCount, [&](const auto *pRenderTarget)
                      { mTextureToFboHashes[*pRenderTarget].emplace_back(fboHash); });

        if (pDepthStencilBuffer != nullptr)
        {
            mTextureToFboHashes[*pDepthStencilBuffer].emplace_back(fboHash);
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldFboId);

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

#if !defined FASTCG_ANDROID
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
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "WGL: Couldn't set the current pixel format");
        }

        mHGLRC = wglCreateContext(mHDC);
        if (mHGLRC == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't create the temporary WGL context");
        }

        if (!wglMakeCurrent(mHDC, mHGLRC))
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't make the temporary WGL context current");
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
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't initialize Glew: %s", glewGetErrorString(glewInitRes));
        }
    }
#endif

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
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't create the WGL context");
        }

        if (!wglMakeCurrent(mHDC, mHGLRC))
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't make the WGL context current");
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
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't find an appropriate framebuffer configuration");
        }

        auto &rWindow = X11Application::GetInstance()->CreateWindow(pVisualInfo);

        XFree(pVisualInfo);

        glXDestroyContext(pDisplay, mpRenderContext);

        auto *pOldErrorHandler = XSetErrorHandler(&GLXContextErrorHandler);
        mpRenderContext = glXCreateContextAttribsARB(pDisplay, fbConfig, 0, True, attribs);
        XSetErrorHandler(pOldErrorHandler);

        if (mpRenderContext == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't create the GLX context");
        }

        XSync(pDisplay, False);

        if (!glXMakeContextCurrent(pDisplay, rWindow, rWindow, mpRenderContext))
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't make the GLX context current");
        }

        glXSwapIntervalEXT(pDisplay, rWindow, mArgs.vsync ? 1 : 0);
#elif defined FASTCG_ANDROID
        mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (!eglInitialize(mDisplay, 0, 0))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't initialize EGL");
        }

        const EGLint configAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE};

        EGLint numConfigs;
        if (!eglChooseConfig(mDisplay, configAttribs, &mConfig, 1, &numConfigs))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't find an appropriate EGL configuration");
        }

        mHeadlessContext = eglCreateContext(mDisplay, mConfig, EGL_NO_CONTEXT, EGL_CONTEXT_ATTRIBS);
        if (mHeadlessContext == EGL_NO_CONTEXT)
        {
            FASTCG_THROW_EXCEPTION(Exception, "EGL: Couldn't create the headless EGL context");
        }

        const EGLint pbufferAttribs[] = {
            EGL_WIDTH, 1,
            EGL_HEIGHT, 1,
            EGL_NONE};

        mPbufferSurface = eglCreatePbufferSurface(mDisplay, mConfig, pbufferAttribs);
        if (mPbufferSurface == EGL_NO_SURFACE)
        {
            FASTCG_THROW_EXCEPTION(Exception, "EGL: Couldn't create the Pbuffer EGL surface");
        }

        if (!eglMakeCurrent(mDisplay, mPbufferSurface, mPbufferSurface, mHeadlessContext))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't make the headless EGL context current");
        }
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
#elif defined FASTCG_ANDROID
        if (mDisplay != nullptr)
        {
            if (mWindowSurface != EGL_NO_SURFACE)
            {
                eglDestroySurface(mDisplay, mWindowSurface);

                mWindowSurface = EGL_NO_SURFACE;
            }
            if (mHeadedContext != EGL_NO_CONTEXT)
            {
                eglDestroyContext(mDisplay, mHeadedContext);

                mHeadedContext = EGL_NO_CONTEXT;
            }
            if (mPbufferSurface != EGL_NO_SURFACE)
            {
                eglDestroySurface(mDisplay, mPbufferSurface);

                mPbufferSurface = EGL_NO_SURFACE;
            }
            if (mHeadlessContext != EGL_NO_CONTEXT)
            {
                eglDestroyContext(mDisplay, mHeadlessContext);

                mHeadlessContext = EGL_NO_CONTEXT;
            }
            eglTerminate(mDisplay);
        }
#else
#error "FastCG::OpenGLRenderingSystem::DestroyOpenGLContext() is not implemented on the current platform"
#endif
    }

    void OpenGLGraphicsSystem::Present()
    {
#if defined FASTCG_WINDOWS
        SwapBuffers(mHDC);
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        auto &rWindow = X11Application::GetInstance()->GetWindow();
        glXSwapBuffers(pDisplay, rWindow);
#elif defined FASTCG_ANDROID
        if (!IsHeadless())
        {
            eglSwapBuffers(mDisplay, mWindowSurface);
        }
#else
#error "OpenGLRenderingSystem::Present() not implemented on the current platform"
#endif
#if !defined FASTCG_DISABLE_GPU_TIMING
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime();
        }
#endif
    }

    double OpenGLGraphicsSystem::GetGpuElapsedTime() const
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
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

#if defined FASTCG_ANDROID
    void OpenGLGraphicsSystem::OnWindowInitialized()
    {
        assert(mHeadlessContext != EGL_NO_CONTEXT);
        assert(mHeadedContext == EGL_NO_CONTEXT);
        assert(mWindowSurface == EGL_NO_SURFACE);

        auto *pWindow = AndroidApplication::GetInstance()->GetWindow();
        assert(pWindow != nullptr);

        mHeadedContext = eglCreateContext(mDisplay, mConfig, mHeadlessContext, EGL_CONTEXT_ATTRIBS);
        if (mHeadedContext == EGL_NO_CONTEXT)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't create the headed EGL context");
        }

        mWindowSurface = eglCreateWindowSurface(mDisplay, mConfig, pWindow, NULL);
        if (mWindowSurface == EGL_NO_SURFACE)
        {
            FASTCG_CHECK_EGL_ERROR("Couldn't create the EGL window surface");
        }

        if (!eglMakeCurrent(mDisplay, mWindowSurface, mWindowSurface, mHeadedContext))
        {
            FASTCG_CHECK_EGL_ERROR("Couldn't make the headed EGL context current");
        }
    }

    void OpenGLGraphicsSystem::OnWindowTerminated()
    {
        if (mWindowSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(mDisplay, mWindowSurface);

            mWindowSurface = EGL_NO_SURFACE;
        }
        if (mHeadedContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(mDisplay, mHeadedContext);

            mHeadedContext = EGL_NO_CONTEXT;
        }
    }
#endif
}

#endif
