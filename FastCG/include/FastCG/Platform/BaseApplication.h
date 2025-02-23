#ifndef FASTCG_BASE_APPLICATION_H
#define FASTCG_BASE_APPLICATION_H

#include <FastCG/Core/Colors.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/Macros.h>
#include <FastCG/Core/Version.h>
#include <FastCG/Graphics/RenderingPath.h>
#include <FastCG/Graphics/ShaderImporter.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Input/MouseButton.h>
#include <FastCG/Rendering/MaterialDefinitionImporter.h>
#include <FastCG/Rendering/RenderingStatistics.h>

#include <glm/glm.hpp>

#ifdef FASTCG_ANDROID
#include <signal.h>
#endif

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace FastCG
{
    using ImportCallback = void (*)(void);

    constexpr uint32_t UNLOCKED_FRAMERATE = ~0u;
    constexpr ImportCallback DEFAULT_IMPORT_CALLBACKS[] = {&ShaderImporter::Import,
                                                           &MaterialDefinitionImporter::Import};

    struct ApplicationSettings
    {
        std::string windowTitle{};
        struct
        {
            uint32_t screenWidth{1024};
            uint32_t screenHeight{768};
            uint32_t frameRate{60};
            uint32_t maxSimultaneousFrames{3};
            bool vsync{false};
            bool headless{false};
        } graphics;
        struct
        {
            RenderingPath path{RenderingPath::DEFERRED};
            glm::vec4 clearColor{Colors::BLACK};
            glm::vec4 ambientLight{Colors::BLACK};
            bool hdr{false};
        } rendering;
        struct
        {
            std::vector<std::filesystem::path> bundles{};
            std::vector<ImportCallback> importCallbacks{
                DEFAULT_IMPORT_CALLBACKS, DEFAULT_IMPORT_CALLBACKS + FASTCG_ARRAYSIZE(DEFAULT_IMPORT_CALLBACKS)};
        } assets;
    };

    class BaseApplication
    {
    public:
        BaseApplication(const ApplicationSettings &settings);
        virtual ~BaseApplication();

        inline static BaseApplication *GetInstance()
        {
            return smpInstance;
        }

        inline const std::string &GetWindowTitle() const
        {
            return mWindowTitle;
        }

        inline uint32_t GetScreenWidth() const
        {
            return mScreenWidth;
        }

        inline uint32_t GetScreenHeight() const
        {
            return mScreenHeight;
        }

        inline float GetAspectRatio() const
        {
            return mScreenWidth / (float)mScreenHeight;
        }

        inline uint32_t GetFrameRate() const
        {
            return mFrameRate;
        }

        inline void SetFrameRate(uint32_t frameRate)
        {
            mFrameRate = frameRate;
            mSecondsPerFrame = 1 / (double)frameRate;
        }

        inline RenderingPath GetRenderingPath() const
        {
            return mSettings.rendering.path;
        }

        inline size_t GetFrameCount() const
        {
            return mFrameCount;
        }

        int Run();
        int Run(int argc, char **argv);

        inline void Exit()
        {
            mRunning = false;
        }

    protected:
        const ApplicationSettings mSettings;
        uint32_t mFrameRate;
        bool mRunning{true};

        virtual bool ParseCommandLineArguments(int argc, char **argv)
        {
            FASTCG_UNUSED(argc);
            FASTCG_UNUSED(argv);
            return true;
        }
        virtual void OnResize()
        {
        }
        virtual void OnStart()
        {
        }
        virtual void OnEnd()
        {
        }
        virtual void OnMouseButton(MouseButton button, MouseButtonState state)
        {
            FASTCG_UNUSED(button);
            FASTCG_UNUSED(state);
        }
        virtual void OnMouseMove(uint32_t x, uint32_t y)
        {
            FASTCG_UNUSED(x);
            FASTCG_UNUSED(y);
        }
        virtual void OnMouseWheel(float delta)
        {
            FASTCG_UNUSED(delta);
        }
        virtual void OnKeyPress(Key key)
        {
            FASTCG_UNUSED(key);
        }
        virtual void OnKeyRelease(Key key)
        {
            FASTCG_UNUSED(key);
        }
        virtual void OnPrintUsage()
        {
        }
        virtual void OnFrameStart(double deltaTime)
        {
        }
        virtual void OnFrameEnd()
        {
        }
        virtual void RunMainLoop() = 0;
        void RunMainLoopIteration(double osTime);
        void WindowResizeCallback(uint32_t width, uint32_t height);
        void MouseButtonCallback(MouseButton button, MouseButtonState state);
        void MouseMoveCallback(uint32_t x, uint32_t y);
        void MouseWheelCallback(float delta);
        void KeyboardCallback(Key key, bool pressed);
        virtual void OnPreInitialize()
        {
        }
        virtual void OnPostInitialize()
        {
        }
        virtual void OnPreFinalize()
        {
        }
        virtual void OnPostFinalize()
        {
        }

    private:
        static BaseApplication *smpInstance;

        std::string mWindowTitle;
        uint32_t mScreenWidth;
        uint32_t mScreenHeight;
        double mSecondsPerFrame;
        size_t mFrameCount{0};
        double mLastAppStart{0};
        double mLastAppElapsedTime{0};
        double mLastGpuElapsedTime{0};
        double mLastPresentElapsedTime{0};
        double mLastWaitTime{0};
        RenderingStatistics mRenderingStatistics;
        void Initialize();
        void Finalize();
    };

}

#define FASTCG_LOG_BUILD_PROPERTIES()                                                                                  \
    FASTCG_LOG_DEBUG(Build, "Platform: %s", FASTCG_PLATFORM);                                                          \
    FASTCG_LOG_DEBUG(Build, "Graphics system: %s", FASTCG_GRAPHICS_SYSTEM);                                            \
    FASTCG_LOG_DEBUG(Build, "FastCG version: %d.%d.%d", FastCG::MAJOR_VERSION, FastCG::MINOR_VERSION,                  \
                     FastCG::PATCH_VERSION)

#if defined FASTCG_ANDROID
#define FASTCG_MAIN(appType)                                                                                           \
    void handleSigtrap(int signal, siginfo_t *info, void *context)                                                     \
    {                                                                                                                  \
        FASTCG_LOG_INFO(main, "Caught a SIGTRAP signal");                                                              \
        ucontext_t *ucontext = (ucontext_t *)context;                                                                  \
        ucontext->uc_mcontext.pc += 4;                                                                                 \
    }                                                                                                                  \
    void setupSignalHandlers()                                                                                         \
    {                                                                                                                  \
        struct sigaction action;                                                                                       \
        action.sa_sigaction = handleSigtrap;                                                                           \
        sigemptyset(&action.sa_mask);                                                                                  \
        action.sa_flags = SA_SIGINFO;                                                                                  \
        sigaction(SIGTRAP, &action, NULL);                                                                             \
    }                                                                                                                  \
    void finish(android_app *pAndroidApp, JNIEnv *pJniEnv)                                                             \
    {                                                                                                                  \
        jobject activity = pAndroidApp->activity->clazz;                                                               \
        jclass activityClass = pJniEnv->GetObjectClass(activity);                                                      \
        jmethodID safeFinishMethod = pJniEnv->GetMethodID(activityClass, "finish", "()V");                             \
        pJniEnv->CallVoidMethod(activity, safeFinishMethod);                                                           \
    }                                                                                                                  \
    void android_main(android_app *pAndroidApp)                                                                        \
    {                                                                                                                  \
        setupSignalHandlers();                                                                                         \
        JNIEnv *pJniEnv;                                                                                               \
        pAndroidApp->activity->vm->AttachCurrentThread(&pJniEnv, NULL);                                                \
        FASTCG_LOG_BUILD_PROPERTIES();                                                                                 \
        appType app;                                                                                                   \
        app.SetJNIEnv(pJniEnv);                                                                                        \
        app.SetAndroidApp(pAndroidApp);                                                                                \
        app.Run();                                                                                                     \
        finish(pAndroidApp, pJniEnv);                                                                                  \
        pAndroidApp->activity->vm->DetachCurrentThread();                                                              \
    }
#else
#define FASTCG_MAIN(appType)                                                                                           \
    int main(int argc, char **argv)                                                                                    \
    {                                                                                                                  \
        FASTCG_LOG_BUILD_PROPERTIES();                                                                                 \
        appType app;                                                                                                   \
        return app.Run(argc, argv);                                                                                    \
    }
#endif

#endif