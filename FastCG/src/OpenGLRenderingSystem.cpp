#ifdef FASTCG_OPENGL

#if defined FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/X11Application.h>
#endif
#include <FastCG/OpenGLRenderingSystem.h>
#include <FastCG/OpenGLForwardRenderingPathStrategy.h>
#include <FastCG/OpenGLExceptions.h>
#include <FastCG/OpenGLDeferredRenderingPathStrategy.h>
#include <FastCG/Exception.h>
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

static_assert(sizeof(ImDrawIdx) == sizeof(uint32_t), "Please configure ImGui to use 32-bit indices");

namespace
{
#ifdef _DEBUG
#define CASE_RETURN_STRING(str) \
    case str:                   \
        return #str
    const char *GetOpenGLDebugOutputMessageSourceString(GLenum source)
    {
        switch (source)
        {
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_API);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_SHADER_COMPILER);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_APPLICATION);
            CASE_RETURN_STRING(GL_DEBUG_SOURCE_OTHER);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpengGL debug output message source");
            return nullptr;
        }
    }

    const char *GetOpenGLDebugOutputMessageTypeString(GLenum type)
    {
        switch (type)
        {
            CASE_RETURN_STRING(GL_DEBUG_TYPE_ERROR);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_PERFORMANCE);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_PORTABILITY);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_OTHER);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_MARKER);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_PUSH_GROUP);
            CASE_RETURN_STRING(GL_DEBUG_TYPE_POP_GROUP);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpengGL debug output message type");
            return nullptr;
        }
    }

    const char *GetOpenGLDebugOutputMessageSeverity(GLenum severity)
    {
        switch (severity)
        {
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_HIGH);
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_MEDIUM);
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_LOW);
            CASE_RETURN_STRING(GL_DEBUG_SEVERITY_NOTIFICATION);
        default:
            FASTCG_THROW_EXCEPTION(FastCG::Exception, "Unhandled OpengGL debug output message severity");
            return nullptr;
        }
    }

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
#endif

#ifdef FASTCG_LINUX
    int GLXContextErrorHandler(Display *dpy, XErrorEvent *ev)
    {
        FASTCG_THROW_EXCEPTION(FastCG::Exception, "Error creating GLX context (error_code: %d)", ev->error_code);
        return 0;
    }
#endif
}

namespace FastCG
{
    OpenGLRenderingSystem::OpenGLRenderingSystem(const RenderingSystemArgs &rArgs) : BaseRenderingSystem(rArgs)
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

        RenderingPathStrategyArgs rpsArgs{
            mArgs.rScreenWidth,
            mArgs.rScreenHeight,
            mArgs.rClearColor,
            mArgs.rAmbientLight,
            mArgs.rDirectionalLights,
            mArgs.rPointLights,
            mArgs.rRenderBatches,
            mArgs.rRenderingStatistics};

        switch (mArgs.renderingPath)
        {
        case RenderingPath::RP_FORWARD_RENDERING:
            mpRenderingPathStrategy = std::make_unique<OpenGLForwardRenderingPathStrategy>(rpsArgs);
            break;
        case RenderingPath::RP_DEFERRED_RENDERING:
            mpRenderingPathStrategy = std::make_unique<OpenGLDeferredRenderingPathStrategy>(rpsArgs);
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled rendering path");
            break;
        }

        glGenBuffers(1, &mImGuiVerticesBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mImGuiVerticesBufferId);
#ifdef _DEBUG
        {
            const char bufferLabel[] = "ImGui Vertices (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mImGuiVerticesBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel);
        }
#endif
        glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(ImDrawVert), nullptr, GL_STREAM_DRAW);

        glGenBuffers(1, &mImGuiIndicesBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mImGuiIndicesBufferId);
#ifdef _DEBUG
        {
            const char bufferLabel[] = "ImGui Indices (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mImGuiIndicesBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel);
        }
#endif
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 90000 * sizeof(ImDrawIdx), nullptr, GL_STREAM_DRAW);

        glGenBuffers(1, &mImGuiConstantsBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, mImGuiConstantsBufferId);
#ifdef _DEBUG
        {
            const char bufferLabel[] = "ImGui Constants (GL_BUFFER)";
            glObjectLabel(GL_BUFFER, mImGuiConstantsBufferId, FASTCG_ARRAYSIZE(bufferLabel), bufferLabel);
        }
#endif
        glBufferData(GL_UNIFORM_BUFFER, sizeof(ImGuiConstants), &mImGuiConstants, GL_DYNAMIC_DRAW);

        glGenVertexArrays(1, &mImGuiVertexArrayId);
        glBindVertexArray(mImGuiVertexArrayId);
#ifdef _DEBUG
        {
            const char vertexArrayLabel[] = "ImGui (GL_VERTEX_ARRAY)";
            glObjectLabel(GL_VERTEX_ARRAY, mImGuiVertexArrayId, FASTCG_ARRAYSIZE(vertexArrayLabel), vertexArrayLabel);
        }
#endif

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, mImGuiVerticesBufferId);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (void *)offsetof(ImDrawVert, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (void *)offsetof(ImDrawVert, uv));
        glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (void *)offsetof(ImDrawVert, col));

        FASTCG_CHECK_OPENGL_ERROR();

        mpRenderingPathStrategy->Initialize();
    }

    void OpenGLRenderingSystem::PostInitialize()
    {
        mpImGuiShader = FindShader("ImGui");
        mImGuiColorMapLocation = mpImGuiShader->GetBindingLocation("uColorMap");

        mpRenderingPathStrategy->PostInitialize();
    }

    void OpenGLRenderingSystem::Finalize()
    {
        if (mImGuiVertexArrayId != ~0u)
        {
            glDeleteVertexArrays(1, &mImGuiVertexArrayId);
            mImGuiVertexArrayId = ~0u;
        }

        if (mImGuiConstantsBufferId != ~0u)
        {
            glDeleteBuffers(1, &mImGuiConstantsBufferId);
            mImGuiConstantsBufferId = ~0u;
        }

        if (mImGuiIndicesBufferId != ~0u)
        {
            glDeleteBuffers(1, &mImGuiIndicesBufferId);
            mImGuiIndicesBufferId = ~0u;
        }

        if (mImGuiVerticesBufferId != ~0u)
        {
            glDeleteBuffers(1, &mImGuiVerticesBufferId);
            mImGuiVerticesBufferId = ~0u;
        }

        mpRenderingPathStrategy->Finalize();

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

#define DECLARE_CREATE_METHOD(className, containerMember)                             \
    className *OpenGLRenderingSystem::Create##className(const className##Args &rArgs) \
    {                                                                                 \
        containerMember.emplace_back(new className{rArgs});                           \
        return containerMember.back();                                                \
    }

    DECLARE_CREATE_METHOD(Material, mMaterials)
    DECLARE_CREATE_METHOD(Mesh, mMeshes)
    DECLARE_CREATE_METHOD(Shader, mShaders)
    DECLARE_CREATE_METHOD(Texture, mTextures)

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

    void OpenGLRenderingSystem::Render(const Camera *pMainCamera)
    {
#if defined FASTCG_WINDOWS
        assert(mHGLRC != 0);
#elif defined FASTCT_LINUX
        assert(mpRenderContext != nullptr);
#endif

        if (pMainCamera == nullptr)
        {
            return;
        }

        mpRenderingPathStrategy->Render(pMainCamera);
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

    void OpenGLRenderingSystem::RenderImGui(const ImDrawData *pImDrawData)
    {
#ifdef _DEBUG
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "ImGui Passes");
#endif

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_SCISSOR_TEST);

        auto displayScale = pImDrawData->FramebufferScale;
        auto displayPos = ImVec2(pImDrawData->DisplayPos.x * displayScale.x, pImDrawData->DisplayPos.y * displayScale.y);
        auto displaySize = ImVec2(pImDrawData->DisplaySize.x * displayScale.x, pImDrawData->DisplaySize.y * displayScale.y);

        glViewport((GLint)displayPos.x, (GLint)displayPos.y, (GLsizei)displaySize.x, (GLsizei)displaySize.y);

        mImGuiConstants.projection = glm::ortho(displayPos.x, displayPos.x + displaySize.x, displayPos.y + displaySize.y, displayPos.y);
        glBindBuffer(GL_UNIFORM_BUFFER, mImGuiConstantsBufferId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ImGuiConstants), &mImGuiConstants);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        mpImGuiShader->Bind();

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, mImGuiConstantsBufferId);

        for (int n = 0; n < pImDrawData->CmdListsCount; n++)
        {
            const auto *cmdList = pImDrawData->CmdLists[n];

            glBindBuffer(GL_ARRAY_BUFFER, mImGuiVerticesBufferId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)cmdList->VtxBuffer.size_in_bytes(), cmdList->VtxBuffer.Data);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mImGuiIndicesBufferId);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, (GLsizeiptr)cmdList->IdxBuffer.size_in_bytes(), cmdList->IdxBuffer.Data);

            for (int cmdIdx = 0; cmdIdx < cmdList->CmdBuffer.Size; cmdIdx++)
            {
                const auto *pCmd = &cmdList->CmdBuffer[cmdIdx];
                if (pCmd->UserCallback)
                {
                    pCmd->UserCallback(cmdList, pCmd);
                }
                else
                {
                    ImVec2 clipMin((pCmd->ClipRect.x - displayPos.x) * displayScale.x, (pCmd->ClipRect.y - displayPos.y) * displayScale.y);
                    ImVec2 clipMax((pCmd->ClipRect.z - displayPos.x) * displayScale.x, (pCmd->ClipRect.w - displayPos.y) * displayScale.y);
                    if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
                    {
                        continue;
                    }

                    glScissor((GLint)clipMin.x, (GLint)(displaySize.y - clipMax.y), (GLsizei)(clipMax.x - clipMin.x), (GLsizei)(clipMax.y - clipMin.y));

                    mpImGuiShader->BindTexture(mImGuiColorMapLocation, *((OpenGLTexture *)pCmd->GetTexID()), 0);

                    glBindVertexArray(mImGuiVertexArrayId);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mImGuiIndicesBufferId);
                    glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pCmd->ElemCount, GL_UNSIGNED_INT, (void *)(intptr_t)(pCmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pCmd->VtxOffset);
                }
            }
        }

        mpImGuiShader->Unbind();

        FASTCG_CHECK_OPENGL_ERROR();

#ifdef _DEBUG
        glPopDebugGroup();
#endif
    }

}

#endif