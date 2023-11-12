#include <FastCG/World/WorldSystem.h>
#include <FastCG/World/FlyController.h>
#include <FastCG/World/ComponentRegistry.h>
#include <FastCG/UI/ImGuiSystem.h>
#include <FastCG/Rendering/RenderingSystem.h>
#include <FastCG/Rendering/Renderable.h>
#include <FastCG/Rendering/PointLight.h>
#include <FastCG/Rendering/ModelLoader.h>
#include <FastCG/Rendering/DirectionalLight.h>
#include <FastCG/Platform/Thread.h>
#include <FastCG/Platform/Timer.h>
#include <FastCG/Platform/BaseApplication.h>
#include <FastCG/Input/MouseButton.h>
#include <FastCG/Input/Key.h>
#include <FastCG/Input/InputSystem.h>
#include <FastCG/Graphics/TextureLoader.h>
#include <FastCG/Graphics/GraphicsSystem.h>
#include <FastCG/Debug/DebugMenuSystem.h>
#include <FastCG/Core/System.h>
#include <FastCG/Core/MsgBox.h>
#include <FastCG/Core/Log.h>
#include <FastCG/Core/Exception.h>
#include <FastCG/Assets/AssetSystem.h>

#include <imgui.h>

#include <cstdio>
#include <cassert>
#include <algorithm>

namespace
{
	void DisplayStatisticsWindow(uint32_t width,
								 uint32_t height,
								 double target,
								 double app,
								 double os,
								 double present,
								 double wait,
								 double gpu,
								 const FastCG::RenderingStatistics &rRenderingStatistics)
	{
		if (ImGui::Begin("Statistics"))
		{

			auto GetColor = [target](double time)
			{
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
#ifdef _DEBUG
	FASTCG_IMPLEMENT_SYSTEM(DebugMenuSystem, DebugMenuSystemArgs);
#endif
	FASTCG_IMPLEMENT_SYSTEM(GraphicsSystem, GraphicsSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(InputSystem, InputSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(ImGuiSystem, ImGuiSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(RenderingSystem, RenderingSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(WorldSystem, WorldSystemArgs);

	BaseApplication::BaseApplication(const ApplicationSettings &settings) : mSettings(settings),
																			mWindowTitle(settings.windowTitle),
																			mScreenWidth(settings.screenWidth),
																			mScreenHeight(settings.screenHeight),
																			mFrameRate(settings.frameRate),
																			mSecondsPerFrame(settings.frameRate == UNLOCKED_FRAMERATE ? 0 : 1 / (double)settings.frameRate)
	{
		if (smpInstance != nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "There can only be one BaseApplication instance");
		}

		smpInstance = this;

		ComponentRegistry::RegisterComponent<Camera>();
		ComponentRegistry::RegisterComponent<DirectionalLight>();
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

			mRunning = true;
			RunMainLoop();

			OnEnd();

			Finalize();

			return 0;
		}
		catch (Exception &e)
		{
			FASTCG_LOG_ERROR("Fatal Exception: %s", e.GetFullDescription().c_str());
			FASTCG_MSG_BOX("Error", "Fatal Exception: %s", e.GetFullDescription().c_str());
			return -1;
		}
	}

	int BaseApplication::Run(int argc, char **argv)
	{
		if (!ParseCommandLineArguments(argc, argv))
		{
			FASTCG_LOG_ERROR("Failed to parse command line arguments");
			OnPrintUsage();
			return -1;
		}

		return Run();
	}

	void BaseApplication::Initialize()
	{
		FASTCG_LOG_VERBOSE("Pre-initializing application");
		OnPreInitialize();

		{
			FASTCG_LOG_VERBOSE("Creating systems");

			AssetSystem::Create({mSettings.assets.bundles});
#ifdef _DEBUG
			DebugMenuSystem::Create({});
#endif
			GraphicsSystem::Create({mScreenWidth,
									mScreenHeight,
									mSettings.maxSimultaneousFrames,
									mSettings.vsync});
			InputSystem::Create({});
			ImGuiSystem::Create({mScreenWidth,
								 mScreenHeight});
			RenderingSystem::Create({mSettings.rendering.path,
									 mScreenWidth,
									 mScreenHeight,
									 mSettings.rendering.clearColor,
									 mSettings.rendering.ambientLight,
									 mRenderingStatistics});
			WorldSystem::Create({mScreenWidth,
								 mScreenHeight});
		}

		{
			FASTCG_LOG_VERBOSE("Initializing systems");

			GraphicsSystem::GetInstance()->Initialize();

			for (const auto &rImportCallback : mSettings.assets.importCallbacks)
			{
				rImportCallback();
			}

			ImGuiSystem::GetInstance()->Initialize();
			RenderingSystem::GetInstance()->Initialize();
			WorldSystem::GetInstance()->Initialize();
		}

		FASTCG_LOG_VERBOSE("Post-initializing application");
		OnPostInitialize();
	}

	void BaseApplication::Finalize()
	{
		FASTCG_LOG_VERBOSE("Pre-finalizing application");
		OnPreFinalize();

		{
			FASTCG_LOG_VERBOSE("Finalizing systems");
			WorldSystem::GetInstance()->Finalize();
			RenderingSystem::GetInstance()->Finalize();
			ImGuiSystem::GetInstance()->Finalize();
			GraphicsSystem::GetInstance()->Finalize();
		}

		{
			FASTCG_LOG_VERBOSE("Destroying systems");
			WorldSystem::Destroy();
			RenderingSystem::Destroy();
			ImGuiSystem::Destroy();
			InputSystem::Destroy();
			GraphicsSystem::Destroy();
#ifdef _DEBUG
			DebugMenuSystem::Destroy();
#endif
			AssetSystem::Destroy();
		}

		FASTCG_LOG_VERBOSE("Post-finalizing application");
		OnPostFinalize();
	}

	bool BaseApplication::ParseCommandLineArguments(int argc, char **argv)
	{
		return true;
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

		KeyChange keyChanges[KEY_COUNT];
		InputSystem::GetKeyChanges(keyChanges);

		InputSystem::GetInstance()->Swap();

		ImGuiSystem::GetInstance()->BeginFrame(appDeltaTime, keyChanges);

		DisplayStatisticsWindow(mScreenWidth,
								mScreenHeight,
								mSecondsPerFrame,
								mLastAppElapsedTime,
								osTime,
								mLastPresentElapsedTime,
								mLastWaitTime,
								mLastGpuElapsedTime,
								mRenderingStatistics);
#ifdef _DEBUG
		DebugMenuSystem::GetInstance()->DrawMenu();
#endif

		WorldSystem::GetInstance()->Update((float)appStart, (float)appDeltaTime);

		ImGuiSystem::GetInstance()->EndFrame();

		mRenderingStatistics.Reset();

		RenderingSystem::GetInstance()->Render();

		auto presentationStart = Timer::GetTime();

		GraphicsSystem::GetInstance()->Present();

		auto appEnd = Timer::GetTime();
		mLastAppElapsedTime = appEnd - appStart;
		mLastPresentElapsedTime = appEnd - presentationStart;
		mLastGpuElapsedTime = GraphicsSystem::GetInstance()->GetGpuElapsedTime();
		mLastAppStart = appStart;

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
		OnResize();
	}
}