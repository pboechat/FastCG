#include <FastCG/Assets/AssetSystem.h>
#include <FastCG/Core/Exception.h>
#include <FastCG/Core/MsgBox.h>
#include <FastCG/Core/Random.h>
#include <FastCG/Core/System.h>
#include <FastCG/Debug/DebugMenuSystem.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Input/InputSystem.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Input/MouseButton.h>
#include <FastCG/Platform/BaseApplication.h>
#include <FastCG/Platform/Thread.h>
#include <FastCG/Platform/Timer.h>
#include <FastCG/Rendering/DirectionalLight.h>
#include <FastCG/Rendering/Fog.h>
#include <FastCG/Rendering/PointLight.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/RenderingSystem.h>
#include <FastCG/UI/ImGuiSystem.h>
#include <FastCG/World/ComponentRegistry.h>
#include <FastCG/World/FlyController.h>
#include <FastCG/World/WorldSystem.h>

#include <imgui.h>

#include <algorithm>
#include <cassert>
#include <cstdio>

// defined in the application's Config.cpp
extern const char *const APPLICATION_NAME;

namespace
{
    void DisplayStatisticsWindow(uint32_t width, uint32_t height, double target, double app, double os, double present,
                                 double wait, double gpu, const FastCG::RenderingStatistics &rRenderingStatistics)
    {
        if (ImGui::Begin("Statistics"))
        {

            auto GetColor = [target](double time) {
                return target == 0 || time > target ? ImVec4{1, 0, 0, 1} : ImVec4{0, 1, 0, 1};
            };

            ImGui::Text("Platform: %s", FASTCG_PLATFORM);
            ImGui::Text("Graphics: %s", FASTCG_GRAPHICS_SYSTEM);
            ImGui::Text("Resolution: %ux%u", width, height);
            ImGui::Text("Target: %.6lf (%zu)", target, target == 0 ? 0 : (uint64_t)(1 / target));
            auto cpu = os + app + present + wait;
            ImGui::TextColored(GetColor(cpu), "CPU: %.6lf (%zu)", cpu, cpu == 0 ? 0 : (uint64_t)(1 / cpu));
            ImGui::TextColored(GetColor(app), "    App: %.6lf (%zu)", app, app == 0 ? 0 : (uint64_t)(1 / app));
            ImGui::TextColored(GetColor(os), "    OS: %.6lf", os);
            ImGui::TextColored(GetColor(present), "    Present: %.6lf", present);
            ImGui::TextColored(GetColor(wait), "    Wait: %.6lf", wait);
            ImGui::TextColored(GetColor(gpu), "GPU: %.6lf (%zu)", gpu, gpu == 0 ? 0 : (uint64_t)(1 / gpu));
            ImGui::Text("Draw Calls: %u", rRenderingStatistics.drawCalls);
            ImGui::Text("Triangles: %u", rRenderingStatistics.triangles);
        }
        ImGui::End();
    }

}

namespace FastCG
{
    BaseApplication *BaseApplication::smpInstance = nullptr;

    FASTCG_IMPLEMENT_SYSTEM(AssetSystem, AssetSystemArgs);
#if _DEBUG
    FASTCG_IMPLEMENT_SYSTEM(DebugMenuSystem, DebugMenuSystemArgs);
#endif
    FASTCG_IMPLEMENT_SYSTEM(GraphicsSystem, GraphicsSystemArgs);
    FASTCG_IMPLEMENT_SYSTEM(InputSystem, InputSystemArgs);
    FASTCG_IMPLEMENT_SYSTEM(ImGuiSystem, ImGuiSystemArgs);
    FASTCG_IMPLEMENT_SYSTEM(RenderingSystem, RenderingSystemArgs);
    FASTCG_IMPLEMENT_SYSTEM(WorldSystem, WorldSystemArgs);

    BaseApplication::BaseApplication(const ApplicationSettings &settings)
        : mSettings(settings), mFrameRate(settings.graphics.frameRate),
          mWindowTitle(settings.windowTitle.empty() ? APPLICATION_NAME : settings.windowTitle),
          mScreenWidth(settings.graphics.screenWidth), mScreenHeight(settings.graphics.screenHeight),
          mSecondsPerFrame(settings.graphics.frameRate == UNLOCKED_FRAMERATE ? 0
                                                                             : 1 / (double)settings.graphics.frameRate)
    {
        if (smpInstance != nullptr)
        {
            FASTCG_THROW_EXCEPTION(Exception, "There can only be one BaseApplication instance");
        }

        smpInstance = this;

        // in attempts to promote semi-deterministic behavior, at least during application startup,
        // PRNG is always seeded with zero
        Random::Seed(0);

        ComponentRegistry::RegisterComponent<Camera>();
        ComponentRegistry::RegisterComponent<DirectionalLight>();
        ComponentRegistry::RegisterComponent<Fog>();
        ComponentRegistry::RegisterComponent<FlyController>();
        ComponentRegistry::RegisterComponent<PointLight>();
        ComponentRegistry::RegisterComponent<Renderable>();
    }

    BaseApplication::~BaseApplication()
    {
        smpInstance = nullptr;
    }

    int BaseApplication::Run()
    {
        try
        {
            Initialize();

            OnStart();

            RunMainLoop();

            OnEnd();

            Finalize();

            return 0;
        }
        catch (Exception &e)
        {
            FASTCG_LOG_ERROR(BaseApplication, "Fatal Exception: %s", e.GetFullDescription().c_str());
            if (!mSettings.graphics.headless)
            {
                FASTCG_MSG_BOX("Error", "Fatal Exception: %s", e.GetFullDescription().c_str());
            }
            return -1;
        }
    }

    int BaseApplication::Run(int argc, char **argv)
    {
        if (!ParseCommandLineArguments(argc, argv))
        {
            FASTCG_LOG_ERROR(BaseApplication, "Failed to parse command line arguments");
            OnPrintUsage();
            return -1;
        }

        return Run();
    }

    void BaseApplication::Initialize()
    {
        {
            FASTCG_LOG_DEBUG(BaseApplication, "Creating systems");

            AssetSystem::Create({mSettings.assets.bundles});
#if _DEBUG
            DebugMenuSystem::Create({});
#endif
            GraphicsSystem::Create({mScreenWidth, mScreenHeight, mSettings.graphics.maxSimultaneousFrames,
                                    mSettings.graphics.vsync, mSettings.graphics.headless});
            InputSystem::Create({});
            ImGuiSystem::Create({mScreenWidth, mScreenHeight});
            RenderingSystem::Create({mSettings.rendering.path, mSettings.rendering.hdr, mScreenWidth, mScreenHeight,
                                     mSettings.rendering.clearColor, mSettings.rendering.ambientLight,
                                     mRenderingStatistics});
            WorldSystem::Create({mScreenWidth, mScreenHeight});
        }

        FASTCG_LOG_DEBUG(BaseApplication, "Pre-initializing application");
        OnPreInitialize();

        {
            FASTCG_LOG_DEBUG(BaseApplication, "Initializing systems");

            GraphicsSystem::GetInstance()->Initialize();

            for (const auto &rImportCallback : mSettings.assets.importCallbacks)
            {
                rImportCallback();
            }

            ImGuiSystem::GetInstance()->Initialize();
            RenderingSystem::GetInstance()->Initialize();
            WorldSystem::GetInstance()->Initialize();
        }

        FASTCG_LOG_DEBUG(BaseApplication, "Post-initializing application");
        OnPostInitialize();
    }

    void BaseApplication::Finalize()
    {
        FASTCG_LOG_DEBUG(BaseApplication, "Pre-finalizing application");
        OnPreFinalize();

        {
            FASTCG_LOG_DEBUG(BaseApplication, "Finalizing systems");
            WorldSystem::GetInstance()->Finalize();
            RenderingSystem::GetInstance()->Finalize();
            ImGuiSystem::GetInstance()->Finalize();
            GraphicsSystem::GetInstance()->Finalize();
        }

        FASTCG_LOG_DEBUG(BaseApplication, "Post-finalizing application");
        OnPostFinalize();

        {
            FASTCG_LOG_DEBUG(BaseApplication, "Destroying systems");
            WorldSystem::Destroy();
            RenderingSystem::Destroy();
            ImGuiSystem::Destroy();
            InputSystem::Destroy();
            GraphicsSystem::Destroy();
#if _DEBUG
            DebugMenuSystem::Destroy();
#endif
            AssetSystem::Destroy();
        }
    }

    void BaseApplication::RunMainLoopIteration(double osTime)
    {
        auto appStart = Timer::GetTime();
        double appDeltaTime;
        if (mLastAppStart != 0)
        {
            appDeltaTime = appStart - mLastAppStart;
        }
        else
        {
            appDeltaTime = 0;
        }

        OnFrameStart(appDeltaTime);

        KeyChange keyChanges[KEY_COUNT];
        InputSystem::GetKeyChanges(keyChanges);

        InputSystem::GetInstance()->Swap();

        ImGuiSystem::GetInstance()->BeginFrame(appDeltaTime, keyChanges);

        DisplayStatisticsWindow(mScreenWidth, mScreenHeight, mSecondsPerFrame, mLastAppElapsedTime, osTime,
                                mLastPresentElapsedTime, mLastWaitTime, mLastGpuElapsedTime, mRenderingStatistics);
#if _DEBUG
        DebugMenuSystem::GetInstance()->DrawMenu();
#endif

        WorldSystem::GetInstance()->Update((float)appStart, (float)appDeltaTime);

        ImGuiSystem::GetInstance()->EndFrame();

        mRenderingStatistics.Reset();

        RenderingSystem::GetInstance()->Render();

        auto presentationStart = Timer::GetTime();

        GraphicsSystem::GetInstance()->SwapFrame();

        auto appEnd = Timer::GetTime();
        mLastAppElapsedTime = appEnd - appStart;
        mLastPresentElapsedTime = appEnd - presentationStart;
        mLastGpuElapsedTime =
            GraphicsSystem::GetInstance()->GetGpuElapsedTime(GraphicsSystem::GetInstance()->GetCurrentFrame());
        mLastAppStart = appStart;

        OnFrameEnd();

        mFrameCount++;

        if (mFrameRate != UNLOCKED_FRAMERATE)
        {
            mLastWaitTime = std::max(0.0, mSecondsPerFrame - mLastAppElapsedTime - 0.00034 /* wait overhead */);
            if (mLastWaitTime > 0)
            {
                double startTime = Timer::GetTime();
                while (Timer::GetTime() - startTime <= mLastWaitTime)
                {
                    // busy-waiting, do nothing.
                }
            }
        }
    }

    void BaseApplication::MouseButtonCallback(MouseButton button, MouseButtonState state)
    {
        InputSystem::GetInstance()->SetMouseButton(button, state);
        OnMouseButton(button, state);
    }

    void BaseApplication::MouseMoveCallback(uint32_t x, uint32_t y)
    {
        InputSystem::GetInstance()->SetMousePosition(glm::uvec2(x, y));
        OnMouseMove(x, y);
    }

    void BaseApplication::MouseWheelCallback(float delta)
    {
        InputSystem::GetInstance()->SetMouseWheelDelta(delta);
        OnMouseWheel(delta);
    }

    void BaseApplication::KeyboardCallback(Key key, bool pressed)
    {
        InputSystem::GetInstance()->SetKey(key, pressed);
        if (pressed)
        {
            OnKeyPress(key);
        }
        else
        {
            OnKeyRelease(key);
        }
    }

    void BaseApplication::WindowResizeCallback(uint32_t width, uint32_t height)
    {
        if (mScreenWidth == width && mScreenHeight == height)
        {
            return;
        }

        mScreenWidth = width;
        mScreenHeight = height;
        GraphicsSystem::GetInstance()->Resize();
        RenderingSystem::GetInstance()->Resize();
        OnResize();
    }
}