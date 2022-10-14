#ifdef FASTCG_OPENGL

#include <FastCG/OpenGLRenderingSystem.h>

#ifdef FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
#endif
#include <FastCG/OpenGLForwardRenderingStrategy.h>
#include <FastCG/OpenGLDeferredRenderingStrategy.h>
#include <FastCG/Exception.h>

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
        printf_s(
            "[%s] - %s - %s - %d - %s\n",
            GetOpenGLDebugOutputMessageSeverity(severity),
            GetOpenGLDebugOutputMessageSourceString(source),
            GetOpenGLDebugOutputMessageTypeString(type),
            id,
            message);
    }
#endif
}

namespace FastCG
{
    OpenGLRenderingSystem::OpenGLRenderingSystem(const RenderingSystemArgs &rArgs) : BaseRenderingSystem(rArgs)
    {
        RenderingPathStrategyArgs rpsArgs{
            mArgs.rScreenWidth,
            mArgs.rScreenHeight,
            mArgs.rClearColor,
            mArgs.rAmbientLight,
            mArgs.rDirectionalLights,
            mArgs.rPointLights,
            mArgs.rLineRenderers,
            mArgs.rPointsRenderers,
            mArgs.rRenderBatches,
            mArgs.rRenderingStatistics};

        switch (mArgs.renderingPath)
        {
        case RenderingPath::RP_FORWARD_RENDERING:
            mpRenderingPathStrategy = std::make_unique<OpenGLForwardRenderingStrategy>(rpsArgs);
            break;
        case RenderingPath::RP_DEFERRED_RENDERING:
            mpRenderingPathStrategy = std::make_unique<OpenGLDeferredRenderingStrategy>(rpsArgs);
            break;
        default:
            FASTCG_THROW_EXCEPTION(Exception, "Unhandled rendering path");
            break;
        }
    }

    void OpenGLRenderingSystem::OnInitialize()
    {
        CreateOpenGLContext();

#ifdef _DEBUG
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallbackARB(OpenGLDebugCallback, nullptr);
#endif
    }

    void OpenGLRenderingSystem::OnStart()
    {
        mpRenderingPathStrategy->OnResourcesLoaded();
    }

    void OpenGLRenderingSystem::OnFinalize()
    {
        DestroyOpenGLContext();
    }

    void OpenGLRenderingSystem::CreateOpenGLContext()
    {
#ifdef FASTCG_WINDOWS
        auto hDC = WindowsApplication::GetInstance()->GetDeviceContextHandle();
        auto tmpHGLRC = wglCreateContext(hDC);
        if (tmpHGLRC == 0)
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error creating a temporary GL context");
        }

        if (!wglMakeCurrent(hDC, tmpHGLRC))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error making the temporary GL context current");
        }

        {
            GLenum glewInitRes;
            if ((glewInitRes = glewInit()) != GLEW_OK)
            {
                FASTCG_THROW_EXCEPTION(Exception, "Error intializing glew: %s", glewGetErrorString(glewInitRes));
            }
        }

        const int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 2,
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
            FASTCG_THROW_EXCEPTION(Exception, "Error creating the final GL context");
        }

        if (!wglMakeCurrent(hDC, mHGLRC))
        {
            FASTCG_THROW_EXCEPTION(Exception, "Error making GL context current");
        }

        wglDeleteContext(tmpHGLRC);
#else
#error FastCG::OpenGLRenderingSystem::CreateOpenGLContext() is not implemented on the current platform
#endif
    }

    void OpenGLRenderingSystem::DestroyOpenGLContext()
    {
#ifdef FASTCG_WINDOWS
        if (mHGLRC != 0)
        {
            wglMakeCurrent(WindowsApplication::GetInstance()->GetDeviceContextHandle(), NULL);
            wglDeleteContext(mHGLRC);
        }
#else
#error "FastCG::OpenGLRenderingSystem::~DestroyOpenGLContext() is not implemented on the current platform"
#endif
    }

    void OpenGLRenderingSystem::OnRender(const Camera *pMainCamera)
    {
#ifdef FASTCG_WINDOWS
        if (mHGLRC == 0)
        {
            return;
        }
#endif

        if (pMainCamera == nullptr)
        {
            return;
        }

        mpRenderingPathStrategy->Render(pMainCamera);
    }

    void OpenGLRenderingSystem::BeforeDrawDebugTexts()
    {
#ifdef _DEBUG
        glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "Text Pass");
#endif

        glViewport(0, 0, mArgs.rScreenWidth, mArgs.rScreenHeight);
    }

    void OpenGLRenderingSystem::AfterDrawDebugTexts()
    {
#ifdef _DEBUG
        glPopDebugGroup();
#endif
    }

}

#endif