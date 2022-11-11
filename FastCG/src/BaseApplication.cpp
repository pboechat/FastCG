#include <FastCG/Thread.h>
#include <FastCG/TextureImporter.h>
#include <FastCG/System.h>
#include <FastCG/ShaderLoader.h>
#include <FastCG/Renderable.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/PointLight.h>
#include <FastCG/MouseButton.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/RenderBatchStrategy.h>
#include <FastCG/Light.h>
#include <FastCG/Key.h>
#include <FastCG/ImGuiSystem.h>
#include <FastCG/ImGuiRenderer.h>
#include <FastCG/InputSystem.h>
#include <FastCG/GameObject.h>
#include <FastCG/ForwardWorldRenderer.h>
#include <FastCG/Exception.h>
#include <FastCG/DirectionalLight.h>
#include <FastCG/DeferredWorldRenderer.h>
#include <FastCG/DebugMenuSystem.h>
#include <FastCG/Component.h>
#include <FastCG/Camera.h>
#include <FastCG/Behaviour.h>
#include <FastCG/BaseApplication.h>
#include <FastCG/AssetSystem.h>

#include <imgui.h>

#include <cstdio>
#include <cassert>
#include <algorithm>

namespace
{
	void DeclareStatisticsWindow(double target, double frame, double cpu, double gpu, double present, const FastCG::RenderingStatistics &rRenderingStatistics)
	{
		if (ImGui::Begin("Statistics"))
		{
			const ImVec4 green = {0, 1, 0, 1};
			const ImVec4 red = {1, 0, 0, 1};

			ImGui::Text("Target: %.6lf", target);
			ImGui::TextColored(target == 0 || frame <= target ? green : red, "Frame: %.6lf", frame);
			ImGui::TextColored(target == 0 || cpu <= target ? green : red, "CPU: %.6lf", cpu);
			ImGui::TextColored(target == 0 || gpu <= target ? green : red, "GPU: %.6lf", gpu);
			ImGui::TextColored(target == 0 || present <= target ? green : red, "Present: %.6lf", present);
			ImGui::Text("Draw Calls: %u", rRenderingStatistics.drawCalls);
			ImGui::Text("Triangles: %u", rRenderingStatistics.triangles);
			ImGui::End();
		}
	}

}

namespace FastCG
{
	BaseApplication *BaseApplication::smpInstance = nullptr;

#define FASTCG_REGISTER_COMPONENT(className, component)                    \
	if (component->GetType().IsDerived(className::TYPE))                   \
	{                                                                      \
		m##className##s.emplace_back(static_cast<className *>(component)); \
	}

#define FASTCG_UNREGISTER_COMPONENT(className, component)                               \
	if (component->GetType().IsDerived(className::TYPE))                                \
	{                                                                                   \
		auto it = std::find(m##className##s.begin(), m##className##s.end(), component); \
		if (it == m##className##s.end())                                                \
		{                                                                               \
			FASTCG_THROW_EXCEPTION(Exception, "Error unregistering: %s", #className);   \
		}                                                                               \
		m##className##s.erase(it);                                                      \
	}

	FASTCG_IMPLEMENT_SYSTEM(AssetSystem, AssetSystemArgs);
#ifdef _DEBUG
	FASTCG_IMPLEMENT_SYSTEM(DebugMenuSystem, DebugMenuSystemArgs);
#endif
	FASTCG_IMPLEMENT_SYSTEM(InputSystem, InputSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(ImGuiSystem, ImGuiSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(RenderingSystem, RenderingSystemArgs);

	BaseApplication::BaseApplication(const ApplicationSettings &settings) : mSettings(settings),
																			mWindowTitle(settings.windowTitle),
																			mScreenWidth(settings.screenWidth),
																			mScreenHeight(settings.screenHeight),
																			mClearColor(settings.clearColor),
																			mAmbientLight(settings.ambientLight),
																			mFrameRate(settings.frameRate),
																			mSecondsPerFrame(settings.frameRate == UNLOCKED_FRAMERATE ? 0 : 1 / (double)settings.frameRate),
																			mpRenderBatchStrategy(std::make_unique<RenderBatchStrategy>())
	{
		if (smpInstance != nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "There can only be one BaseApplication instance");
		}

		smpInstance = this;

		switch (mSettings.renderingPath)
		{
		case RenderingPath::FORWARD:
			mpWorldRenderer = std::unique_ptr<IWorldRenderer>(new ForwardWorldRenderer({mScreenWidth,
																						mScreenHeight,
																						mClearColor,
																						mAmbientLight,
																						mDirectionalLights,
																						mPointLights,
																						mpRenderBatchStrategy->GetRenderBatches(),
																						mRenderingStatistics}));
			break;
		case RenderingPath::DEFERRED:
			mpWorldRenderer = std::unique_ptr<IWorldRenderer>(new DeferredWorldRenderer({mScreenWidth,
																						 mScreenHeight,
																						 mClearColor,
																						 mAmbientLight,
																						 mDirectionalLights,
																						 mPointLights,
																						 mpRenderBatchStrategy->GetRenderBatches(),
																						 mRenderingStatistics}));
			break;
		default:
			FASTCG_THROW_EXCEPTION(Exception, "Unhandled rendering path: %d", (int)mSettings.renderingPath);
			break;
		}
		mpImGuiRenderer = std::make_unique<ImGuiRenderer>(ImGuiRendererArgs{mScreenWidth,
																			mScreenHeight});
	}

	BaseApplication::~BaseApplication()
	{
		auto gameObjectsToDestroy = mGameObjects;
		for (auto *pGameObject : gameObjectsToDestroy)
		{
			GameObject::Destroy(pGameObject);
		}

		mpMainCamera = nullptr;

		assert(mGameObjects.empty());
		assert(mCameras.empty());
		assert(mDirectionalLights.empty());
		assert(mPointLights.empty());
		assert(mRenderables.empty());
		assert(mBehaviours.empty());
		assert(mComponents.empty());

		smpInstance = nullptr;
	}

	void BaseApplication::RegisterGameObject(GameObject *pGameObject)
	{
		assert(pGameObject != nullptr);
		mGameObjects.emplace_back(pGameObject);
	}

	void BaseApplication::UnregisterGameObject(GameObject *pGameObject)
	{
		assert(pGameObject != nullptr);
		auto it = std::find(mGameObjects.begin(), mGameObjects.end(), pGameObject);
		assert(it != mGameObjects.end());
		mGameObjects.erase(it);
	}

	void BaseApplication::RegisterComponent(Component *pComponent)
	{
		assert(pComponent != nullptr);

		FASTCG_REGISTER_COMPONENT(DirectionalLight, pComponent);
		FASTCG_REGISTER_COMPONENT(PointLight, pComponent);
		FASTCG_REGISTER_COMPONENT(Renderable, pComponent);
		FASTCG_REGISTER_COMPONENT(Behaviour, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			RegisterCamera(static_cast<Camera *>(pComponent));
		}
		else if (pComponent->GetType().IsExactly(Renderable::TYPE))
		{
			mpRenderBatchStrategy->AddRenderable(static_cast<Renderable *>(pComponent));
		}

		mComponents.emplace_back(pComponent);
	}

	void BaseApplication::RegisterCamera(Camera *pCamera)
	{
		mCameras.emplace_back(pCamera);
		SetMainCamera(pCamera);
	}

	void BaseApplication::UnregisterComponent(Component *pComponent)
	{
		assert(pComponent != nullptr);

		FASTCG_UNREGISTER_COMPONENT(Camera, pComponent);
		FASTCG_UNREGISTER_COMPONENT(DirectionalLight, pComponent);
		FASTCG_UNREGISTER_COMPONENT(PointLight, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Renderable, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Behaviour, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Component, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			if (mCameras.size() > 1)
			{
				SetMainCamera(mCameras[0]);
			}
		}
		else if (pComponent->GetType().IsExactly(Renderable::TYPE))
		{
			mpRenderBatchStrategy->RemoveRenderable(static_cast<Renderable *>(pComponent));
		}
	}

	void BaseApplication::SetMainCamera(Camera *pCamera)
	{
		if (pCamera != nullptr)
		{
			pCamera->SetEnabled(true);
		}

		for (auto *pOtherCamera : mCameras)
		{
			if (pOtherCamera == pCamera)
			{
				continue;
			}

			pOtherCamera->SetEnabled(false);
		}

		mpMainCamera = pCamera;
		if (mpMainCamera != nullptr)
		{
			mpMainCamera->SetAspectRatio(GetAspectRatio());
		}
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
		AssetSystem::Create({mSettings.assetBundles});
#ifdef _DEBUG
		DebugMenuSystem::Create({});
#endif
		InputSystem::Create({});
		ImGuiSystem::Create({mScreenWidth,
							 mScreenHeight});
		RenderingSystem::Create({mScreenWidth,
								 mScreenHeight});

		ImGuiSystem::GetInstance()->Initialize();
		RenderingSystem::GetInstance()->Initialize();

		ShaderLoader::LoadShaders(mSettings.renderingPath);

		mpWorldRenderer->Initialize();
		mpImGuiRenderer->Initialize();

		OnInitialize();
	}

	void BaseApplication::Finalize()
	{
		OnFinalize();

		mpImGuiRenderer->Finalize();
		mpWorldRenderer->Finalize();

		ImGuiSystem::GetInstance()->Finalize();
		RenderingSystem::GetInstance()->Finalize();

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

		InputSystem::GetInstance()->Swap();

		ImGuiSystem::GetInstance()->BeginFrame(frameDeltaTime);

		for (auto *pGameObject : mGameObjects)
		{
			if (pGameObject->GetTransform()->GetParent() != nullptr)
			{
				continue;
			}
			pGameObject->GetTransform()->Update();
		}

		for (auto *pBehaviour : mBehaviours)
		{
			pBehaviour->Update((float)cpuStart, (float)frameDeltaTime);
		}

		DeclareStatisticsWindow(mSecondsPerFrame, frameDeltaTime, mLastCpuElapsedTime, mLastGpuElapsedTime, RenderingSystem::GetInstance()->GetLastPresentElapsedTime(), mRenderingStatistics);
#ifdef _DEBUG
		DebugMenuSystem::GetInstance()->DrawMenu();
#endif

		ImGuiSystem::GetInstance()->EndFrame();

		mRenderingStatistics.Reset();

		auto *pRenderingContext = RenderingSystem::GetInstance()->CreateRenderingContext();
		pRenderingContext->Begin();
		{
			if (mpMainCamera != nullptr)
			{
				mpWorldRenderer->Render(mpMainCamera, pRenderingContext);
			}
			mpImGuiRenderer->Render(ImGui::GetDrawData(), pRenderingContext);
		}
		pRenderingContext->End();

		auto cpuEnd = mStartTimer.GetTime();
		mLastCpuElapsedTime = cpuEnd - cpuStart;

		RenderingSystem::GetInstance()->Present();

		mLastGpuElapsedTime = pRenderingContext->GetElapsedTime();

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
		if (mpMainCamera != nullptr)
		{
			mpMainCamera->SetAspectRatio(GetAspectRatio());
		}
		RenderingSystem::GetInstance()->Resize();
		mpWorldRenderer->Resize();
		OnResize();
	}
}