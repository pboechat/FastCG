#ifdef FASTCG_OPENGL

#if defined FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/X11Application.h>
#endif
#include <FastCG/OpenGLUtils.h>
#include <FastCG/OpenGLGraphicsSystem.h>
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

    void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
    {
        printf(
            "[%s] - %s - %s - %d - %s\n",
            FastCG::GetOpenGLDebugOutputMessageSeverity(severity),
            FastCG::GetOpenGLDebugOutputMessageSourceString(source),
            FastCG::GetOpenGLDebugOutputMessageTypeString(type),
            id,
            message);
    }

}

namespace FastCG
{
    OpenGLGraphicsSystem::OpenGLGraphicsSystem(const GraphicsSystemArgs &rArgs) : BaseGraphicsSystem(rArgs)
    {
    }

    OpenGLGraphicsSystem::~OpenGLGraphicsSystem() = default;

    void OpenGLGraphicsSystem::Initialize()
    {
        BaseGraphicsSystem::Initialize();

        CreateOpenGLContext(true);

        GLenum glewInitRes;
        if ((glewInitRes = glewInit()) != GLEW_OK)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error intializing glew: %s", glewGetErrorString(glewInitRes));
        }

        CreateOpenGLContext();

        InitializeDeviceProperties();

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallbackARB(OpenGLDebugCallback, nullptr);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PUSH_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
        glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_POP_GROUP, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
#endif

#ifdef _DEBUG
        glGenQueries(1, &mPresentTimestampQuery);
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

        mInitialized = true;
    }

#define DESTROY_ALL(containerName)             \
    for (const auto *pElement : containerName) \
    {                                          \
        delete pElement;                       \
    }                                          \
    containerName.clear()

    void OpenGLGraphicsSystem::Finalize()
    {
        mInitialized = false;

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

        DESTROY_ALL(mMaterials);
        DESTROY_ALL(mMeshes);
        DESTROY_ALL(mShaders);
        DESTROY_ALL(mBuffers);
        DESTROY_ALL(mTextures);
        DESTROY_ALL(mRenderingContexts);

#ifdef _DEBUG
        glDeleteQueries(1, &mPresentTimestampQuery);
#endif

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
    void OpenGLGraphicsSystem::SetupPixelFormat() const
    {
        auto hDC = WindowsApplication::GetInstance()->GetDeviceContext();
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
        auto pixelFormat = ChoosePixelFormat(hDC, &pixelFormatDescr);
        if (!SetPixelFormat(hDC, pixelFormat, &pixelFormatDescr))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error setting pixel format");
        }
    }
#elif defined FASTCG_LINUX
    XVisualInfo *OpenGLGraphicsSystem::GetVisualInfo()
    {
        if (mpVisualInfo != nullptr)
        {
            return mpVisualInfo;
        }

        AcquireVisualInfoAndFbConfig();

        return mpVisualInfo;
    }

    void OpenGLGraphicsSystem::AcquireVisualInfoAndFbConfig()
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
            GLX_DEPTH_SIZE, 0,
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

#define DECLARE_CREATE_METHOD(className, containerMember)        \
    OpenGL##className *OpenGLGraphicsSystem::Create##className() \
    {                                                            \
        containerMember.emplace_back(new OpenGL##className{});   \
        return containerMember.back();                           \
    }

#define DECLARE_CREATE_METHOD_WITH_ARGS(className, containerMember, argType, argName) \
    OpenGL##className *OpenGLGraphicsSystem::Create##className(argType argName)       \
    {                                                                                 \
        containerMember.emplace_back(new OpenGL##className{argName});                 \
        return containerMember.back();                                                \
    }

    DECLARE_CREATE_METHOD_WITH_ARGS(Buffer, mBuffers, const BufferArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Mesh, mMeshes, const MeshArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(MaterialDefinition, mMaterialDefinitions, const OpenGLMaterialDefinition::MaterialDefinitionArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(RenderingContext, mRenderingContexts, const RenderingContextArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Shader, mShaders, const ShaderArgs &, rArgs)
    DECLARE_CREATE_METHOD_WITH_ARGS(Texture, mTextures, const TextureArgs &, rArgs)

    OpenGLMaterial *OpenGLGraphicsSystem::CreateMaterial(const OpenGLMaterial::MaterialArgs &rArgs)
    {
        OpenGLBuffer *pMaterialConstantBuffer = nullptr;
        const auto &rConstantBuffer = rArgs.pMaterialDefinition->GetConstantBuffer();
        if (rConstantBuffer.GetSize() > 0)
        {
            pMaterialConstantBuffer = CreateBuffer({rArgs.name + " Material Constant Buffer",
                                                    BufferType::UNIFORM,
                                                    BufferUsage::DYNAMIC,
                                                    rConstantBuffer.GetSize(),
                                                    rConstantBuffer.GetData()});
        }

        mMaterials.emplace_back(new OpenGLMaterial(rArgs, pMaterialConstantBuffer));
        return mMaterials.back();
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

    DECLARE_DESTROY_METHOD(Buffer, mBuffers)
    DECLARE_DESTROY_METHOD(Mesh, mMeshes)
    DECLARE_DESTROY_METHOD(MaterialDefinition, mMaterialDefinitions)
    DECLARE_DESTROY_METHOD(RenderingContext, mRenderingContexts)
    DECLARE_DESTROY_METHOD(Shader, mShaders)

    void OpenGLGraphicsSystem::DestroyMaterial(const OpenGLMaterial *pMaterial)
    {
        assert(pMaterial != nullptr);
        const auto *pMaterialConstantBuffer = pMaterial->GetConstantBuffer();
        if (pMaterialConstantBuffer != nullptr)
        {
            DestroyBuffer(pMaterialConstantBuffer);
        }

        auto it = std::find(mMaterials.cbegin(), mMaterials.cend(), pMaterial);
        if (it != mMaterials.cend())
        {
            mMaterials.erase(it);
            delete pMaterial;
        }
        else
        {
            FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy material '%s'", pMaterial->GetName().c_str());
        }
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
        {
            auto it = std::find(mTextures.cbegin(), mTextures.cend(), pTexture);
            if (it != mTextures.cend())
            {
                mTextures.erase(it);
                delete pTexture;
            }
            else
            {
                FASTCG_THROW_EXCEPTION(Exception, "Couldn't destroy texture '%s'", pTexture->GetName().c_str());
            }
        }
    }

    GLuint OpenGLGraphicsSystem::GetOrCreateFramebuffer(const OpenGLTexture *const *pTextures, size_t textureCount)
    {
        assert(textureCount > 0);
        assert(textureCount <= mDeviceProperties.maxColorAttachments);
        assert(std::all_of(pTextures, pTextures + textureCount, [](const auto *pTexture)
                           { return pTexture != nullptr; }));

        uint32_t fboHash = 0;
        std::for_each(pTextures, pTextures + textureCount, [&fboHash](const auto *pTexture)
                      { CRC(fboHash, *pTexture); });

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

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldFboId);

        auto status = glCheckNamedFramebufferStatusEXT(fboId, GL_DRAW_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            FASTCG_THROW_EXCEPTION(OpenGLException, "Error creating FBO: 0x%x\n", status);
        }

        mFboIds[fboHash] = fboId;

        std::for_each(pTextures, pTextures + textureCount, [&](const auto *pTexture)
                      { mTextureToFboHashes[*pTexture].emplace_back(fboHash); });

        return fboId;
    }

    GLuint OpenGLGraphicsSystem::GetOrCreateVertexArray(const OpenGLBuffer *const *pBuffers, size_t bufferCount)
    {
        assert(bufferCount > 0);
        assert(std::all_of(pBuffers, pBuffers + bufferCount, [](const auto *pBuffer)
                           { 
                            assert(pBuffer != nullptr); 
                            const auto& rVbDescs = pBuffer->GetVertexBindingDescriptors(); 
                            return pBuffer->GetType() == BufferType::VERTEX_ATTRIBUTE && 
                                std::all_of(rVbDescs.cbegin(), rVbDescs.cend(), [](const auto& rVbDesc) { return rVbDesc.IsValid(); }); }));

        uint32_t vaoHash = 0;
        std::for_each(pBuffers, pBuffers + bufferCount, [&vaoHash](const auto *pBuffer)
                      { CRC(vaoHash, *pBuffer); });

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
                        glBindBuffer(GetOpenGLTarget(pBuffer->GetType()), *pBuffer);
            for (const auto &rVbDesc : pBuffer->GetVertexBindingDescriptors())
            {
                glEnableVertexAttribArray(rVbDesc.binding);
                glVertexAttribPointer(rVbDesc.binding, rVbDesc.size, GetOpenGLType(rVbDesc.type), (GLboolean)rVbDesc.normalized, rVbDesc.stride, (const GLvoid*)(uintptr_t)rVbDesc.offset);
            } });

        mVaoIds.emplace(vaoHash, vaoId);

        return vaoId;
    }

    void OpenGLGraphicsSystem::CreateOpenGLContext(bool temporary /* = false */)
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

            wglSwapIntervalEXT(mArgs.vsync ? 1 : 0);
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

            glXSwapIntervalEXT(pDisplay, rWindow, mArgs.vsync ? 1 : 0);
        }

        if (pOldRenderContext != nullptr)
        {
            glXDestroyContext(pDisplay, pOldRenderContext);
        }
#else
#error FastCG::OpenGLRenderingSystem::CreateOpenGLContext() is not implemented on the current platform
#endif
    }

    void OpenGLGraphicsSystem::InitializeDeviceProperties()
    {
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &mDeviceProperties.maxColorAttachments);
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &mDeviceProperties.maxDrawBuffers);
    }

    void OpenGLGraphicsSystem::DestroyOpenGLContext()
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

    void OpenGLGraphicsSystem::Present()
    {
#ifdef _DEBUG
        GLint64 presentStart;
        glQueryCounter(mPresentTimestampQuery, GL_TIMESTAMP);
        glGetInteger64v(GL_TIMESTAMP, &presentStart);
#endif
#if defined FASTCG_WINDOWS
        auto hDC = WindowsApplication::GetInstance()->GetDeviceContext();
        SwapBuffers(hDC);
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

        for (auto *pRenderingContext : mRenderingContexts)
        {
            pRenderingContext->RetrieveElapsedTime();
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
        for (auto *pRenderingContext : mRenderingContexts)
        {
            elapsedTime += pRenderingContext->GetElapsedTime();
        }
        return elapsedTime;
#else
        return 0;
#endif
    }
}

#endif