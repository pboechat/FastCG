#include <FastCG/WorldSystem.h>
#include <FastCG/Thread.h>
#include <FastCG/TextureLoader.h>
#include <FastCG/System.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/Renderable.h>
#include <FastCG/PointLight.h>
#include <FastCG/MouseButton.h>
#include <FastCG/ModelLoader.h>
#include <FastCG/Key.h>
#include <FastCG/ImGuiSystem.h>
#include <FastCG/InputSystem.h>
#include <FastCG/FlyController.h>
#include <FastCG/Exception.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/DebugMenuSystem.h>
#include <FastCG/ComponentRegistry.h>
#include <FastCG/BaseApplication.h>
#include <FastCG/AssetSystem.h>

#include <imgui.h>

#include <cstdio>
#include <cassert>
#include <algorithm>

namespace
{
	void DisplayStatisticsWindow(uint32_t width, uint32_t height, double target, double frame, double cpu, double gpu, double present, const FastCG::RenderingStatistics &rRenderingStatistics)
	{
		if (ImGui::Begin("Statistics"))
		{
			const ImVec4 green = {0, 1, 0, 1};
			const ImVec4 orange = {1, 0.27f, 0, 1};
			const ImVec4 red = {1, 0, 0, 1};
			const auto warnThreshold = 0.75 * target;

			ImGui::Text("Resolution: %ux%u", width, height);
			ImGui::Text("Target Time: %.6lf (%zu)", target, target == 0 ? 0 : (uint64_t)(1 / target));
			ImGui::TextColored(target == 0 || frame <= target ? (frame <= warnThreshold ? green : orange) : red, "Frame Time: %.6lf (%zu)", frame, frame == 0 ? 0 : (uint64_t)(1 / frame));
			ImGui::TextColored(target == 0 || cpu <= target ? (frame <= warnThreshold ? green : orange) : red, "CPU Time: %.6lf", cpu);
			ImGui::TextColored(target == 0 || gpu <= target ? (frame <= warnThreshold ? green : orange) : red, "GPU Time: %.6lf", gpu);
			ImGui::TextColored(target == 0 || present <= target ? green : red, "Present Time: %.6lf", present);
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

	int BaseApplication::Run(int argc, char **argv)
	{
		if (!ParseCommandLineArguments(argc, argv))
		{
			OnPrintUsage();
			return -1;
		}

		try
		{
			Initialize();

			mStartTimer.Start();
			OnStart();

			mRunning = true;
			RunMainLoop();

			mStartTimer.End();
			OnEnd();

			Finalize();

			return 0;
		}
		catch (Exception &e)
		{
			FASTCG_MSG_BOX("Error", "Fatal Exception: %s", e.GetFullDescription().c_str());
			return -1;
		}
	}

	void BaseApplication::Initialize()
	{
		AssetSystem::Create({mSettings.assets.bundles});
#ifdef _DEBUG
		DebugMenuSystem::Create({});
#endif
		InputSystem::Create({});
		ImGuiSystem::Create({mScreenWidth,
							 mScreenHeight});
		RenderingSystem::Create({mScreenWidth,
								 mScreenHeight,
								 mSettings.vsync});
		WorldSystem::Create({mSettings.rendering.path,
							 mScreenWidth,
							 mScreenHeight,
							 mSettings.rendering.clearColor,
							 mSettings.rendering.ambientLight,
							 mRenderingStatistics});

		RenderingSystem::GetInstance()->Initialize();

		for (const auto &rImportCallback : mSettings.assets.importCallbacks)
		{
			rImportCallback();
		}

		ImGuiSystem::GetInstance()->Initialize();
		WorldSystem::GetInstance()->Initialize();

		OnInitialize();
	}

	void BaseApplication::Finalize()
	{
		OnFinalize();

		WorldSystem::GetInstance()->Finalize();
		ImGuiSystem::GetInstance()->Finalize();
		RenderingSystem::GetInstance()->Finalize();

		WorldSystem::Destroy();
		RenderingSystem::Destroy();
		ImGuiSystem::Destroy();
		InputSystem::Destroy();
#ifdef _DEBUG
		DebugMenuSystem::Destroy();
#endif
		AssetSystem::Destroy();
	}

	bool BaseApplication::ParseCommandLineArguments(int argc, char **argv)
	{
		return true;
	}

	void BaseApplication::RunMainLoopIteration()
	{
		auto cpuStart = mStartTimer.GetTime();
		double frameDeltaTime;
		if (mLastFrameStart != 0)
		{
			frameDeltaTime = cpuStart - mLastFrameStart;
		}
		else
		{
			frameDeltaTime = 0;
		}
		mLastFrameStart = cpuStart;

		KeyChange keyChanges[KEY_COUNT];
		InputSystem::GetKeyChanges(keyChanges);

		InputSystem::GetInstance()->Swap();

		ImGuiSystem::GetInstance()->BeginFrame(frameDeltaTime, keyChanges);

#ifdef _DEBUG
		DisplayStatisticsWindow(mScreenWidth,
								mScreenHeight,
								mSecondsPerFrame,
								frameDeltaTime,
								mLastCpuElapsedTime,
								mLastGpuElapsedTime,
								RenderingSystem::GetInstance()->GetPresentElapsedTime(),
								mRenderingStatistics);
		DebugMenuSystem::GetInstance()->DrawMenu();
#endif

		WorldSystem::GetInstance()->Update((float)cpuStart, (float)frameDeltaTime);

		ImGuiSystem::GetInstance()->EndFrame();

		mRenderingStatistics.Reset();

		WorldSystem::GetInstance()->Render();
		ImGuiSystem::GetInstance()->Render();

		auto cpuEnd = mStartTimer.GetTime();
		mLastCpuElapsedTime = cpuEnd - cpuStart;

		RenderingSystem::GetInstance()->Present();

		mLastGpuElapsedTime = RenderingSystem::GetInstance()->GetGpuElapsedTime();

		mTotalFrameElapsedTime += frameDeltaTime;
		mFrameCount++;

		if (mFrameRate != UNLOCKED_FRAMERATE)
		{
			auto idleTime = mSecondsPerFrame - frameDeltaTime;
			if (idleTime > 0)
			{
				Thread::Sleep(idleTime);
				frameDeltaTime += idleTime;
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
		RenderingSystem::GetInstance()->Resize();
		OnResize();
	}
}