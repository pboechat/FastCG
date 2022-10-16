#include <FastCG/Thread.h>
#include <FastCG/TextureImporter.h>
#include <FastCG/ShaderRegistry.h>
#include <FastCG/RenderingSystem.h>
#include <FastCG/MouseButton.h>
#include <FastCG/ModelImporter.h>
#include <FastCG/MaterialBasedMeshBatchingStrategy.h>
#include <FastCG/KeyCode.h>
#include <FastCG/InputSystem.h>
#include <FastCG/FontRegistry.h>
#include <FastCG/Exception.h>
#include <FastCG/Colors.h>
#include <FastCG/Behaviour.h>
#include <FastCG/BaseApplication.h>

#include <cstdio>
#include <cassert>
#include <algorithm>

namespace FastCG
{
	const std::string SHADERS_FOLDER = "shaders";
	const std::string FORWARD_RENDERING_SHADERS_FOLDER = SHADERS_FOLDER + "/forward";
	const std::string DEFERRED_RENDERING_SHADERS_FOLDER = SHADERS_FOLDER + "/deferred";
	const std::string FONTS_FOLDER = "fonts";

	BaseApplication *BaseApplication::s_mpInstance = nullptr;

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

#define FASTCG_IMPLEMENT_SYSTEM(className, argsClassName) \
	static className *s_p##className = nullptr;           \
	void className::Create(const argsClassName &rArgs)    \
	{                                                     \
		s_p##className = new className(rArgs);            \
	}                                                     \
	void className::Destroy()                             \
	{                                                     \
		delete s_p##className;                            \
	}                                                     \
	className *className::GetInstance()                   \
	{                                                     \
		return s_p##className;                            \
	}

	FASTCG_IMPLEMENT_SYSTEM(RenderingSystem, RenderingSystemArgs);
	FASTCG_IMPLEMENT_SYSTEM(InputSystem, InputSystemArgs);

	BaseApplication::BaseApplication(const ApplicationSettings &settings) : mSettings(settings),
																			mWindowTitle(settings.windowTitle),
																			mScreenWidth(settings.screenWidth),
																			mScreenHeight(settings.screenHeight),
																			mAssetsPath(settings.assetsPath),
																			mClearColor(settings.clearColor),
																			mAmbientLight(settings.ambientLight),
																			mShowFPS(settings.showFPS),
																			mShowRenderingStatistics(settings.showRenderingStatistics),
																			mSecondsPerFrame(1.0 / (double)settings.frameRate)
	{
		if (s_mpInstance != nullptr)
		{
			FASTCG_THROW_EXCEPTION(Exception, "There can only be one BaseApplication instance");
		}

		s_mpInstance = this;
	}

	BaseApplication::~BaseApplication()
	{
		ShaderRegistry::Unload();
		FontRegistry::Unload();

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
		assert(mMeshFilters.empty());
		assert(mBehaviours.empty());
		assert(mLineRenderers.empty());
		assert(mPointsRenderers.empty());
		assert(mComponents.empty());

		mMeshBatches.clear();

		ModelImporter::Dispose();

		s_mpInstance = nullptr;
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
		FASTCG_REGISTER_COMPONENT(MeshFilter, pComponent);
		FASTCG_REGISTER_COMPONENT(LineRenderer, pComponent);
		FASTCG_REGISTER_COMPONENT(PointsRenderer, pComponent);
		FASTCG_REGISTER_COMPONENT(Behaviour, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			RegisterCamera(static_cast<Camera *>(pComponent));
		}
		else if (pComponent->GetType().IsExactly(MeshFilter::TYPE))
		{
			mpMeshBatchingStrategy->AddMeshFilter(static_cast<MeshFilter *>(pComponent));
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
		FASTCG_UNREGISTER_COMPONENT(LineRenderer, pComponent);
		FASTCG_UNREGISTER_COMPONENT(PointsRenderer, pComponent);
		FASTCG_UNREGISTER_COMPONENT(MeshFilter, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Behaviour, pComponent);
		FASTCG_UNREGISTER_COMPONENT(Component, pComponent);

		if (pComponent->GetType().IsExactly(Camera::TYPE) && pComponent->IsEnabled())
		{
			if (mCameras.size() > 1)
			{
				SetMainCamera(mCameras[0]);
			}
		}
		else if (pComponent->GetType().IsExactly(MeshFilter::TYPE))
		{
			mpMeshBatchingStrategy->RemoveMeshFilter(static_cast<MeshFilter *>(pComponent));
		}
	}

	void BaseApplication::SetMainCamera(Camera *pCamera)
	{
		pCamera->SetEnabled(true);

		for (auto *pOtherCamera : mCameras)
		{
			if (pOtherCamera == pCamera)
			{
				continue;
			}

			pOtherCamera->SetEnabled(false);
		}

		mpMainCamera = pCamera;
		mpMainCamera->SetAspectRatio(GetAspectRatio());
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
		InputSystem::Create({});
		RenderingSystem::Create({mSettings.renderingPath,
								 mScreenWidth,
								 mScreenHeight,
								 mClearColor,
								 mAmbientLight,
								 mDirectionalLights,
								 mPointLights,
								 mLineRenderers,
								 mPointsRenderers,
								 mMeshBatches,
								 mRenderingStatistics});

		InitializePresentation();

		RenderingSystem::GetInstance()->Initialize();

		ShaderRegistry::LoadShadersFromDisk(mAssetsPath + "/" + SHADERS_FOLDER);

		switch (mSettings.renderingPath)
		{
		case RenderingPath::RP_FORWARD_RENDERING:
			ShaderRegistry::LoadShadersFromDisk(mAssetsPath + "/" + FORWARD_RENDERING_SHADERS_FOLDER);
			break;
		case RenderingPath::RP_DEFERRED_RENDERING:
			ShaderRegistry::LoadShadersFromDisk(mAssetsPath + "/" + DEFERRED_RENDERING_SHADERS_FOLDER);
			break;
		default:
			break;
		}

		FontRegistry::LoadFontsFromDisk(mAssetsPath + "/" + FONTS_FOLDER);

		TextureImporter::SetBasePath(mAssetsPath);

		ModelImporter::SetBasePath(mAssetsPath);

		mpMeshBatchingStrategy = std::make_unique<MaterialBasedMeshBatchingStrategy>(mMeshBatches);

		RenderingSystem::GetInstance()->Start();

		mpInternalGameObject = GameObject::Instantiate();
		Camera::Instantiate(mpInternalGameObject);

		OnInitialize();
	}

	void BaseApplication::Finalize()
	{
		OnFinalize();

		RenderingSystem::GetInstance()->Finalize();

		FinalizePresentation();

		RenderingSystem::Destroy();
		InputSystem::Destroy();
	}

	bool BaseApplication::ParseCommandLineArguments(int argc, char **argv)
	{
		return true;
	}

	void BaseApplication::Update()
	{
		auto currTime = mStartTimer.GetTime();
		double deltaTime;
		if (mLastFrameTime != 0)
		{
			deltaTime = currTime - mLastFrameTime;
		}
		else
		{
			deltaTime = 0;
		}

		InputSystem::GetInstance()->Swap();

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
			pBehaviour->Update((float)mStartTimer.GetTime(), (float)deltaTime);
		}

		if (mShowFPS)
		{
			ShowFPS();
		}

		if (mShowRenderingStatistics)
		{
			ShowRenderingStatistics();
		}

		Render();

		mLastFrameTime = currTime;

		mTotalElapsedTime += deltaTime;
		mElapsedFrames++;

		Present();

		auto idleTime = mSecondsPerFrame - deltaTime;
		if (idleTime > 0)
		{
			Thread::Sleep(idleTime);
			deltaTime += idleTime;
		}
	}

	void BaseApplication::ShowFPS()
	{
		char fpsText[128];

		sprintf_s(fpsText, FASTCG_ARRAYSIZE(fpsText), "FPS: %.3f", mElapsedFrames / mTotalElapsedTime);
		RenderingSystem::GetInstance()->DrawDebugText(fpsText, mScreenWidth - 240, 17, Colors::LIME);
	}

	void BaseApplication::ShowRenderingStatistics()
	{
		char renderStatsText[128];

		sprintf_s(renderStatsText, FASTCG_ARRAYSIZE(renderStatsText), "Draw Calls: %zu", mRenderingStatistics.drawCalls);
		RenderingSystem::GetInstance()->DrawDebugText(renderStatsText, mScreenWidth - 240, 34, Colors::LIME);

		sprintf_s(renderStatsText, FASTCG_ARRAYSIZE(renderStatsText), "No. Triangles: %zu", mRenderingStatistics.numberOfTriangles);
		RenderingSystem::GetInstance()->DrawDebugText(renderStatsText, mScreenWidth - 240, 51, Colors::LIME);
	}

	void BaseApplication::Render()
	{
		RenderingSystem::GetInstance()->Render(mpMainCamera);
		RenderingSystem::GetInstance()->DrawDebugTexts();
	}

	void BaseApplication::BeforeMeshFilterChange(MeshFilter *pMeshFilter)
	{
		mpMeshBatchingStrategy->RemoveMeshFilter(pMeshFilter);
	}

	void BaseApplication::AfterMeshFilterChange(MeshFilter *pMeshFilter)
	{
		mpMeshBatchingStrategy->AddMeshFilter(pMeshFilter);
	}

	void BaseApplication::MouseButtonCallback(MouseButton button, MouseButtonState state, int x, int y)
	{
		InputSystem::GetInstance()->SetMouseButton(button, state);
		OnMouseButton(button, state, x, y);
	}

	void BaseApplication::MouseWheelCallback(int direction, int x, int y)
	{
		if (direction == 1)
		{
			InputSystem::GetInstance()->IncrementMouseWheelDelta();
		}
		else if (direction == -1)
		{
			InputSystem::GetInstance()->DecrementMouseWheelDelta();
		}
		OnMouseWheel(direction, x, y);
	}

	void BaseApplication::MouseMoveCallback(int x, int y)
	{
		InputSystem::GetInstance()->SetMousePosition(glm::vec2((float)x, (float)(mScreenHeight - y)));
		OnMouseMove(x, y);
	}

	void BaseApplication::KeyboardCallback(int keyCode, bool pressed)
	{
		InputSystem::GetInstance()->SetKey(keyCode, pressed);
		if (pressed)
		{
			OnKeyPress(keyCode);
		}
		else
		{
			OnKeyRelease(keyCode);
		}
	}
}