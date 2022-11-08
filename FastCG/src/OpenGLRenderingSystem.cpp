#ifdef FASTCG_OPENGL

#if defined FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/X11Application.h>
#endif
#include <FastCG/OpenGLUtils.h>
#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/Exception.h>
#include <FastCG/FastCG.h>
#include <FastCG/AssetSystem.h>

#if defined FASTCG_WINDOWS
#include <GL/wglew.h>
#elif defined FASTCG_LINUX
#include <GL/glxew.h>
#endif
#include <GL/glew.h>
#include <GL/gl.h>

#if defined FASTCG_LINUX
#include <X11/extensions/Xrender.h>
#endif

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

    // Source: https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html
    void CRC(uint32_t &h, uint32_t ki)
    {
        auto highOrder = h & 0xf8000000; // extract high-order 5 bits from h
                                         // 0xf8000000 is the hexadecimal representation
                                         //   for the 32-bit number with the first five
                                         //   bits = 1 and the other bits = 0
        h = h << 5;                      // shift h left by 5 bits
        h = h ^ (highOrder >> 27);       // move the highOrder 5 bits to the low-order
                                         //   end and XOR into h
        h = h ^ ki;                      // XOR h and ki
    }
}

namespace FastCG
{
    void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
    {
        printf(
            "[%s] - %s - %s - %d - %s\n",
            GetOpenGLDebugOutputMessageSeverity(severity),
            GetOpenGLDebugOutputMessageSourceString(source),
            GetOpenGLDebugOutputMessageTypeString(type),
            id,
            message);
    }

    OpenGLRenderingSystem::OpenGLRenderingSystem(const RenderingSystemArgs &rArgs) : BaseRenderingSystem(rArgs),
                                                                                     mArgs(rArgs)
    {
    }

    OpenGLRenderingSystem::~OpenGLRenderingSystem() = default;

    void OpenGLRenderingSystem::Initialize()
    {
        CreateOpenGLContext(true);

        GLenum glewInitRes;
        if ((glewInitRes = glewInit()) != GLEW_OK)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error intializing glew: %s", glewGetErrorString(glewInitRes));
        }

        CreateOpenGLContext();

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallbackARB(OpenGLDebugCallback, nullptr);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
#endif

        // Create backbuffer handler (fake texture)
        mpBackbuffer = CreateTexture({"Backbuffer",
                                      1,
                                      1,
                                      TextureType::TEXTURE_2D,
                                      TextureFormat::RGBA,
                                      {32, 32, 32, 32},
                                      TextureDataType::FLOAT,
                                      TextureFilter::POINT_FILTER,
                                      TextureWrapMode::CLAMP,
                                      false});
    }

#define DESTROY_ALL(containerName)             \
    for (const auto *pElement : containerName) \
    {                                          \
        delete pElement;                       \
    }                                          \
    containerName.clear()

    void OpenGLRenderingSystem::Finalize()
    {
        DESTROY_ALL(mMaterials);
        DESTROY_ALL(mMeshes);
        DESTROY_ALL(mShaders);
        DESTROY_ALL(mBuffers);
        DESTROY_ALL(mTextures);
        DESTROY_ALL(mRenderingContexts);

        for (const auto &kvp : mVaoIds)
        {
            glDeleteVertexArrays(1, &kvp.second);
        }
        mVaoIds.clear();

        DestroyOpenGLContext();

#if defined FASTCG_WINDOWS
        WindowsApplication::GetInstance()->DestroyDeviceContext();
#elif defined FASTCG_LINUX
        if (mpVisualInfo != nullptr)
        {
            XFree(mpVisualInfo);
            mpVisualInfo = nullptr;
            mpFbConfig = nullptr;
        }
        X11Application::GetInstance()->CloseDisplay();
#endif
    }

#if defined FASTCG_WINDOWS
    void OpenGLRenderingSystem::SetupPixelFormat() const
    {
        auto hDC = WindowsApplication::GetInstance()->GetDeviceContext();
        PIXELFORMATDESCRIPTOR pixelFormatDescr =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER,
                PFD_TYPE_RGBA,
                32,
                0, 0, 0, 0, 0, 0, // color bits (ignored)
                0,                // no alpha buffer
                0,                // alpha bits (ignored)
                0,                // no accumulation buffer
                0, 0, 0, 0,       // accum bits (ignored)
                32,               // depth buffer
                0,                // no stencil buffer
                0,                // no auxiliary buffers
                PFD_MAIN_PLANE,   // main layer
                0,                // reserved
                0, 0, 0,          // no layer, visible, damage masks
            };
        auto pixelFormat = ChoosePixelFormat(hDC, &pixelFormatDescr);
        if (!SetPixelFormat(hDC, pixelFormat, &pixelFormatDescr))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error setting pixel format");
        }
    }
#elif defined FASTCG_LINUX
    XVisualInfo *OpenGLRenderingSystem::GetVisualInfo()
    {
        if (mpVisualInfo != nullptr)
        {
            return mpVisualInfo;
        }

        AcquireVisualInfoAndFbConfig();

        return mpVisualInfo;
    }

    void OpenGLRenderingSystem::AcquireVisualInfoAndFbConfig()
    {
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();

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
            GLX_DEPTH_SIZE, 24,
            None};

        auto defaultScreen = DefaultScreen(pDisplay);

        int numFbConfigs;
        auto fbConfigs = glXChooseFBConfig(pDisplay, defaultScreen, fbAttribs, &numFbConfigs);
        for (int i = 0; i < numFbConfigs; i++)
        {
            auto *pVisualInfo = (XVisualInfo *)glXGetVisualFromFBConfig(pDisplay, fbConfigs[i]);
            if (pVisualInfo == nullptr)
            {
                continue;
            }

            auto *pVisualFormat = XRenderFindVisualFormat(pDisplay, pVisualInfo->visual);
            if (pVisualFormat == nullptr)
            {
                continue;
            }

            if (pVisualFormat->direct.alphaMask == 0)
            {
                mpFbConfig = fbConfigs[i];
                mpVisualInfo = pVisualInfo;
                break;
            }

            XFree(pVisualInfo);
        }

        if (mpFbConfig == nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "No matching FB config");
        }
    }
#endif

#define DECLARE_CREATE_METHOD(className, containerMember)         \
    OpenGL##className *OpenGLRenderingSystem::Create##className() \
    {                                                             \
        containerMember.emplace_back(new OpenGL##className{});    \
        return containerMember.back();                            \
    }

#define DECLARE_CREATE_METHOD_WITH_ARGS(className, containerMember, argType, argName) \
    OpenGL##className *OpenGLRenderingSystem::Create##className(argType argName)      \
    {                                                                                 \
        containerMember.emplace_back(new OpenGL##className{argName});                 \
        return containerMember.back();                                                \
    }

    DECLARE_CREATE_METHOD_WITH_ARGS(Buffer, mBuffers, const BufferArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Material, mMaterials, const OpenGLMaterial::MaterialArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Mesh, mMeshes, const MeshArgs &, rArgs)
    DECLARE_CREATE_METHOD(RenderingContext, mRenderingContexts)
    DECLARE_CREATE_METHOD_WITH_ARGS(Shader, mShaders, const ShaderArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Texture, mTextures, const TextureArgs &, rArgs)

#define DECLARE_DESTROY_METHOD(className, containerMember)                                   \
    void OpenGLRenderingSystem::Destroy##className(const className *p##className)            \
    {                                                                                        \
        auto it = std::find(containerMember.cbegin(), containerMember.cend(), p##className); \
        if (it != containerMember.cend())                                                    \
        {                                                                                    \
            containerMember.erase(it);                                                       \
        }                                                                                    \
        else                                                                                 \
        {                                                                                    \
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy " #className);               \
        }                                                                                    \
    }

    DECLARE_DESTROY_METHOD(Buffer, mBuffers)
    DECLARE_DESTROY_METHOD(Material, mMaterials)
    DECLARE_DESTROY_METHOD(Mesh, mMeshes)
    DECLARE_DESTROY_METHOD(RenderingContext, mRenderingContexts)
    DECLARE_DESTROY_METHOD(Shader, mShaders)
    DECLARE_DESTROY_METHOD(Texture, mTextures)

    GLuint OpenGLRenderingSystem::GetOrCreateFramebuffer(const OpenGLTexture *const *pTextures, size_t textureCount)
    {
        assert(textureCount > 0);
        assert(std::all_of(pTextures, pTextures + textureCount, [](const auto *pTexture)
                           { return pTexture != nullptr; }));

        uint32_t h = 0;
        std::for_each(pTextures, pTextures + textureCount, [&h](const auto *pTexture)
                      { CRC(h, *pTexture); });

        auto it = mFboIds.find(h);
        if (it != mFboIds.end())
        {
            return it->second;
        }

        GLuint fboId;
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboId);
#ifdef _DEBUG
        {
            auto framebufferLabel = std::string("FBO ") + std::to_string(mFboIds.size()) + " (GL_FRAMEBUFFER)";
            glObjectLabel(GL_FRAMEBUFFER, fboId, (GLsizei)framebufferLabel.size(), framebufferLabel.c_str());
        }
#endif
        std::for_each(pTextures, pTextures + textureCount, [i = 0](const auto *pTexture) mutable
                      {
            GLenum attachment;
            if (pTexture->GetFormat() == TextureFormat::DEPTH_STENCIL)
            {
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            }
            else if (pTexture->GetFormat() == TextureFormat::DEPTH)
            {
                attachment = GL_DEPTH_ATTACHMENT;
            }
            else
            {
                attachment = GL_COLOR_ATTACHMENT0 + (i++);
            }
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GetOpenGLTarget(pTexture->GetType()), *pTexture, 0); });

        auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            FASTCG_THROW_EXCEPTION(OpenGLException, "Error creating FBO: 0x%x\n", status);
        }

        mFboIds.emplace(h, fboId);

        return fboId;
    }

    GLuint OpenGLRenderingSystem::GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, size_t bufferCount)
    {
        assert(bufferCount > 0);
        assert(std::all_of(pBuffers, pBuffers + bufferCount, [](const auto *pBuffer)
                           { 
                            assert(pBuffer != nullptr); 
                            const auto& rVbDescs = pBuffer->GetVertexBindingDescriptors(); 
                            return pBuffer->GetType() == BufferType::VERTEX_ATTRIBUTE && 
                                std::all_of(rVbDescs.cbegin(), rVbDescs.cend(), [](const auto& rVbDesc) { return rVbDesc.IsValid(); }); }));

        uint32_t h = 0;
        std::for_each(pBuffers, pBuffers + bufferCount, [&h](const auto *pBuffer)
                      { CRC(h, *pBuffer); });

        auto it = mVaoIds.find(h);
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
                        glBindBuffer(GetOpenGLTarget(pBuffer->GetType()), *pBuffer);
            for (const auto &rVbDesc : pBuffer->GetVertexBindingDescriptors())
            {
                glEnableVertexAttribArray(rVbDesc.binding);
                glVertexAttribPointer(rVbDesc.binding, rVbDesc.size, GetOpenGLType(rVbDesc.type), (GLboolean)rVbDesc.normalized, rVbDesc.stride, (const GLvoid*)(uintptr_t)rVbDesc.offset);
            } });

        mVaoIds.emplace(h, vaoId);

        return vaoId;
    }

    void OpenGLRenderingSystem::CreateOpenGLContext(bool temporary /* = false */)
    {
#if defined FASTCG_WINDOWS
        auto hDC = WindowsApplication::GetInstance()->GetDeviceContext();

        auto oldHGLRC = mHGLRC;
        if (temporary)
        {
            mHGLRC = wglCreateContext(hDC);
            if (mHGLRC == 0)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Error creating a temporary WGL context");
            }

            if (!wglMakeCurrent(hDC, mHGLRC))
            {
                FASTCG_THROW_EXCEPTION(Exception, "Error making a temporary WGL context current");
            }
        }
        else
        {
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

            mHGLRC = wglCreateContextAttribsARB(hDC, mHGLRC, attribs);
            if (mHGLRC == 0)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Error creating a final WGL context");
            }

            if (!wglMakeCurrent(hDC, mHGLRC))
            {
                FASTCG_THROW_EXCEPTION(Exception, "Error making a final WGL context current");
            }
        }

        if (oldHGLRC != 0)
        {
            wglDeleteContext(oldHGLRC);
        }
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        static Window sTempWindow{None};
        if (sTempWindow != None)
        {
            XDestroyWindow(pDisplay, sTempWindow);
            XSync(pDisplay, False);
            sTempWindow = None;
        }
        auto *pOldRenderContext = mpRenderContext;
        if (temporary)
        {
            auto defaultScreen = DefaultScreen(pDisplay);
            sTempWindow = XCreateSimpleWindow(pDisplay, RootWindow(pDisplay, defaultScreen), 0, 0, 1, 1, 1, BlackPixel(pDisplay, defaultScreen), WhitePixel(pDisplay, defaultScreen));

            XMapWindow(pDisplay, sTempWindow);

            XWindowAttributes tempWindowAttribs;
            XGetWindowAttributes(pDisplay, sTempWindow, &tempWindowAttribs);

            XVisualInfo tempVisualInfo;
            tempVisualInfo.visualid = XVisualIDFromVisual(tempWindowAttribs.visual);

            int n;
            XVisualInfo *pTempVisualInfo = XGetVisualInfo(pDisplay, VisualIDMask, &tempVisualInfo, &n);

            auto *pOldErrorHandler = XSetErrorHandler(&GLXContextErrorHandler);
            mpRenderContext = glXCreateContext(pDisplay, pTempVisualInfo, 0, true);
            XSetErrorHandler(pOldErrorHandler);

            XFree(pTempVisualInfo);

            glXMakeCurrent(pDisplay, sTempWindow, mpRenderContext);
        }
        else
        {
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

            if (mpFbConfig == nullptr)
            {
                AcquireVisualInfoAndFbConfig();
            }

            auto *pOldErrorHandler = XSetErrorHandler(&GLXContextErrorHandler);
            mpRenderContext = glXCreateContextAttribsARB(pDisplay, mpFbConfig, 0, True, attribs);
            XSetErrorHandler(pOldErrorHandler);

            if (mpRenderContext == nullptr)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Error creating a final GLX context");
            }

            XSync(pDisplay, False);

            auto &rWindow = X11Application::GetInstance()->GetWindow();
            if (!glXMakeContextCurrent(pDisplay, rWindow, rWindow, mpRenderContext))
            {
                FASTCG_THROW_EXCEPTION(Exception, "Error making a final GLX context current");
            }
        }

        if (pOldRenderContext != nullptr)
        {
            glXDestroyContext(pDisplay, pOldRenderContext);
        }
#else
#error FastCG::OpenGLRenderingSystem::CreateOpenGLContext() is not implemented on the current platform
#endif
    }

    void OpenGLRenderingSystem::DestroyOpenGLContext()
    {
#if defined FASTCG_WINDOWS
        if (mHGLRC != 0)
        {
            wglMakeCurrent(WindowsApplication::GetInstance()->GetDeviceContext(), nullptr);
            wglDeleteContext(mHGLRC);
        }
#elif defined FASTCG_LINUX
        if (mpRenderContext != nullptr)
        {
            auto *pDisplay = X11Application::GetInstance()->GetDisplay();

            glXMakeContextCurrent(pDisplay, None, None, nullptr);
            glXDestroyContext(pDisplay, mpRenderContext);

            mpRenderContext = nullptr;
        }
#else
#error "FastCG::OpenGLRenderingSystem::~DestroyOpenGLContext() is not implemented on the current platform"
#endif
    }

    void OpenGLRenderingSystem::Present()
    {
#if defined FASTCG_WINDOWS
        auto hDC = WindowsApplication::GetInstance()->GetDeviceContext();
        SwapBuffers(hDC);
#elif defined FASTCG_LINUX
        auto *pDisplay = X11Application::GetInstance()->GetDisplay();
        auto &window = X11Application::GetInstance()->GetWindow();
        glXSwapIntervalEXT(pDisplay, window, 0);
        glXSwapBuffers(pDisplay, window);
#else
#error "OpenGLRenderingSystem::Present() not implemented on the current platform"
#endif
    }
}

#endif