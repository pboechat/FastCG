#ifdef FASTCG_OPENGL

#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/Core/Exception.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Graphics/OpenGL/OpenGLErrorHandling.h>
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
#include <FastCG/Graphics/OpenGL/OpenGLUtils.h>
#include <FastCG/Platform/Application.h>

#if defined FASTCG_WINDOWS
#include <windows.h>
#elif defined FASTCG_LINUX
#include <X11/extensions/Xrender.h>
#endif

#include <algorithm>
#include <iostream>
#include <stdio.h>

namespace
{
#if defined FASTCG_LINUX
    int GLXContextErrorHandler(Display *pDisplay, XErrorEvent *pErrorEvent)
    {
        assert(pErrorEvent != nullptr);
        FASTCG_UNUSED(pDisplay);
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "GLX: Couldn't create a context (error: %d)",
                               pErrorEvent->error_code);
        return 0;
    }

    GLXContext CreateGLXContextAndMakeCurrent(Display *pDisplay, GLXFBConfig fbConfig, GLXDrawable drawable,
                                              GLXContext oldContext = nullptr)
    {
        assert(pDisplay != nullptr);
        assert(fbConfig != nullptr);
        assert(drawable > 0);

        if (!GLXEW_ARB_create_context)
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "GLX: GLX_ARB_create_context extension not supported");
        }

        const int contextAttribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB,
            4,
            GLX_CONTEXT_MINOR_VERSION_ARB,
            3,
            GLX_CONTEXT_PROFILE_MASK_ARB,
            GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
            GLX_CONTEXT_FLAGS_ARB,
            GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if _DEBUG
                | GLX_CONTEXT_DEBUG_BIT_ARB
#endif
            ,
            0
        };

        auto *pOldErrorHandler = XSetErrorHandler(&GLXContextErrorHandler);
        auto context = glXCreateContextAttribsARB(pDisplay, fbConfig, oldContext, True, contextAttribs);
        XSetErrorHandler(pOldErrorHandler);

        if (context == nullptr)
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "GLX: Couldn't create context");
        }

        if (!glXMakeContextCurrent(pDisplay, drawable, drawable, context))
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "GLX: Couldn't make context current");
        }

        return context;
    }
#elif defined FASTCG_ANDROID
    inline const char *eglGetErrorString(EGLint error)
    {
#ifdef CASE_RETURN_STRING
#undef CASE_RETURN_STRING
#endif

#define CASE_RETURN_STRING(str)                                                                                        \
    case str:                                                                                                          \
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

#if _DEBUG
#define FASTCG_CHECK_EGL_ERROR(fmt, ...)                                                                               \
    {                                                                                                                  \
        EGLint __error;                                                                                                \
        if ((__error = eglGetError()) != EGL_SUCCESS)                                                                  \
        {                                                                                                              \
            char __eglErrorBuffer[4096];                                                                               \
            FASTCG_COMPILER_WARN_PUSH                                                                                  \
            FASTCG_COMPILER_WARN_IGNORE_FORMAT_TRUNCATION                                                              \
            sprintf(__eglErrorBuffer, fmt, ##__VA_ARGS__);                                                             \
            FASTCG_COMPILER_WARN_POP                                                                                   \
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "EGL: %s (error: %s)", __eglErrorBuffer,                         \
                                   eglGetErrorString(__error));                                                        \
        }                                                                                                              \
    }
#else
#define FASTCG_CHECK_EGL_ERROR(...)
#endif

    const EGLint EGL_CONTEXT_ATTRIBS[] = {EGL_CONTEXT_MAJOR_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 2, EGL_NONE};

#elif defined FASTCG_WINDOWS
    HGLRC CreateWGLContextAndMakeCurrent(HDC deviceContext, HGLRC parentContext)
    {
        if (!WGLEW_ARB_create_context)
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "WGL: WGL_ARB_create_context extension not supported");
        }

        const int contextAttribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB,
            4,
            WGL_CONTEXT_MINOR_VERSION_ARB,
            3,
            WGL_CONTEXT_PROFILE_MASK_ARB,
            WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB,
            WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if _DEBUG
                | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
            ,
            0
        };

        auto context = wglCreateContextAttribsARB(deviceContext, parentContext, contextAttribs);
        if (context == nullptr)
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "WGL: Couldn't create a context");
        }

        if (!wglMakeCurrent(deviceContext, context))
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "WGL: Couldn't make the context current");
        }

        return context;
    }
#endif

    void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                             const GLchar *message, const GLvoid *userParam)
    {
        FASTCG_UNUSED(source);
        FASTCG_UNUSED(type);
        FASTCG_UNUSED(id);
        FASTCG_UNUSED(severity);
        FASTCG_UNUSED(length);
        FASTCG_UNUSED(message);
        FASTCG_UNUSED(userParam);
        FASTCG_LOG_DEBUG(OpenGLGraphicsSystem, "%s - %s - %s - %u - %s",
                         FastCG::GetOpenGLDebugOutputMessageSeverity(severity),
                         FastCG::GetOpenGLDebugOutputMessageSourceString(source),
                         FastCG::GetOpenGLDebugOutputMessageTypeString(type), id, message);
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

#if defined FASTCG_ANDROID
        mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (!eglInitialize(mDisplay, 0, 0))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't initialize API");
        }
#endif

        CreateOpenGLHeadlessContext();

#if _DEBUG
        FASTCG_CHECK_OPENGL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
        FASTCG_CHECK_OPENGL_CALL(glDebugMessageCallback(OpenGLDebugCallback, nullptr));
        FASTCG_CHECK_OPENGL_CALL(glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP,
                                                       GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false));
        FASTCG_CHECK_OPENGL_CALL(glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_POP_GROUP,
                                                       GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false));
        FASTCG_CHECK_OPENGL_CALL(
            glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 0, nullptr, false));
#if !defined FASTCG_ENABLE_GPU_PERF_HINTS
        FASTCG_CHECK_OPENGL_CALL(
            glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, 0, nullptr, false));
#endif
#endif

        QueryDeviceProperties();
    }

#define DECLARE_DESTROY_METHOD(className, containerMember)                                                             \
    void OpenGLGraphicsSystem::Destroy##className(const OpenGL##className *p##className)                               \
    {                                                                                                                  \
        assert(p##className != nullptr);                                                                               \
        auto it = std::find(containerMember.cbegin(), containerMember.cend(), p##className);                           \
        if (it != containerMember.cend())                                                                              \
        {                                                                                                              \
            containerMember.erase(it);                                                                                 \
            delete p##className;                                                                                       \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Couldn't destroy " #className " '%s'",                          \
                                   p##className->GetName().c_str());                                                   \
        }                                                                                                              \
    }

    OpenGLGraphicsContext *OpenGLGraphicsSystem::CreateGraphicsContext(
        const typename OpenGLGraphicsContext::Args &rArgs)
    {
        auto *pGraphicsContext = BaseGraphicsSystem::CreateGraphicsContext(rArgs);
        pGraphicsContext->OnPostContextCreate();
        return pGraphicsContext;
    }

    void OpenGLGraphicsSystem::DestroyTexture(const OpenGLTexture *pTexture)
    {
        assert(pTexture != nullptr);
        // delete fbos that reference the texture to be deleted
        {
            auto it = mTextureToFboHashes.find(*pTexture);
            if (it != mTextureToFboHashes.end())
            {
                for (auto fboHash : it->second)
                {
                    if (mFboIds.find(fboHash) != mFboIds.end())
                    {
                        FASTCG_CHECK_OPENGL_CALL(glDeleteFramebuffers(1, &mFboIds[fboHash]));
                        mFboIds.erase(fboHash);
                    }
                }
            }
        }
        BaseGraphicsSystem::DestroyTexture(pTexture);
    }

    void OpenGLGraphicsSystem::Submit()
    {
        auto fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        FASTCG_CHECK_OPENGL_ERROR("Failed to create fence");
        assert(fence != 0);
        FASTCG_CHECK_OPENGL_CALL(glFlush());
        auto result = glClientWaitSync(fence, 0, GL_TIMEOUT_IGNORED);
        FASTCG_CHECK_OPENGL_ERROR("Failed to wait for fence");
        if (result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED)
        {
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Failed to wait for fence");
        }
        FASTCG_CHECK_OPENGL_CALL(glDeleteSync(fence));

#if !defined FASTCG_DISABLE_GPU_TIMING
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime(mCurrentFrame);
        }
#endif
    }

    void OpenGLGraphicsSystem::WaitPreviousFrame()
    {
        auto previousFrame = GetPreviousFrame();

        if (mFrameFences[previousFrame] == nullptr)
        {
            // nothing to wait for
            return;
        }

        auto result = glClientWaitSync(mFrameFences[previousFrame], 0, GL_TIMEOUT_IGNORED);
        FASTCG_CHECK_OPENGL_ERROR("Failed to wait for fence");
        if (result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED)
        {
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Failed to wait for fence");
        }
        FASTCG_CHECK_OPENGL_CALL(glDeleteSync(mFrameFences[previousFrame]));
        mFrameFences[previousFrame] = nullptr;

#if !defined FASTCG_DISABLE_GPU_TIMING
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime(previousFrame);
        }
#endif
    }

    GLuint OpenGLGraphicsSystem::GetOrCreateFramebuffer(const OpenGLTexture *const *pRenderTargets,
                                                        uint32_t renderTargetCount,
                                                        const OpenGLTexture *pDepthStencilBuffer)
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
        FASTCG_CHECK_OPENGL_CALL(glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldFboId));

        GLuint fboId;
        FASTCG_CHECK_OPENGL_CALL(glGenFramebuffers(1, &fboId));
        FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboId));
#if _DEBUG
        {
            auto framebufferLabel = std::string("FBO ") + std::to_string(mFboIds.size()) + " (GL_FRAMEBUFFER)";
            FASTCG_CHECK_OPENGL_CALL(
                glObjectLabel(GL_FRAMEBUFFER, fboId, (GLsizei)framebufferLabel.size(), framebufferLabel.c_str()));
        }
#endif
        std::for_each(pRenderTargets, pRenderTargets + renderTargetCount, [i = 0](const auto *pRenderTarget) mutable {
            FASTCG_CHECK_OPENGL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (i++),
                                                            GetOpenGLTarget(pRenderTarget->GetType()), *pRenderTarget,
                                                            0));
        });

        if (pDepthStencilBuffer != nullptr)
        {
            GLenum attachment;
            if (HasStencil(pDepthStencilBuffer->GetFormat()))
            {
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            }
            else
            {
                attachment = GL_DEPTH_ATTACHMENT;
            }
            FASTCG_CHECK_OPENGL_CALL(glFramebufferTexture2D(
                GL_FRAMEBUFFER, attachment, GetOpenGLTarget(pDepthStencilBuffer->GetType()), *pDepthStencilBuffer, 0));
        }

        auto status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Couldn't create FBO: 0x%x\n", status);
        }

        mFboIds.emplace(fboHash, fboId);

        std::for_each(pRenderTargets, pRenderTargets + renderTargetCount,
                      [&](const auto *pRenderTarget) { mTextureToFboHashes[*pRenderTarget].emplace_back(fboHash); });

        if (pDepthStencilBuffer != nullptr)
        {
            mTextureToFboHashes[*pDepthStencilBuffer].emplace_back(fboHash);
        }

        FASTCG_CHECK_OPENGL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldFboId));

        return fboId;
    }

    GLuint OpenGLGraphicsSystem::GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, uint32_t bufferCount)
    {
        assert(bufferCount > 0);
        assert(std::all_of(pBuffers, pBuffers + bufferCount, [](const auto *pBuffer) {
            assert(pBuffer != nullptr);
            const auto &rVbDescs = pBuffer->GetVertexBindingDescriptors();
            return (pBuffer->GetUsage() & BufferUsageFlagBit::VERTEX_BUFFER) != 0 &&
                   std::all_of(rVbDescs.cbegin(), rVbDescs.cend(),
                               [](const auto &rVbDesc) { return rVbDesc.IsValid(); });
        }));

        auto vaoHash = (size_t)FNV1a(reinterpret_cast<const uint8_t *>(pBuffers), bufferCount * sizeof(OpenGLBuffer *));

        auto it = mVaoIds.find(vaoHash);
        if (it != mVaoIds.end())
        {
            return it->second;
        }

        GLuint vaoId;
        FASTCG_CHECK_OPENGL_CALL(glGenVertexArrays(1, &vaoId));
        FASTCG_CHECK_OPENGL_CALL(glBindVertexArray(vaoId));
#if _DEBUG
        {
            auto vertexArrayLabel = std::string("VAO ") + std::to_string(mVaoIds.size()) + " (GL_VERTEX_ARRAY)";
            FASTCG_CHECK_OPENGL_CALL(
                glObjectLabel(GL_VERTEX_ARRAY, vaoId, (GLsizei)vertexArrayLabel.size(), vertexArrayLabel.c_str()));
        }
#endif

        std::for_each(pBuffers, pBuffers + bufferCount, [](const auto *pBuffer) {
            FASTCG_CHECK_OPENGL_CALL(glBindBuffer(GetOpenGLTarget(pBuffer->GetUsage()), *pBuffer));
            for (const auto &rVbDesc : pBuffer->GetVertexBindingDescriptors())
            {
                assert(rVbDesc.size > 0);
                FASTCG_CHECK_OPENGL_CALL(glEnableVertexAttribArray(rVbDesc.binding));
                FASTCG_CHECK_OPENGL_CALL(glVertexAttribPointer(
                    rVbDesc.binding, rVbDesc.size, GetOpenGLType(rVbDesc.type), (GLboolean)rVbDesc.normalized,
                    rVbDesc.stride, (const GLvoid *)(uintptr_t)rVbDesc.offset));
            }
        });

        mVaoIds.emplace(vaoHash, vaoId);

        return vaoId;
    }

    void OpenGLGraphicsSystem::CreateOpenGLHeadlessContext()
    {
        NotifyPreContextCreate();

#if defined FASTCG_WINDOWS
        auto instance = GetModuleHandle(nullptr);

        WNDCLASS windowClass = {};
        windowClass.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
        };
        windowClass.hInstance = instance;
        windowClass.lpszClassName = "FastCG_OpenGL_TempWindow";

        if (!RegisterClass(&windowClass))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Win32: Couldn't register temporary window class");
        }

        auto hWnd =
            CreateWindowEx(0, "FastCG_OpenGL_TempWindow", "FastCG_TempWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                           CW_USEDEFAULT, mArgs.rScreenWidth, mArgs.rScreenHeight, nullptr, nullptr, instance, nullptr);

        HDC tempDeviceContext = GetDC(hWnd);

        if (tempDeviceContext == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Win32: Couldn't get a temporary device context");
        }

        PIXELFORMATDESCRIPTOR pixelFormatDescr = {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA,
            32,             // color bits
            8,              // R color bits
            0,              // R shift bits
            8,              // G color bits
            0,              // G shift bits
            8,              // B color bits
            0,              // B shift bits
            8,              // A color bits
            0,              // A shift bits
            0,              // accum bits
            0,              // R accum bits
            0,              // G accum bits
            0,              // B accum bits
            0,              // A accum bits
            24,             // depth bits
            8,              // stencil bits
            0,              // auxiliary buffers
            PFD_MAIN_PLANE, // layer type
            0,              // reserved
            0,              // layer mask
            0,              // visible mask
            0,              // damage mask
        };

        auto pixelFormat = ChoosePixelFormat(tempDeviceContext, &pixelFormatDescr);
        if (pixelFormat == 0)
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Win32: Couldn't choose a pixel format");
        }

        if (!::SetPixelFormat(tempDeviceContext, pixelFormat, &pixelFormatDescr))
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Win32: Couldn't set the current pixel format");
        }

        auto tempContext = wglCreateContext(tempDeviceContext);
        if (tempContext == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't create a temporary context");
        }

        if (!wglMakeCurrent(tempDeviceContext, tempContext))
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't make the temporary context current");
        }

        GLenum glewResult;
        if ((glewResult = glewInit()) != GLEW_OK)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't initialize glew: %s", glewGetErrorString(glewResult));
        }

        if (!WGLEW_ARB_pbuffer)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: WGL_ARB_pbuffer extension not supported");
        }

        if (!WGLEW_EXT_colorspace)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: WGL_EXT_colorspace extension not supported");
        }

        const int pixelAttribs[] = {WGL_DRAW_TO_PBUFFER_ARB,
                                    GL_TRUE,
                                    WGL_SUPPORT_OPENGL_ARB,
                                    GL_TRUE,
                                    WGL_ACCELERATION_ARB,
                                    WGL_FULL_ACCELERATION_ARB,
                                    WGL_PIXEL_TYPE_ARB,
                                    WGL_TYPE_RGBA_ARB,
                                    WGL_RED_BITS_ARB,
                                    8,
                                    WGL_GREEN_BITS_ARB,
                                    8,
                                    WGL_BLUE_BITS_ARB,
                                    8,
                                    WGL_ALPHA_BITS_ARB,
                                    8,
                                    WGL_DEPTH_BITS_ARB,
                                    24,
                                    WGL_STENCIL_BITS_ARB,
                                    8,
                                    WGL_COLORSPACE_EXT,
                                    WGL_COLORSPACE_LINEAR_EXT,
                                    0};

        UINT pixelFormatCount = 0;
        if (!wglChoosePixelFormatARB(tempDeviceContext, pixelAttribs, NULL, 1, &pixelFormat, &pixelFormatCount))
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't choose a pixel format");
        }

        if (pixelFormatCount == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't find an appropriate pixel format");
        }

        int pbufferAttribs[] = {WGL_PBUFFER_LARGEST_ARB, 1, 0};
        mPbuffer = wglCreatePbufferARB(tempDeviceContext, pixelFormat, (int)mArgs.rScreenWidth,
                                       (int)mArgs.rScreenHeight, pbufferAttribs);
        if (mPbuffer == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't create a pbuffer");
        }

        mDeviceContext = wglGetPbufferDCARB(mPbuffer);

        wglMakeCurrent(tempDeviceContext, nullptr);
        wglDeleteContext(tempContext);
        ReleaseDC(hWnd, tempDeviceContext);
        DestroyWindow(hWnd);

        mContext = CreateWGLContextAndMakeCurrent(mDeviceContext, nullptr);
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);

        int dummy;
        if (!glXQueryExtension(pDisplay, &dummy, &dummy))
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: OpenGL not supported by X server");
        }

        auto defaultScreen = DefaultScreen(pDisplay);

        int visualAttribs[] = {GLX_RGBA, None};

        auto *pVisualInfo = glXChooseVisual(pDisplay, defaultScreen, visualAttribs);
        if (pVisualInfo == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't choose a visual info");
        }

        auto tempContext = glXCreateContext(pDisplay, pVisualInfo, NULL, GL_TRUE);
        if (tempContext == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't create a temporary context");
        }

        auto pixmap = XCreatePixmap(pDisplay, DefaultRootWindow(pDisplay), mArgs.rScreenWidth, mArgs.rScreenHeight,
                                    pVisualInfo->depth);
        if (pixmap == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't create a pixmap");
        }

        auto tempDrawable = glXCreateGLXPixmap(pDisplay, pVisualInfo, pixmap);
        if (tempDrawable == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't create a temporary drawable");
        }

        XFree(pVisualInfo);

        glXMakeCurrent(pDisplay, tempDrawable, tempContext);

        GLenum glewResult;
        if ((glewResult = glewInit()) != GLEW_OK)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't initialize glew: %s", glewGetErrorString(glewResult));
        }

        glXMakeContextCurrent(pDisplay, None, None, nullptr);
        glXDestroyGLXPixmap(pDisplay, tempDrawable);
        XFreePixmap(pDisplay, pixmap);
        glXDestroyContext(pDisplay, tempContext);

        const int fbAttribs[] = {GLX_RENDER_TYPE,
                                 GLX_RGBA_BIT,
                                 GLX_DRAWABLE_TYPE,
                                 GLX_PBUFFER_BIT,
                                 GLX_DOUBLEBUFFER,
                                 True,
                                 GLX_RED_SIZE,
                                 8,
                                 GLX_GREEN_SIZE,
                                 8,
                                 GLX_BLUE_SIZE,
                                 8,
                                 GLX_ALPHA_SIZE,
                                 8,
                                 GLX_DEPTH_SIZE,
                                 24,
                                 GLX_STENCIL_SIZE,
                                 8,
                                 None};

        int fbConfigCount = 0;
        auto *pFbConfigs = glXChooseFBConfig(pDisplay, defaultScreen, fbAttribs, &fbConfigCount);

        if (fbConfigCount == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't find an appropriate framebuffer configuration");
        }

        // TODO: be more selective?
        auto fbConfig = pFbConfigs[0];

        const int pbufferAttribs[] = {GLX_PBUFFER_WIDTH, (int)mArgs.rScreenWidth, GLX_PBUFFER_HEIGHT,
                                      (int)mArgs.rScreenHeight, None};
        mDrawable = glXCreatePbuffer(pDisplay, fbConfig, pbufferAttribs);
        if (mDrawable == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't create a pbuffer");
        }

        mContext = CreateGLXContextAndMakeCurrent(pDisplay, fbConfig, mDrawable);
#elif defined FASTCG_ANDROID
        assert(mDisplay != nullptr);

        const EGLint contextAttribs[] = {EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                         EGL_NONE};

        EGLint contextConfigsCount;
        if (!eglChooseConfig(mDisplay, contextAttribs, &mConfig, 1, &contextConfigsCount))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't find an appropriate context configuration");
        }
        assert(contextConfigsCount > 0);

        mContext = eglCreateContext(mDisplay, mConfig, mContext, EGL_CONTEXT_ATTRIBS);
        if (mContext == EGL_NO_CONTEXT)
        {
            FASTCG_THROW_EXCEPTION(Exception, "EGL: Couldn't create a headless context");
        }

        const EGLint pbufferAttribs[] = {EGL_WIDTH, (EGLint)mArgs.rScreenWidth, EGL_HEIGHT, (EGLint)mArgs.rScreenHeight,
                                         EGL_NONE};

        mSurface = eglCreatePbufferSurface(mDisplay, mConfig, pbufferAttribs);
        if (mSurface == EGL_NO_SURFACE)
        {
            FASTCG_THROW_EXCEPTION(Exception, "EGL: Couldn't create a pbuffer surface");
        }

        if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't make the headless context current");
        }
#else
#error "FastCG::OpenGLRenderingSystem::CreateOpenGLHeadlessContext() is not implemented on the current platform"
#endif

        NotifyPostContextCreate();
    }

    void OpenGLGraphicsSystem::CreateOpenGLHeadedContext(void *pWindow)
    {
        NotifyPreContextCreate();

#if defined FASTCG_WINDOWS
        assert(pWindow != nullptr);

        auto oldDeviceContext = mDeviceContext;

        mDeviceContext = GetDC((HWND)pWindow);
        if (mDeviceContext == nullptr)
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Win32: Couldn't get the device context for the given window");
        }

        if (!WGLEW_ARB_pbuffer)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: WGL_ARB_pbuffer extension not supported");
        }

        if (!WGLEW_EXT_colorspace)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: WGL_EXT_colorspace extension not supported");
        }

        const int pixelAttribs[] = {WGL_DRAW_TO_WINDOW_ARB,
                                    GL_TRUE,
                                    WGL_SUPPORT_OPENGL_ARB,
                                    GL_TRUE,
                                    WGL_DOUBLE_BUFFER_ARB,
                                    GL_TRUE,
                                    WGL_PIXEL_TYPE_ARB,
                                    WGL_TYPE_RGBA_ARB,
                                    WGL_RED_BITS_ARB,
                                    8,
                                    WGL_GREEN_BITS_ARB,
                                    8,
                                    WGL_BLUE_BITS_ARB,
                                    8,
                                    WGL_ALPHA_BITS_ARB,
                                    8,
                                    WGL_DEPTH_BITS_ARB,
                                    24,
                                    WGL_STENCIL_BITS_ARB,
                                    8,
                                    WGL_COLORSPACE_EXT,
                                    WGL_COLORSPACE_LINEAR_EXT,
                                    0};

        int pixelFormat;
        UINT pixelFormatCount = 0;
        if (!wglChoosePixelFormatARB(mDeviceContext, pixelAttribs, NULL, 1, &pixelFormat, &pixelFormatCount))
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't choose a pixel format");
        }

        if (pixelFormatCount == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "WGL: Couldn't find an appropriate pixel format");
        }

        PIXELFORMATDESCRIPTOR pixelFormatDescr;
        if (!::SetPixelFormat(mDeviceContext, pixelFormat, &pixelFormatDescr))
        {
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Win32: Couldn't set the current pixel format");
        }

        auto oldContext = mContext;

        mContext = CreateWGLContextAndMakeCurrent(mDeviceContext, oldContext);

        wglDeleteContext(oldContext);
        wglReleasePbufferDCARB(mPbuffer, oldDeviceContext);
        wglDestroyPbufferARB(mPbuffer);
        mPbuffer = nullptr;

        if (WGLEW_EXT_swap_control)
        {
            wglSwapIntervalEXT(mArgs.vsync ? 1 : 0);
        }
#elif defined FASTCG_LINUX
        assert(pWindow == nullptr);

        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        assert(pDisplay != nullptr);

        auto defaultScreen = DefaultScreen(pDisplay);

        const int fbAttribs[] = {GLX_RENDER_TYPE,
                                 GLX_RGBA_BIT,
                                 GLX_DRAWABLE_TYPE,
                                 GLX_WINDOW_BIT,
                                 GLX_DOUBLEBUFFER,
                                 True,
                                 GLX_RED_SIZE,
                                 8,
                                 GLX_GREEN_SIZE,
                                 8,
                                 GLX_BLUE_SIZE,
                                 8,
                                 GLX_ALPHA_SIZE,
                                 8,
                                 GLX_DEPTH_SIZE,
                                 24,
                                 GLX_STENCIL_SIZE,
                                 8,
                                 None};

        int fbConfigCount = 0;
        auto *pFbConfigs = glXChooseFBConfig(pDisplay, defaultScreen, fbAttribs, &fbConfigCount);

        if (fbConfigCount == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't find an appropriate framebuffer configuration");
        }

        assert(pFbConfigs != nullptr);
        GLXFBConfig fbConfig = nullptr;
        XVisualInfo *pVisualInfo = nullptr;
        for (int i = 0; i < fbConfigCount; i++)
        {
            auto *pCurrVisualInfo = (XVisualInfo *)glXGetVisualFromFBConfig(pDisplay, pFbConfigs[i]);
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
                fbConfig = pFbConfigs[i];
                pVisualInfo = pCurrVisualInfo;
                break;
            }

            XFree(pCurrVisualInfo);
        }

        if (fbConfig == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "GLX: Couldn't find an appropriate framebuffer configuration");
        }

        auto oldDrawable = mDrawable;

        mDrawable = X11Application::GetInstance()->CreateWindow(pVisualInfo);
        assert(mDrawable != 0);

        XFree(pVisualInfo);

        auto oldContext = mContext;

        mContext = CreateGLXContextAndMakeCurrent(pDisplay, fbConfig, mDrawable, oldContext);

        glXDestroyPbuffer(pDisplay, oldDrawable);
        glXDestroyContext(pDisplay, oldContext);

        if (GLXEW_EXT_swap_control)
        {
            glXSwapIntervalEXT(pDisplay, mDrawable, mArgs.vsync ? 1 : 0);
        }
#elif defined FASTCG_ANDROID
        auto oldContext = mContext;

        const EGLint contextAttribs[] = {EGL_SURFACE_TYPE,
                                         EGL_WINDOW_BIT,
                                         EGL_RENDERABLE_TYPE,
                                         EGL_OPENGL_ES2_BIT,
                                         EGL_RED_SIZE,
                                         8,
                                         EGL_GREEN_SIZE,
                                         8,
                                         EGL_BLUE_SIZE,
                                         8,
                                         EGL_ALPHA_SIZE,
                                         8,
                                         EGL_DEPTH_SIZE,
                                         24,
                                         EGL_STENCIL_SIZE,
                                         8,
                                         EGL_NONE};

        EGLint contextConfigsCount;
        if (!eglChooseConfig(mDisplay, contextAttribs, &mConfig, 1, &contextConfigsCount))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't find an appropriate context configuration");
        }
        assert(contextConfigsCount > 0);

        mContext = eglCreateContext(mDisplay, mConfig, mContext, EGL_CONTEXT_ATTRIBS);
        if (mContext == EGL_NO_CONTEXT)
        {
            FASTCG_THROW_EXCEPTION(Exception, "EGL: Couldn't create the headed context");
        }

        auto oldSurface = mSurface;

        mSurface = eglCreateWindowSurface(mDisplay, mConfig, (ANativeWindow *)pWindow, NULL);
        if (mSurface == EGL_NO_SURFACE)
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't create a window surface");
        }

        if (!eglMakeCurrent(mDisplay, mSurface, mSurface, mContext))
        {
            FASTCG_CHECK_EGL_ERROR("EGL: Couldn't make the headed context current");
        }

        eglDestroySurface(mDisplay, oldSurface);
        eglDestroyContext(mDisplay, oldContext);
#else
#error "FastCG::OpenGLRenderingSystem::CreateOpenGLHeadedContext() is not implemented on the current platform"
#endif

        NotifyPostContextCreate();
    }

    void OpenGLGraphicsSystem::QueryDeviceProperties()
    {
        FASTCG_CHECK_OPENGL_CALL(glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mDeviceProperties.maxColorAttachments));
        FASTCG_CHECK_OPENGL_CALL(glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mDeviceProperties.maxDrawBuffers));
        FASTCG_CHECK_OPENGL_CALL(
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &mDeviceProperties.maxTextureUnits));
    }

    void OpenGLGraphicsSystem::DestroyOpenGLContext(void *pWindow)
    {
#if defined FASTCG_WINDOWS
        if (mPbuffer != nullptr)
        {
            wglDestroyPbufferARB(mPbuffer);
            mPbuffer = nullptr;
        }
        if (mContext != nullptr)
        {
            wglMakeCurrent(mDeviceContext, nullptr);
            wglDeleteContext(mContext);
            mContext = nullptr;
        }
        if (mDeviceContext != nullptr)
        {
            ReleaseDC((HWND)pWindow, mDeviceContext);
            mDeviceContext = nullptr;
        }
#elif defined FASTCG_LINUX
        if (mContext != nullptr)
        {
            auto *pDisplay = X11Application::GetInstance()->GetDisplay();
            assert(pDisplay != nullptr);

            glXMakeContextCurrent(pDisplay, None, None, nullptr);
            glXDestroyContext(pDisplay, mContext);
            mContext = nullptr;
        }
#elif defined FASTCG_ANDROID
        if (mSurface != EGL_NO_SURFACE)
        {
            eglDestroySurface(mDisplay, mSurface);
            mSurface = EGL_NO_SURFACE;
        }
        if (mContext != EGL_NO_CONTEXT)
        {
            eglDestroyContext(mDisplay, mContext);
            mContext = EGL_NO_CONTEXT;
        }
        if (mDisplay != nullptr)
        {
            eglTerminate(mDisplay);
        }
#else
#error "FastCG::OpenGLRenderingSystem::DestroyOpenGLContext() is not implemented on the current platform"
#endif
    }

    void OpenGLGraphicsSystem::SwapBuffers() const
    {
#if defined FASTCG_WINDOWS
        if (!::SwapBuffers(mDeviceContext))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Win32: Failed to swap buffers");
        }
#elif defined FASTCG_LINUX
        glXSwapBuffers(X11Application::GetInstance()->GetDisplay(), mDrawable);
#elif defined FASTCG_ANDROID
        if (!eglSwapBuffers(mDisplay, mSurface))
        {
            FASTCG_THROW_EXCEPTION(Exception, "EGL: Failed to swap buffers");
        }
#else
#error "FastCG::OpenGLRenderingSystem::SwapBuffers() not implemented on the current platform"
#endif
    }

    void OpenGLGraphicsSystem::SwapFrame()
    {
        if (mFrameFences[mCurrentFrame] != nullptr)
        {
            FASTCG_CHECK_OPENGL_CALL(glDeleteSync(mFrameFences[mCurrentFrame]));
        }
        mFrameFences[mCurrentFrame] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        FASTCG_CHECK_OPENGL_ERROR("Failed to create fence");
        assert(mFrameFences[mCurrentFrame] != 0);
        SwapBuffers();
        mCurrentFrame ^= 1;

        if (mFrameFences[mCurrentFrame] != nullptr)
        {
            auto result = glClientWaitSync(mFrameFences[mCurrentFrame], 0, GL_TIMEOUT_IGNORED);
            FASTCG_CHECK_OPENGL_ERROR("Failed to wait for fence");
            if (result != GL_ALREADY_SIGNALED && result != GL_CONDITION_SATISFIED)
            {
                FASTCG_THROW_EXCEPTION(Exception, "OpenGL: Failed to wait for fence");
            }
            FASTCG_CHECK_OPENGL_CALL(glDeleteSync(mFrameFences[mCurrentFrame]));
            mFrameFences[mCurrentFrame] = nullptr;
        }

#if !defined FASTCG_DISABLE_GPU_TIMING
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->RetrieveElapsedTime(mCurrentFrame);
        }
#endif
    }

    double OpenGLGraphicsSystem::GetGpuElapsedTime(uint32_t frame) const
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        double elapsedTime = 0;
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            elapsedTime += pGraphicsContext->GetElapsedTime(frame);
        }
        return elapsedTime;
#else
        return 0;
#endif
    }

    void OpenGLGraphicsSystem::OnPostWindowInitialize(void *pWindow)
    {
        CreateOpenGLHeadedContext(pWindow);
    }

    void OpenGLGraphicsSystem::OnPreWindowTerminate(void *pWindow)
    {
        DestroyOpenGLContext(pWindow);
    }

    void OpenGLGraphicsSystem::NotifyPreContextCreate()
    {
        // destroy all container objects
        for (const auto &rKvp : mFboIds)
        {
            FASTCG_CHECK_OPENGL_CALL(glDeleteFramebuffers(1, &rKvp.second));
        }
        mFboIds.clear();

        for (const auto &rKvp : mVaoIds)
        {
            FASTCG_CHECK_OPENGL_CALL(glDeleteVertexArrays(1, &rKvp.second));
        }
        mVaoIds.clear();

        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->OnPreContextCreate();
        }
    }

    void OpenGLGraphicsSystem::NotifyPostContextCreate()
    {
        for (auto *pGraphicsContext : GetGraphicsContexts())
        {
            pGraphicsContext->OnPostContextCreate();
        }
    }
}

#endif
